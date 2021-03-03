#include "GraphicsImpl.h"
#include "GraphicsPlatform.h"

#include <JsRuntimeInternalState.h>

namespace Babylon
{
    namespace
    {
        constexpr auto JS_GRAPHICS_READY_NAME = "whenGraphicsReady";
    }

    Graphics::Impl::Impl()
        : Callback{[this](const auto& data) { CaptureCallback(data); }}
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Initialized = false;

        auto& init = m_state.Bgfx.InitState;
        init.type = BgfxDefaultRendererType;
        init.resolution.reset = BGFX_RESET_FLAGS;
        init.callback = &Callback;
    }

    Graphics::Impl::~Impl()
    {
        DisableRendering();
    }

    void* Graphics::Impl::GetNativeWindow()
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Bgfx.InitState.platformData.nwh;
    }

    void Graphics::Impl::SetNativeWindow(void* nativeWindowPtr, void* windowTypePtr, void* contextPtr, void* backBufferPtr, void* depthBufferPtr)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;

        auto& pd = m_state.Bgfx.InitState.platformData;
        pd.ndt = windowTypePtr;
        pd.nwh = nativeWindowPtr;
        pd.context = contextPtr;
        pd.backBuffer = backBufferPtr;
        pd.backBufferDS = depthBufferPtr;
    }

    void Graphics::Impl::Resize(size_t width, size_t height)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Resolution.Width = width;
        m_state.Resolution.Height = height;
        UpdateBgfxResolution();
    }

    void Graphics::Impl::UpdateBgfxResolution()
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        auto& res = m_state.Bgfx.InitState.resolution;
        auto level = m_state.Resolution.HardwareScalingLevel;
        res.width = static_cast<uint32_t>(m_state.Resolution.Width / level);
        res.height = static_cast<uint32_t>(m_state.Resolution.Height / level);
    }

    void Graphics::Impl::AddRenderWorkTask(arcana::task<void, std::exception_ptr> renderWorkTask)
    {
        std::scoped_lock RenderWorkTasksLock{m_renderWorkTasksMutex};
        m_renderWorkTasks.push_back(std::move(renderWorkTask));
    }

    arcana::task<void, std::exception_ptr> Graphics::Impl::GetBeforeRenderTask()
    {
        return m_beforeRenderTaskCompletionSource.as_task();
    }

    arcana::task<void, std::exception_ptr> Graphics::Impl::GetAfterRenderTask()
    {
        return m_afterRenderTaskCompletionSource.as_task();
    }

    void Graphics::Impl::EnableRendering()
    {
        std::scoped_lock lock{m_state.Mutex};

        if (!m_state.Bgfx.Initialized)
        {
            auto& init{m_state.Bgfx.InitState};

            // Set the thread affinity (all other rendering operations must happen on this thread).
            // If the context is not null, it means the device was created externally
            // and you need to call RenderCurrentFrame() on the thread in which it was created.
            if (init.platformData.context == nullptr)
                m_renderThreadAffinity = std::this_thread::get_id();

            // Initialize bgfx.
            bgfx::setPlatformData(init.platformData);
            bgfx::init(init);
            bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
            bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(init.resolution.width), static_cast<uint16_t>(init.resolution.height));
            bgfx::touch(0);

            m_state.Bgfx.Initialized = true;
            m_state.Bgfx.Dirty = false;

            // Allow JS awaiters to start doing graphics operations.
            m_enableRenderTaskCompletionSource.complete();
        }
    }

    void Graphics::Impl::DisableRendering()
    {
        assert(m_renderThreadAffinity.check());

        std::scoped_lock lock{m_state.Mutex};

        if (m_state.Bgfx.Initialized)
        {
            bgfx::shutdown();
            m_state.Bgfx.Initialized = false;
            m_enableRenderTaskCompletionSource = {};
            m_renderThreadAffinity = {};
        }
    }

    void Graphics::Impl::StartRenderingCurrentFrame()
    {
        assert(m_renderThreadAffinity.check());

        if (m_rendering)
        {
            throw std::runtime_error{"Current frame cannot be started before prior frame has been finished."};
        }
        m_rendering = true;

        // Ensure rendering is enabled
        EnableRendering();

        {
            std::scoped_lock lock{m_state.Mutex};
            if (m_state.Bgfx.Dirty)
            {
                bgfx::setPlatformData(m_state.Bgfx.InitState.platformData);
                auto& res = m_state.Bgfx.InitState.resolution;

                // Ensure bgfx rebinds all texture information.
                bgfx::discard(BGFX_DISCARD_ALL);
                bgfx::reset(res.width, res.height, res.reset);
                bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(res.width), static_cast<uint16_t>(res.height));

#if __APPLE__
                bgfx::frame();
#else
                bgfx::touch(0);
#endif

                m_state.Bgfx.Dirty = false;
            }
        }

        auto oldBeforeRenderTaskCompletionSource = m_beforeRenderTaskCompletionSource;
        m_beforeRenderTaskCompletionSource = {};
        oldBeforeRenderTaskCompletionSource.complete();
    }

    void Graphics::Impl::FinishRenderingCurrentFrame()
    {
        assert(m_renderThreadAffinity.check());

        if (!m_rendering)
        {
            throw std::runtime_error{"Current frame cannot be finished prior to having been started."};
        }

        bool finished{false};
        bool workDone{false};
        std::exception_ptr error{};
        RenderCurrentFrameAsync(finished, workDone, error);
        while (!finished)
        {
            m_renderWorkDispatcher.blocking_tick(arcana::cancellation::none());
        }

        if (error != nullptr)
        {
            std::rethrow_exception(error);
        }

        if (workDone)
        {
            {
                std::scoped_lock lock{m_state.Mutex};
                if (m_state.Bgfx.Dirty)
                {
                    bgfx::discard();
                }
            }

            bgfx::frame();
        }

        auto oldRenderTaskCompletionSource = m_afterRenderTaskCompletionSource;
        m_afterRenderTaskCompletionSource = {};
        oldRenderTaskCompletionSource.complete();

        m_rendering = false;
    }

    arcana::task<void, std::exception_ptr> Graphics::Impl::RenderCurrentFrameAsync(bool& finished, bool& workDone, std::exception_ptr& error)
    {
        bool anyTasks{};
        arcana::task<void, std::exception_ptr> whenAllTask{};
        {
            std::scoped_lock RenderWorkTasksLock{m_renderWorkTasksMutex};
            anyTasks = !m_renderWorkTasks.empty();
            if (anyTasks)
            {
                whenAllTask = arcana::when_all<std::exception_ptr>(m_renderWorkTasks);
                m_renderWorkTasks.clear();
            }
        }

        workDone = workDone || anyTasks;

        if (!anyTasks)
        {
            finished = true;
            return arcana::task_from_result<std::exception_ptr>();
        }
        else
        {
            return whenAllTask.then(m_renderWorkDispatcher, arcana::cancellation::none(), [this, &finished, &workDone, &error](const arcana::expected<void, std::exception_ptr>& result) mutable {
                if (result.has_error())
                {
                    finished = true;
                    error = result.error();
                    return arcana::task_from_result<std::exception_ptr>();
                }

                return RenderCurrentFrameAsync(finished, workDone, error);
            });
        }
    }

    void Graphics::Impl::SetDiagnosticOutput(std::function<void(const char* output)> outputFunction)
    {
        Callback.SetDiagnosticOutput(std::move(outputFunction));
    }

    float Graphics::Impl::GetHardwareScalingLevel()
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.HardwareScalingLevel;
    }

    void Graphics::Impl::SetHardwareScalingLevel(float level)
    {
        if (level <= std::numeric_limits<float>::epsilon())
        {
            throw std::runtime_error{"HardwareScalingValue cannot be less than or equal to 0."};
        }
        {
            std::scoped_lock lock{m_state.Mutex};
            m_state.Resolution.HardwareScalingLevel = level;
        }

        UpdateBgfxResolution();
    }

    Graphics::Impl::CaptureCallbackTicketT Graphics::Impl::AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback)
    {
        // If we're not already capturing, start.
        {
            std::scoped_lock lock{m_state.Mutex};
            if ((m_state.Bgfx.InitState.resolution.reset & BGFX_RESET_CAPTURE) == 0)
            {
                m_state.Bgfx.Dirty = true;
                m_state.Bgfx.InitState.resolution.reset |= BGFX_RESET_CAPTURE;
            }
        }

        return m_captureCallbacks.insert(std::move(callback), m_captureCallbacksMutex);
    }

    void Graphics::Impl::CaptureCallback(const BgfxCallback::CaptureData& data)
    {
        std::scoped_lock callbackLock{m_captureCallbacksMutex};

        // If no one is listening anymore, stop capturing.
        if (m_captureCallbacks.empty())
        {
            std::scoped_lock stateLock{m_state.Mutex};
            m_state.Bgfx.Dirty = true;
            m_state.Bgfx.InitState.resolution.reset &= ~BGFX_RESET_CAPTURE;
            return;
        }

        for (const auto& callback : m_captureCallbacks)
        {
            callback(data);
        }
    }

    void Graphics::Impl::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_GRAPHICS_NAME, Napi::External<Impl>::New(env, this));

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_GRAPHICS_READY_NAME,
            Napi::Function::New(
                env, [this, env](const Napi::CallbackInfo&) -> Napi::Value {
                    auto deferred{Napi::Promise::Deferred::New(env)};
                    auto promise{deferred.Promise()};

                    arcana::task<void, std::exception_ptr> enableRenderTask;
                    {
                        std::scoped_lock lock{m_state.Mutex};
                        enableRenderTask = m_enableRenderTaskCompletionSource.as_task();
                    }

                    auto& jsRuntime{JsRuntime::GetFromJavaScript(env)};
                    enableRenderTask.then(arcana::inline_scheduler, JsRuntime::InternalState::GetFromJavaScript(env).Cancellation, [&jsRuntime, deferred{std::move(deferred)}]() mutable {
                        jsRuntime.Dispatch([deferred{std::move(deferred)}](Napi::Env env) {
                            deferred.Resolve(env.Null());
                        });
                    });

                    return std::move(promise);
                },
                JS_GRAPHICS_READY_NAME));
    }

    Graphics::Impl& Graphics::Impl::GetFromJavaScript(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
                    .Get(JS_GRAPHICS_NAME)
                    .As<Napi::External<Graphics::Impl>>()
                    .Data();
    }
}
