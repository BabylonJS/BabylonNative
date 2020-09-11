#pragma once

#include <Babylon/Graphics.h>
#include "BgfxCallback.h"

#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace Babylon
{
    class Graphics::Impl
    {
    public:
        static Graphics::Impl& GetImpl(Graphics& graphics)
        {
            return *graphics.m_impl;
        }

        ~Impl();

        std::unique_ptr<Graphics::Frame> AdvanceFrame();

        void AddRenderWorkTask(arcana::task<void, std::exception_ptr> renderWorkTask);
        arcana::task<void, std::exception_ptr> GetBeforeRenderTask();
        arcana::task<void, std::exception_ptr> GetAfterRenderTask();

        BgfxCallback BgfxCallback{};

    private:
        friend Graphics::Frame;

        arcana::manual_dispatcher<128> Dispatcher{};
        arcana::task_completion_source<void, std::exception_ptr> BeforeRenderTaskCompletionSource{};
        arcana::task_completion_source<void, std::exception_ptr> AfterRenderTaskCompletionSource{};

        std::vector<arcana::task<void, std::exception_ptr>> RenderWorkTasks{};
        std::mutex RenderWorkTasksMutex{};

        arcana::task<void, std::exception_ptr> RenderTask(bool& finished, bool& workDone);
    };
}
