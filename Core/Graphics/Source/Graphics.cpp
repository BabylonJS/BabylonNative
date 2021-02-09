#include "GraphicsImpl.h"

#include <JsRuntimeInternalState.h>

#include <cassert>
#include <limits>

#if (ANDROID)
// MSAA is disabled on Android.
// See issue https://github.com/BabylonJS/BabylonNative/issues/494#issuecomment-731135918
// for explanation
#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MAXANISOTROPY)
#elif __APPLE__
// On Apple devices we need to set BGFX_RESET_FLIP_AFTER_RENDER so that draw calls are immediately pushed
// to the Metal command buffer to avoid a one frame delay.
#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY | BGFX_RESET_FLIP_AFTER_RENDER)
#else
#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)
#endif
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
#if (ANDROID)
        init.type = bgfx::RendererType::OpenGLES;
#else
        init.type = bgfx::RendererType::Direct3D11;
#endif
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

    void Graphics::Impl::SetNativeWindow(void* nativeWindowPtr, void* windowTypePtr)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;

        auto& pd = m_state.Bgfx.InitState.platformData;
        pd.ndt = windowTypePtr;
        pd.nwh = nativeWindowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
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
            // Set the thread affinity (all other rendering operations must happen on this thread).
            m_renderThreadAffinity = std::this_thread::get_id();

            // Initialize bgfx.
            auto& init{m_state.Bgfx.InitState};
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

    Graphics::Graphics()
        : m_impl{std::make_unique<Impl>()}
    {
    }

    Graphics::~Graphics() = default;

    template<>
    void Graphics::UpdateWindow<void*>(void* windowPtr)
    {
        m_impl->SetNativeWindow(windowPtr, nullptr);
    }

    template<>
    void Graphics::UpdateWindow<void*, void*>(void* windowPtr, void* windowTypePtr)
    {
        m_impl->SetNativeWindow(windowPtr, windowTypePtr);
    }

    template<>
    std::unique_ptr<Graphics> Graphics::CreateGraphics<void*, size_t, size_t>(void* nativeWindowPtr, size_t width, size_t height)
    {
        std::unique_ptr<Graphics> graphics{new Graphics()};
        graphics->UpdateWindow<void*>(nativeWindowPtr);
        graphics->UpdateSize(width, height);
        return graphics;
    }

    template<>
    std::unique_ptr<Graphics> Graphics::CreateGraphics<void*, void*, size_t, size_t>(void* nativeWindowPtr, void* nativeWindowTypePtr, size_t width, size_t height)
    {
        std::unique_ptr<Graphics> graphics{new Graphics()};
        graphics->UpdateWindow<void*, void*>(nativeWindowPtr, nativeWindowTypePtr);
        graphics->UpdateSize(width, height);
        return graphics;
    }

    void Graphics::UpdateSize(size_t width, size_t height)
    {
        m_impl->Resize(width, height);
    }

    void Graphics::Impl::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_GRAPHICS_NAME, Napi::External<Impl>::New(env, this));

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_GRAPHICS_READY_NAME, Napi::Function::New(env, [this, env](const Napi::CallbackInfo&) -> Napi::Value {
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
        }, JS_GRAPHICS_READY_NAME));
    }

    Graphics::Impl& Graphics::Impl::GetFromJavaScript(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
                    .Get(JS_GRAPHICS_NAME)
                    .As<Napi::External<Graphics::Impl>>()
                    .Data();
    }

    void Graphics::AddToJavaScript(Napi::Env env)
    {
        m_impl->AddToJavaScript(env);
    }

    void Graphics::EnableRendering()
    {
        m_impl->EnableRendering();
    }

    void Graphics::DisableRendering()
    {
        m_impl->DisableRendering();
    }

    void Graphics::StartRenderingCurrentFrame()
    {
        m_impl->StartRenderingCurrentFrame();
    }

    void Graphics::FinishRenderingCurrentFrame()
    {
        m_impl->FinishRenderingCurrentFrame();
    }

    void Graphics::SetDiagnosticOutput(std::function<void(const char* output)> outputFunction)
    {
        m_impl->SetDiagnosticOutput(std::move(outputFunction));
    }

    void Graphics::SetHardwareScalingLevel(float level)
    {
        m_impl->SetHardwareScalingLevel(level);
    }

    float Graphics::GetHardwareScalingLevel()
    {
        return m_impl->GetHardwareScalingLevel();
    }
}
