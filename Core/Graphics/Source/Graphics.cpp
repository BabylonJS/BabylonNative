#include "GraphicsImpl.h"

#include <JsRuntimeInternalState.h>

#include <cassert>

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
        constexpr auto JS_GRAPHICS_NAME = "_Graphics";
        constexpr auto JS_GRAPHICS_READY_NAME = "whenGraphicsReady";
    }

    // Forward declares of important specializations.
    // clang-format off
    template<> std::unique_ptr<Graphics> Graphics::CreateGraphics<void*, size_t, size_t>(void*, size_t, size_t);
    template<> void Graphics::UpdateWindow<void*>(void*);
    // clang-format on

    Graphics::Impl::Impl()
    {
        std::scoped_lock lock{m_bgfxState.Mutex};
        m_bgfxState.Initialized = false;

        auto& init = m_bgfxState.InitState;
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
        std::scoped_lock lock{m_bgfxState.Mutex};
        return m_bgfxState.InitState.platformData.nwh;
    }

    void Graphics::Impl::SetNativeWindow(void* nativeWindowPtr)
    {
        std::scoped_lock lock{m_bgfxState.Mutex};
        m_bgfxState.Dirty = true;

        auto& pd = m_bgfxState.InitState.platformData;
        pd.ndt = nullptr;
        pd.nwh = nativeWindowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
    }

    void Graphics::Impl::Resize(size_t width, size_t height)
    {
        std::scoped_lock lock{m_bgfxState.Mutex};
        m_bgfxState.Dirty = true;

        auto& res = m_bgfxState.InitState.resolution;
        res.width = static_cast<uint32_t>(width);
        res.height = static_cast<uint32_t>(height);
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
        std::scoped_lock lock{m_bgfxState.Mutex};

        if (!m_bgfxState.Initialized)
        {
            // Set the thread affinity (all other rendering operations must happen on this thread).
            m_renderThreadAffinity = std::this_thread::get_id();

            // Initialize bgfx.
            auto& init{m_bgfxState.InitState};
            bgfx::setPlatformData(init.platformData);
            bgfx::init(init);
            bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
            bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(init.resolution.width), static_cast<uint16_t>(init.resolution.height));
            bgfx::touch(0);

            m_bgfxState.Initialized = true;
            m_bgfxState.Dirty = false;

            // Allow JS awaiters to start doing graphics operations.
            m_enableRenderTaskCompletionSource.complete();
        }
    }

    void Graphics::Impl::DisableRendering()
    {
        assert(m_renderThreadAffinity.check());

        std::scoped_lock lock{m_bgfxState.Mutex};

        if (m_bgfxState.Initialized)
        {
            bgfx::shutdown();
            m_bgfxState.Initialized = false;
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
            std::scoped_lock lock{m_bgfxState.Mutex};
            if (m_bgfxState.Dirty)
            {
                bgfx::setPlatformData(m_bgfxState.InitState.platformData);
                auto& res = m_bgfxState.InitState.resolution;
                bgfx::reset(res.width, res.height, BGFX_RESET_FLAGS);
                bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(res.width), static_cast<uint16_t>(res.height));

#if __APPLE__
                bgfx::frame();
#else
                bgfx::touch(0);
#endif

                m_bgfxState.Dirty = false;
            }
        }

        auto oldBeforeRenderTaskCompletionSource = m_beforeRenderTaskCompletionSource;
        m_beforeRenderTaskCompletionSource = {};

        // callbacks
        for (auto& callback : m_onBeginFrameCallbacks)
        {
            callback.second();
        }
        oldBeforeRenderTaskCompletionSource.complete();
    }

    void Graphics::Impl::FinishRenderingCurrentFrame()
    {
        assert(m_renderThreadAffinity.check());

        if (!m_rendering)
        {
            throw std::runtime_error{"Current frame cannot be finished prior to having been started."};
        }

        // callbacks
        for (auto& callback : m_onEndFrameCallbacks)
        {
            callback.second();
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
                std::scoped_lock lock{m_bgfxState.Mutex};
                if (m_bgfxState.Dirty)
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

    Graphics::Graphics()
        : m_impl{std::make_unique<Impl>()}
    {
    }

    Graphics::~Graphics() = default;

    template<>
    std::unique_ptr<Graphics> Graphics::CreateGraphics<void*, size_t, size_t>(void* nativeWindowPtr, size_t width, size_t height)
    {
        std::unique_ptr<Graphics> graphics{new Graphics()};
        graphics->UpdateWindow(nativeWindowPtr);
        graphics->UpdateSize(width, height);
        return graphics;
    }

    template<>
    void Graphics::UpdateWindow<void*>(void* windowPtr)
    {
        m_impl->SetNativeWindow(windowPtr);
    }

    void Graphics::UpdateSize(size_t width, size_t height)
    {
        m_impl->Resize(width, height);
    }

    void Graphics::Impl::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_GRAPHICS_IMPL_NAME, Napi::External<Impl>::New(env, this));

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_GRAPHICS_READY_NAME, Napi::Function::New(env, [this, env](const Napi::CallbackInfo&) -> Napi::Value {
            auto deferred{Napi::Promise::Deferred::New(env)};
            auto promise{deferred.Promise()};

            arcana::task<void, std::exception_ptr> enableRenderTask;
            {
                std::scoped_lock lock{m_bgfxState.Mutex};
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
                    .Get(JS_GRAPHICS_IMPL_NAME)
                    .As<Napi::External<Graphics::Impl>>()
                    .Data();
    }

    Graphics::CallbackHandle Graphics::Impl::RegisterOnBeginFrame(std::function<void()> callback)
    {
        static CallbackHandle callbackHandle{};
        callbackHandle++;

        m_onBeginFrameCallbacks.insert(std::make_pair(callbackHandle, callback));
        return callbackHandle;
    }

    void Graphics::Impl::UnregisterOnBeginFrame(Graphics::CallbackHandle callbackHandle)
    {
        auto iter = m_onBeginFrameCallbacks.find(callbackHandle);
        if (iter != m_onBeginFrameCallbacks.end())
        {
            m_onBeginFrameCallbacks.erase(iter);
        }
    }

    Graphics::CallbackHandle Graphics::Impl::RegisterOnEndFrame(std::function<void()> callback)
    {
        static CallbackHandle callbackHandle{};
        callbackHandle++;

        m_onEndFrameCallbacks.insert(std::make_pair(callbackHandle, callback));
        return callbackHandle;
    }

    void Graphics::Impl::UnregisterOnEndFrame(Graphics::CallbackHandle callbackHandle)
    {
        auto iter = m_onEndFrameCallbacks.find(callbackHandle);
        if (iter != m_onEndFrameCallbacks.end())
        {
            m_onEndFrameCallbacks.erase(iter);
        }
    }

    void Graphics::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_GRAPHICS_NAME, Napi::External<Graphics>::New(env, this));

        m_impl->AddToJavaScript(env);
    }

    Graphics& Graphics::GetFromJavaScript(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
            .Get(JS_GRAPHICS_NAME)
            .As<Napi::External<Graphics>>()
            .Data();
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

    Graphics::CallbackHandle Graphics::RegisterOnBeginFrame(std::function<void()> callback)
    {
        return m_impl->RegisterOnBeginFrame(callback);
    }

    void Graphics::UnregisterOnBeginFrame(Graphics::CallbackHandle callbackHandle)
    {
        m_impl->UnregisterOnBeginFrame(callbackHandle);
    }

    Graphics::CallbackHandle Graphics::RegisterOnEndFrame(std::function<void()> callback)
    {
        return m_impl->RegisterOnEndFrame(callback);
    }

    void Graphics::UnregisterOnEndFrame(Graphics::CallbackHandle callbackHandle)
    {
        m_impl->UnregisterOnEndFrame(callbackHandle);
    }

    void Graphics::SetDiagnosticOutput(std::function<void(const char* output)> outputFunction)
    {
        m_impl->SetDiagnosticOutput(std::move(outputFunction));
    }
}
