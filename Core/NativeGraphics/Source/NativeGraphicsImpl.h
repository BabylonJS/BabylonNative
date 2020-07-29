#pragma once

#include <Babylon/NativeGraphics.h>

#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace Babylon
{
    struct NativeGraphics::Impl
    {
        static NativeGraphics::Impl& GetImpl(NativeGraphics& graphics)
        {
            return *graphics.m_impl;
        }

        ~Impl();

        // TODO: Populate this with something, probably not a pointer.
        std::unique_ptr<bgfx::CallbackI> BgfxCallback{};

        arcana::manual_dispatcher<128> Dispatcher{};
        arcana::task_completion_source<void, std::exception_ptr> RenderTaskCompletionSource{};

        std::vector<arcana::task<void, std::exception_ptr>> RenderWorkTasks{};
        std::mutex RenderWorkTasksMutex{};

        void AddRenderWorkTask(arcana::task<void, std::exception_ptr> renderWorkTask);
        arcana::task<void, std::exception_ptr> GetRenderTask();
        void Render();

    private:
        arcana::task<void, std::exception_ptr> RenderTask(std::function<void()> render);
    };
}
