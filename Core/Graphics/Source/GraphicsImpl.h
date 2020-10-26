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
        static constexpr auto JS_GRAPHICS_NAME = "_Graphics";

    public:
        ~Impl();

        void AddToJavaScript(Napi::Env);
        static Impl& GetFromJavaScript(Napi::Env);

        void AddRenderWorkTask(arcana::task<void, std::exception_ptr> renderWorkTask);
        arcana::task<void, std::exception_ptr> GetBeforeRenderTask();
        arcana::task<void, std::exception_ptr> GetAfterRenderTask();

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        void RenderCurrentFrame()
        {
            StartRenderingCurrentFrame();
            FinishRenderingCurrentFrame();
        }

        BgfxCallback Callback{};

    private:
        bool m_rendering{false};

        arcana::manual_dispatcher<128> Dispatcher{};
        arcana::task_completion_source<void, std::exception_ptr> BeforeRenderTaskCompletionSource{};
        arcana::task_completion_source<void, std::exception_ptr> AfterRenderTaskCompletionSource{};

        std::vector<arcana::task<void, std::exception_ptr>> RenderWorkTasks{};
        std::mutex RenderWorkTasksMutex{};

        arcana::task<void, std::exception_ptr> RenderCurrentFrameAsync(bool& finished, bool& workDone, std::exception_ptr& error);
    };
}
