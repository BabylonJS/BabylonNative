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
        template<typename CallableT>
        Impl(CallableT initializer)
        {
            // TODO: THIS IS WRONG
            arcana::make_task(m_renderWorkDispatcher, arcana::cancellation::none(), [this, initializer = std::move(initializer)] {
                initializer(*this);
            });
        }

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

        arcana::task_completion_source<void, std::exception_ptr> m_beforeRenderTaskCompletionSource{};
        arcana::task_completion_source<void, std::exception_ptr> m_afterRenderTaskCompletionSource{};

        arcana::manual_dispatcher<128> m_renderWorkDispatcher{};
        std::vector<arcana::task<void, std::exception_ptr>> m_renderWorkTasks{};
        std::mutex m_renderWorkTasksMutex{};

        arcana::task<void, std::exception_ptr> RenderCurrentFrameAsync(bool& finished, bool& workDone);
    };
}
