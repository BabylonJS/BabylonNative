#include "GraphicsImpl.h"

#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)

namespace Babylon
{
    Graphics::Impl::~Impl()
    {
        bgfx::shutdown();
    }

    void Graphics::Impl::AddRenderWorkTask(arcana::task<void, std::exception_ptr> renderWorkTask)
    {
        std::scoped_lock RenderWorkTasksLock{RenderWorkTasksMutex};
        RenderWorkTasks.push_back(std::move(renderWorkTask));
    }

    arcana::task<void, std::exception_ptr> Graphics::Impl::GetBeforeRenderTask()
    {
        return BeforeRenderTaskCompletionSource.as_task();
    }

    arcana::task<void, std::exception_ptr> Graphics::Impl::GetAfterRenderTask()
    {
        return AfterRenderTaskCompletionSource.as_task();
    }

    void Graphics::Impl::StartRenderingCurrentFrame()
    {
        if (m_rendering)
        {
            throw std::runtime_error{"Current frame cannot be started before prior frame has been finished."};
        }
        m_rendering = true;

        auto oldBeforeRenderTaskCompletionSource = BeforeRenderTaskCompletionSource;
        BeforeRenderTaskCompletionSource = {};
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
        RenderCurrentFrameAsync(finished, workDone);
        while (!finished)
        {
            Dispatcher.blocking_tick(arcana::cancellation::none());
        }

        if (workDone)
        {
            bgfx::frame();
        }

        auto oldRenderTaskCompletionSource = AfterRenderTaskCompletionSource;
        AfterRenderTaskCompletionSource = {};
        oldRenderTaskCompletionSource.complete();

        m_rendering = false;
    }

    arcana::task<void, std::exception_ptr> Graphics::Impl::RenderCurrentFrameAsync(bool& finished, bool& workDone)
    {
        bool anyTasks{};
        arcana::task<void, std::exception_ptr> whenAllTask{};
        {
            std::scoped_lock RenderWorkTasksLock{RenderWorkTasksMutex};
            anyTasks = !RenderWorkTasks.empty();
            if (anyTasks)
            {
                whenAllTask = arcana::when_all<std::exception_ptr>(RenderWorkTasks);
                RenderWorkTasks.clear();
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
            return whenAllTask.then(Dispatcher, arcana::cancellation::none(), [this, &finished, &workDone]() mutable {
                return RenderCurrentFrameAsync(finished, workDone);
            });
        }
    }

    Graphics::Graphics()
        : m_impl{std::make_unique<Graphics::Impl>()}
    {
    }

    Graphics::~Graphics() = default;

    template<>
    std::unique_ptr<Graphics> Graphics::InitializeFromWindow<void*>(void* nativeWindowPtr, size_t width, size_t height)
    {
        std::unique_ptr<Graphics> graphics{new Graphics()};

        // Initialize bgfx.
        bgfx::Init init{};
        init.platformData.nwh = nativeWindowPtr;
        bgfx::setPlatformData(init.platformData);
#if (ANDROID)
        init.type = bgfx::RendererType::OpenGLES;
#else
        init.type = bgfx::RendererType::Direct3D11;
#endif
        init.resolution.width = static_cast<uint32_t>(width);
        init.resolution.height = static_cast<uint32_t>(height);
        init.resolution.reset = BGFX_RESET_FLAGS;
        init.callback = &graphics->m_impl->BgfxCallback;
        bgfx::init(init);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(init.resolution.width), static_cast<uint16_t>(init.resolution.height));
        bgfx::touch(0);

        return graphics;
    }

    template<>
    void Graphics::ReinitializeFromWindow<void*>(void* windowPtr, size_t width, size_t height)
    {
        bgfx::PlatformData pd{};
        pd.ndt = nullptr;
        pd.nwh = windowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
        bgfx::setPlatformData(pd);
        bgfx::reset(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    void Graphics::Impl::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_GRAPHICS_NAME, Napi::External<Impl>::New(env, this));
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

    void Graphics::UpdateSize(size_t width, size_t height)
    {
        m_impl->GetAfterRenderTask().then(arcana::inline_scheduler, arcana::cancellation::none(), [width, height] {
            const auto w = static_cast<uint16_t>(width);
            const auto h = static_cast<uint16_t>(height);

            auto bgfxStats = bgfx::getStats();
            if (w != bgfxStats->width || h != bgfxStats->height)
            {
                bgfx::reset(w, h, BGFX_RESET_FLAGS);
                bgfx::setViewRect(0, 0, 0, w, h);
#ifdef __APPLE__
                bgfx::frame();
#else
                    bgfx::touch(0);
#endif
            }
        });
    }
}