#pragma once

#include <Babylon/Graphics.h>
#include "BgfxCallback.h"
#include "FrameBufferManager.h"

#include <arcana/threading/dispatcher.h>
#include <arcana/threading/task.h>
#include <arcana/threading/affinity.h>
#include "braking_shared_mutex.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/semaphore.h>

#include <shared_mutex>
#include <memory>
#include <map>

using MutexT = arcana::braking_shared_mutex;
//using MutexT = std::shared_mutex;

class Semaphore : public bx::Semaphore
{
public:
    auto GetPostFinalAction()
    {
        return gsl::finally([this]() {
            post();
        });
    }
};

class ShiftManager
{
public:
    ShiftManager()
    {
        m_mutex.lock();
    }

    void BeginShift()
    {
        m_postCount = 0;
        m_mutex.unlock();
    }

    void EndShift()
    {
        bool wait{false};
        do
        {
            m_mutex.lock();
            wait = m_postCount > 0;
            m_postCount -= 1;

            if (wait)
            {
                m_mutex.unlock();
            }
        } while (wait && m_semaphore.wait());
    }

    using PunchCard = decltype(std::declval<Semaphore>().GetPostFinalAction());
    PunchCard PunchIn()
    {
        std::scoped_lock lock{m_mutex};
        m_postCount += 1;
        return m_semaphore.GetPostFinalAction();
    }

private:
    Semaphore m_semaphore{};
    size_t m_postCount{};
    std::mutex m_mutex{};
};

namespace Babylon
{
    class Graphics::Impl
    {
    public:
        class UpdateToken final
        {
        public:
            UpdateToken(const UpdateToken&) = delete;
            UpdateToken(UpdateToken&&) = default;

            bgfx::Encoder& GetEncoder();

        private:
            friend class Graphics::Impl;

            UpdateToken(Graphics::Impl&);

            Impl& m_graphicsImpl;
            ShiftManager::PunchCard m_punchCard;
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

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        FrameBuffer& AddFrameBuffer(bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool backBuffer);
        void RemoveFrameBuffer(const FrameBuffer& frameBuffer);
        FrameBuffer& DefaultFrameBuffer();
        FrameBuffer& BoundFrameBuffer();

        BgfxCallback& Callback();

    private:
        friend class UpdateToken;

        void UpdateBgfxState();
        void UpdateBgfxResolution();
        void DiscardIfDirty();
        void Frame();
        bgfx::Encoder& GetEncoderForThread();

        arcana::affinity m_renderThreadAffinity{};
        arcana::affinity m_jsThreadAffinity{};

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

        ShiftManager m_shiftManager{};

        RenderScheduler m_beforeRenderScheduler;
        RenderScheduler m_afterRenderScheduler;

        BgfxCallback m_callback{};

        std::unique_ptr<FrameBufferManager> m_frameBufferManager{};

        std::map<std::thread::id, bgfx::Encoder*> m_threadIdToEncoder{};
        std::mutex m_threadIdToEncoderMutex{};
    };
}
