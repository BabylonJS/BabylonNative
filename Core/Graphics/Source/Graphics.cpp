#include "GraphicsImpl.h"

#include <JsRuntimeInternalState.h>

#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)

namespace Babylon
{
    namespace
    {
        constexpr auto JS_SENTINEL_NAME = "graphicsInitializationPromise";
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
        bgfx::shutdown();
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

    void Graphics::Impl::StartRenderingCurrentFrame()
    {
        if (m_rendering)
        {
            throw std::runtime_error{"Current frame cannot be started before prior frame has been finished."};
        }
        m_rendering = true;

        {
            std::scoped_lock lock{m_bgfxState.Mutex};

            if (!m_bgfxState.Initialized)
            {
                // Initialize bgfx.
                auto& init = m_bgfxState.InitState;
                bgfx::setPlatformData(init.platformData);
                bgfx::init(init);
                bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
                bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(init.resolution.width), static_cast<uint16_t>(init.resolution.height));
                bgfx::touch(0);

                m_bgfxState.Initialized = true;
                m_bgfxState.Dirty = false;
            }
            else if (m_bgfxState.Dirty)
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
        oldBeforeRenderTaskCompletionSource.complete();
    }

    void Graphics::Impl::FinishRenderingCurrentFrame()
    {
        if (!m_rendering)
        {
            throw std::runtime_error{"Current frame cannot be finished prior to having been started."};
        }

        bool finished = false;
        bool workDone = false;
        auto renderTask{RenderCurrentFrameAsync(finished, workDone)};
        while (!finished)
        {
            m_renderWorkDispatcher.blocking_tick(arcana::cancellation::none());
        }
        m_processedRenderWorkTasks.clear();

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

    arcana::task<void, std::exception_ptr> Graphics::Impl::RenderCurrentFrameAsync(bool& finished, bool& workDone)
    {
        bool anyTasks{};
        arcana::task<void, std::exception_ptr> whenAllTask{};
        {
            std::scoped_lock RenderWorkTasksLock{m_renderWorkTasksMutex};
            anyTasks = !m_renderWorkTasks.empty();
            if (anyTasks)
            {
                whenAllTask = arcana::when_all<std::exception_ptr>(m_renderWorkTasks);
                m_processedRenderWorkTasks.insert(m_processedRenderWorkTasks.end(), m_renderWorkTasks.begin(), m_renderWorkTasks.end());
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
            return whenAllTask.then(m_renderWorkDispatcher, arcana::cancellation::none(), [this, &finished, &workDone]() mutable {
                return RenderCurrentFrameAsync(finished, workDone);
            });
        }
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
            .Set(JS_GRAPHICS_NAME, Napi::External<Impl>::New(env, this));

        auto deferred = Napi::Promise::Deferred::New(env);
        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_SENTINEL_NAME, deferred.Promise());
        bool initialized;
        {
            std::scoped_lock lock{m_bgfxState.Mutex};
            initialized = m_bgfxState.Initialized;
        }
        if (initialized)
        {
            deferred.Resolve(env.Null());
        }
        else
        {
            auto& jsRuntime = JsRuntime::GetFromJavaScript(env);
            this->GetAfterRenderTask().then(arcana::inline_scheduler, JsRuntime::InternalState::GetFromJavaScript(env).Cancellation, [&jsRuntime, deferred = std::move(deferred)]() mutable {
                jsRuntime.Dispatch([deferred = std::move(deferred)](Napi::Env env) {
                    deferred.Resolve(env.Null());
                });
            });
        }
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

    void Graphics::StartRenderingCurrentFrame()
    {
        m_impl->StartRenderingCurrentFrame();
    }

    void Graphics::FinishRenderingCurrentFrame()
    {
        m_impl->FinishRenderingCurrentFrame();
    }
}
