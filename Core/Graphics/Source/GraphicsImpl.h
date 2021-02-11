#pragma once

#include <Babylon/Graphics.h>
#include "BgfxCallback.h"
#include "FrameBufferManager.h"
#include "SafeTimespanGuarantor.h"

#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>
#include <arcana/threading/affinity.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/semaphore.h>

#include <memory>
#include <map>

namespace Babylon
{
    class Graphics::Impl
    {
    public:
        class UpdateToken final
        {
        public:
            UpdateToken(const UpdateToken& other) = delete;
            UpdateToken(UpdateToken&&) = default;

            bgfx::Encoder* GetEncoder();

            FrameBuffer& AddFrameBuffer(bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool backBuffer);
            void RemoveFrameBuffer(const FrameBuffer& frameBuffer);
            FrameBuffer& DefaultFrameBuffer();
            FrameBuffer& BoundFrameBuffer();

        private:
            friend class Graphics::Impl;

            UpdateToken(Graphics::Impl&);

            Impl& m_graphicsImpl;
            SafeTimespanGuarantor::SafetyGuarantee m_guarantee;
        };

        class RenderScheduler final
        {
        public:
            template<typename CallableT>
            void operator()(CallableT&& callable)
            {
                m_dispatcher(callable);
            }

        private:
            friend Impl;

            arcana::manual_dispatcher<128> m_dispatcher;
        };

        Impl();
        ~Impl();

        void* GetNativeWindow();
        void SetNativeWindow(void* nativeWindowPtr, void* windowTypePtr);
        void Resize(size_t width, size_t height);

        void AddToJavaScript(Napi::Env);
        static Impl& GetFromJavaScript(Napi::Env);

        RenderScheduler& BeforeRenderScheduler();
        RenderScheduler& AfterRenderScheduler();

        void EnableRendering();
        void DisableRendering();

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        UpdateToken GetUpdateToken();

        void SetDiagnosticOutput(std::function<void(const char* output)> diagnosticOutput);

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

    private:
        friend class UpdateToken;

        void UpdateBgfxState();
        void UpdateBgfxResolution();
        void DiscardIfDirty();
        void Frame();
        bgfx::Encoder* GetEncoderForThread();
        void EndEncoders();

        arcana::affinity m_renderThreadAffinity{};

        arcana::cancellation_source m_cancellationSource{};

        struct
        {
            std::recursive_mutex Mutex{};

            struct
            {
                bgfx::Init InitState{};
                bool Initialized{};
                bool Dirty{};
            } Bgfx{};

            struct
            {
                size_t Width{};
                size_t Height{};
                float HardwareScalingLevel{1.0f};
            } Resolution{};
        } m_state;

        SafeTimespanGuarantor m_safeTimespanGuarantor{};

        RenderScheduler m_beforeRenderScheduler;
        RenderScheduler m_afterRenderScheduler;

        BgfxCallback m_bgfxCallback{};

        std::unique_ptr<FrameBufferManager> m_frameBufferManager{};

        std::map<std::thread::id, bgfx::Encoder*> m_threadIdToEncoder{};
        std::mutex m_threadIdToEncoderMutex{};
    };
}
