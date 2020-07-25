#include "NativeGraphics.h"

#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)

namespace Babylon
{
    struct NativeGraphics::CallbackTicket::Impl
    {
        // TODO: Contain the ticket or whatever.
    };

    NativeGraphics::CallbackTicket::CallbackTicket()
        : m_impl{std::make_unique<NativeGraphics::CallbackTicket::Impl>()}
    {
    }

    struct NativeGraphics::Impl
    {
        ~Impl();

        // TODO: Populate this with something, probably not a pointer.
        std::unique_ptr<bgfx::CallbackI> BgfxCallback{};

        arcana::manual_dispatcher<128> Dispatcher{};
        arcana::task_completion_source<void, std::exception_ptr> RenderTaskCompletionSource{};

        std::vector<arcana::task<void, std::exception_ptr>> RenderWorkTasks{};
        std::mutex RenderWorkTasksMutex{};

        void AddRenderWorkTask(arcana::task<void, std::exception_ptr> renderWorkTask)
        {
            std::scoped_lock RenderWorkTasksLock{RenderWorkTasksMutex};
            RenderWorkTasks.push_back(std::move(renderWorkTask));
        }

        arcana::task<void, std::exception_ptr> GetRenderTask()
        {
            return RenderTaskCompletionSource.as_task();
        }

        void Render()
        {
            bool rendered = false;
            RenderTask([this, &rendered]() mutable {
                bgfx::frame();
                
                rendered = true;
                auto oldRenderTaskCompletionSource = RenderTaskCompletionSource;
                RenderTaskCompletionSource = {};
                oldRenderTaskCompletionSource.complete();
            });
            while (!rendered)
            {
                Dispatcher.blocking_tick(arcana::cancellation::none());
            }
        }

        arcana::task<void, std::exception_ptr> RenderTask(std::function<void()> render)
        {
            bool anyTasks{};
            arcana::task<void, std::exception_ptr> whenAllTask{};
            {
                std::scoped_lock RenderWorkTasksLock{RenderWorkTasksMutex};
                anyTasks = RenderWorkTasks.empty();
                if (anyTasks)
                {
                    whenAllTask = arcana::when_all<std::exception_ptr>(RenderWorkTasks);
                    RenderWorkTasks.clear();
                }
            }

            if (!anyTasks)
            {
                return arcana::make_task(Dispatcher, arcana::cancellation::none(), std::move(render));
            }
            else
            {
                return whenAllTask.then(Dispatcher, arcana::cancellation::none(), [this, render = std::move(render)]() mutable {
                    return RenderTask(std::move(render));
                });
            }
        }
    };

    NativeGraphics::Impl::~Impl()
    {
        bgfx::shutdown();
    }

    template<>
    NativeGraphics NativeGraphics::InitializeFromWindow<void*>(void* nativeWindowPtr, size_t width, size_t height)
    {
        NativeGraphics graphics{};

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
        init.callback = graphics.m_impl->BgfxCallback.get();
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
}