#pragma once

#include <Babylon/Graphics.h>
#include "BgfxCallback.h"

#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>
#include <arcana/threading/affinity.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace Babylon
{
    class Graphics::Impl
    {
        static constexpr auto JS_GRAPHICS_NAME = "_Graphics";

    public:
        Impl();
        ~Impl();

        void* GetNativeWindow();
        void SetNativeWindow(void* nativeWindowPtr, void* windowTypePtr);
        void Resize(size_t width, size_t height);

        void AddToJavaScript(Napi::Env);
        static Impl& GetFromJavaScript(Napi::Env);

        void AddRenderWorkTask(arcana::task<void, std::exception_ptr> renderWorkTask);
        arcana::task<void, std::exception_ptr> GetBeforeRenderTask();
        arcana::task<void, std::exception_ptr> GetAfterRenderTask();

        void EnableRendering();
        void DisableRendering();

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        void RenderCurrentFrame()
        {
            StartRenderingCurrentFrame();
            FinishRenderingCurrentFrame();
        }

        void SetDiagnosticOutput(std::function<void(const char* output)> outputFunction);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        BgfxCallback Callback{};

    private:
        arcana::affinity m_renderThreadAffinity{};
        void UpdateBgfxResolution();

        bool m_rendering{false};

        struct
        {
            std::mutex Mutex{};

            struct
            {
                bgfx::Init InitState{};
                bool Initialized{};
                bool Dirty{};
            } Bgfx{};

            struct
            {
                std::mutex Mutex{};
                size_t width{};
                size_t height{};
                float hardwareScalingLevel{1.0f};
            } Resolution{};
        } m_state{};



        arcana::task_completion_source<void, std::exception_ptr> m_enableRenderTaskCompletionSource{};
        arcana::task_completion_source<void, std::exception_ptr> m_beforeRenderTaskCompletionSource{};
        arcana::task_completion_source<void, std::exception_ptr> m_afterRenderTaskCompletionSource{};

        arcana::manual_dispatcher<128> m_renderWorkDispatcher{};
        std::vector<arcana::task<void, std::exception_ptr>> m_renderWorkTasks{};
        std::mutex m_renderWorkTasksMutex{};

        arcana::task<void, std::exception_ptr> RenderCurrentFrameAsync(bool& finished, bool& workDone, std::exception_ptr& error);
    };
}
