#include "NativeGraphicsImpl.h"

#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)

namespace Babylon
{
    NativeGraphics::Impl::~Impl()
    {
        bgfx::shutdown();
    }

    void NativeGraphics::Impl::AddRenderWorkTask(arcana::task<void, std::exception_ptr> renderWorkTask)
    {
        std::scoped_lock RenderWorkTasksLock{RenderWorkTasksMutex};
        RenderWorkTasks.push_back(std::move(renderWorkTask));
    }

    arcana::task<void, std::exception_ptr> NativeGraphics::Impl::GetRenderTask()
    {
        return RenderTaskCompletionSource.as_task();
    }

    void NativeGraphics::Impl::Render()
    {
        bool finished = false;
        bool workDone = false;
        RenderTask(finished, workDone);
        while (!finished)
        {
            Dispatcher.blocking_tick(arcana::cancellation::none());
        }

        if (workDone)
        {
            bgfx::frame();
        }

        auto oldRenderTaskCompletionSource = RenderTaskCompletionSource;
        RenderTaskCompletionSource = {};
        oldRenderTaskCompletionSource.complete();
    }

    arcana::task<void, std::exception_ptr> NativeGraphics::Impl::RenderTask(bool& finished, bool& workDone)
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
                return RenderTask(finished, workDone);
            });
        }
    }

    NativeGraphics::NativeGraphics()
        : m_impl{std::make_unique<NativeGraphics::Impl>()}
    {
    }

    NativeGraphics::~NativeGraphics() = default;

    template<>
    std::unique_ptr<NativeGraphics> NativeGraphics::InitializeFromWindow<void*>(void* nativeWindowPtr, size_t width, size_t height)
    {
        std::unique_ptr<NativeGraphics> graphics{new NativeGraphics()};

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
        //init.callback = graphics->m_impl->BgfxCallback.get();
        bgfx::init(init);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(init.resolution.width), static_cast<uint16_t>(init.resolution.height));
        bgfx::touch(0);

        return graphics;
    }

    template<>
    void NativeGraphics::ReinitializeFromWindow<void*>(void* windowPtr, size_t width, size_t height)
    {
        bgfx::PlatformData pd;
        pd.ndt = nullptr;
        pd.nwh = windowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
        bgfx::setPlatformData(pd);
        bgfx::reset(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    void NativeGraphics::Render()
    {
        m_impl->Render();
    }
}