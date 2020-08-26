#pragma once

#include <Babylon/NativeGraphics.h>
#include "BgfxCallback.h"

#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace Babylon
{
    class NativeGraphics::Impl
    {
    public:
        static NativeGraphics::Impl& GetImpl(NativeGraphics& graphics)
        {
            return *graphics.m_impl;
        }

        ~Impl();

        std::unique_ptr<NativeGraphics::Frame> AdvanceFrame();

        void AddRenderWorkTask(arcana::task<void, std::exception_ptr> renderWorkTask);
        arcana::task<void, std::exception_ptr> GetBeforeRenderTask();
        arcana::task<void, std::exception_ptr> GetAfterRenderTask();

        BgfxCallback BgfxCallback{};

    private:
        friend NativeGraphics::Frame;

        arcana::manual_dispatcher<128> Dispatcher{};
        arcana::task_completion_source<void, std::exception_ptr> BeforeRenderTaskCompletionSource{};
        arcana::task_completion_source<void, std::exception_ptr> AfterRenderTaskCompletionSource{};

        std::vector<arcana::task<void, std::exception_ptr>> RenderWorkTasks{};
        std::mutex RenderWorkTasksMutex{};

        arcana::task<void, std::exception_ptr> RenderTask(bool& finished, bool& workDone);
    };
}
