#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <napi/env.h>

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <string>
#include <vector>

struct ma_device;

namespace Babylon::Polyfills::Internal
{
    struct AudioNodeState;
    struct AudioContextState;
    struct BufferSourceState;

    class AudioEngine final : public std::enable_shared_from_this<AudioEngine>
    {
    public:
        static std::shared_ptr<AudioEngine> Create(JsRuntime& runtime);
        ~AudioEngine();

        AudioEngine(const AudioEngine&) = delete;
        AudioEngine& operator=(const AudioEngine&) = delete;

        uint32_t SampleRate() const;
        double CurrentTime() const;
        bool IsRunning() const;
        bool UsesNullBackend() const;
        bool DeviceInitialized() const;

        bool EnsureDeviceRunning();
        void SuspendDevice();
        void ResumeDevice();
        void TryResumeAfterInterruption();
        void SetClockSuspended(bool suspended);
        void PrepareForShutdown();
        void OnContextRunningChanged();
        bool HasRunningContext();

        void RegisterContext(const std::shared_ptr<AudioContextState>& context);
        void UnregisterContext(AudioContextState* context);

        void RegisterSource(const std::shared_ptr<BufferSourceState>& source);
        void UnregisterSource(BufferSourceState* source);

        void ScheduleOnJsThread(std::function<void()> callback);

        std::mutex& GraphMutex();

    private:
        explicit AudioEngine(JsRuntime& runtime);

        bool InitializeDevice();
        void ShutdownDevice();
        void StartNullClock();
        void StopNullClock();

        static void DataCallback(ma_device* device, void* output, const void* input, uint32_t frameCount);
        void Mix(float* outputInterleaved, uint32_t frameCount);

        JsRuntimeScheduler m_runtimeScheduler;
        std::unique_ptr<ma_device> m_device{};
        mutable std::mutex m_deviceMutex{};
        uint32_t m_sampleRate{48000};
        std::atomic<uint64_t> m_framesProcessed{0};
        std::atomic<bool> m_deviceStarted{false};
        std::atomic<bool> m_deviceInitialized{false};
        std::atomic<bool> m_usingNullBackend{false};
        std::atomic<bool> m_clockSuspended{true};
        std::atomic<bool> m_shuttingDown{false};
        std::atomic<bool> m_nullClockRunning{false};
        std::thread m_nullClockThread{};

        std::mutex m_graphMutex{};
        std::vector<std::weak_ptr<AudioContextState>> m_contexts{};
        std::vector<std::weak_ptr<BufferSourceState>> m_sources{};
    };

    std::function<double()> MakeCurrentTimeGetter(const std::shared_ptr<AudioContextState>& context);

    std::shared_ptr<AudioEngine> GetAudioEngineFromJavaScript(Napi::Env env);
}
