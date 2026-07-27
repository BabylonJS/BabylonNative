#include "AudioEngine.h"
#include "AudioNode.h"

#include <Babylon/Polyfills/AudioPlatform.h>

#include <miniaudio.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        constexpr uint32_t OUTPUT_CHANNEL_COUNT = 2;
        constexpr uint32_t NULL_BACKEND_FRAME_COUNT = 256;
        constexpr const char* NULL_BACKEND_ENV = "BABYLON_NATIVE_WEBAUDIO_BACKEND";

        bool UseNullBackend()
        {
            const char* value = std::getenv(NULL_BACKEND_ENV);
            return value != nullptr && std::strcmp(value, "null") == 0;
        }
    }

    std::shared_ptr<AudioEngine> AudioEngine::Create(JsRuntime& runtime)
    {
        auto engine = std::shared_ptr<AudioEngine>(new AudioEngine(runtime));
        Babylon::Polyfills::Internal::RegisterAudioEngine(engine);
        return engine;
    }

    AudioEngine::AudioEngine(JsRuntime& runtime)
        : m_runtimeScheduler{runtime}
    {
    }

    AudioEngine::~AudioEngine()
    {
        PrepareForShutdown();
    }

    void AudioEngine::PrepareForShutdown()
    {
        if (m_shuttingDown.exchange(true))
        {
            return;
        }

        UnregisterAudioEngine(this);
        SetClockSuspended(true);
        SuspendDevice();
        {
            std::lock_guard<std::mutex> lock{m_graphMutex};
            m_sources.clear();
        }
        ShutdownDevice();
    }

    bool AudioEngine::InitializeDevice()
    {
        if (m_shuttingDown.load(std::memory_order_relaxed))
        {
            return false;
        }

        std::lock_guard<std::mutex> lock{m_deviceMutex};
        if (m_deviceInitialized.load(std::memory_order_relaxed))
        {
            return true;
        }

        const bool forceNull = UseNullBackend();
        if (forceNull)
        {
            m_usingNullBackend.store(true, std::memory_order_relaxed);
            m_sampleRate = 48000;
            m_deviceInitialized.store(true, std::memory_order_relaxed);
            return true;
        }

        m_device = std::make_unique<ma_device>();

        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.format = ma_format_f32;
        config.playback.channels = OUTPUT_CHANNEL_COUNT;
        config.dataCallback = DataCallback;
        config.pUserData = this;

        ma_result result = MA_ERROR;
        {
            Babylon::Polyfills::Internal::PreparePlayback();
            result = ma_device_init(nullptr, &config, m_device.get());
            if (result != MA_SUCCESS)
            {
                ma_backend backends[] = {ma_backend_null};
                result = ma_device_init_ex(backends, 1, nullptr, &config, m_device.get());
                m_usingNullBackend.store(true, std::memory_order_relaxed);
            }
            else
            {
                m_usingNullBackend.store(false, std::memory_order_relaxed);
            }
        }

        if (result != MA_SUCCESS)
        {
            m_device.reset();
            m_deviceInitialized.store(false, std::memory_order_relaxed);
            m_deviceStarted.store(false, std::memory_order_relaxed);
            return false;
        }

        m_sampleRate = m_device->sampleRate;
        m_deviceInitialized.store(true, std::memory_order_relaxed);
        return true;
    }

    void AudioEngine::ShutdownDevice()
    {
        StopNullClock();

        std::lock_guard<std::mutex> lock{m_deviceMutex};
        if (!m_device)
        {
            m_deviceInitialized.store(false, std::memory_order_relaxed);
            m_deviceStarted.store(false, std::memory_order_relaxed);
            return;
        }

        if (m_deviceStarted.load(std::memory_order_relaxed))
        {
            ma_device_stop(m_device.get());
            m_deviceStarted.store(false, std::memory_order_relaxed);
        }

        ma_device_uninit(m_device.get());
        m_device.reset();
        m_deviceInitialized.store(false, std::memory_order_relaxed);
        m_deviceStarted.store(false, std::memory_order_relaxed);
    }

    bool AudioEngine::EnsureDeviceRunning()
    {
        if (m_shuttingDown.load(std::memory_order_relaxed))
        {
            return false;
        }

        if (!InitializeDevice())
        {
            return false;
        }

        std::lock_guard<std::mutex> lock{m_deviceMutex};
        if (!m_deviceInitialized.load(std::memory_order_relaxed))
        {
            return false;
        }

        if (!m_usingNullBackend.load(std::memory_order_relaxed) && !m_device)
        {
            return false;
        }

        if (!m_deviceStarted.load(std::memory_order_relaxed))
        {
            if (m_usingNullBackend.load(std::memory_order_relaxed))
            {
                StartNullClock();
            }
            else if (ma_device_start(m_device.get()) != MA_SUCCESS)
            {
                return false;
            }

            m_deviceStarted.store(true, std::memory_order_relaxed);
        }

        return true;
    }

    void AudioEngine::SuspendDevice()
    {
        if (m_usingNullBackend.load(std::memory_order_relaxed))
        {
            StopNullClock();
            m_deviceStarted.store(false, std::memory_order_relaxed);
            return;
        }

        std::lock_guard<std::mutex> lock{m_deviceMutex};
        if (!m_device || !m_deviceStarted.load(std::memory_order_relaxed))
        {
            return;
        }

        ma_device_stop(m_device.get());
        m_deviceStarted.store(false, std::memory_order_relaxed);
    }

    void AudioEngine::ResumeDevice()
    {
        if (m_usingNullBackend.load(std::memory_order_relaxed))
        {
            if (!m_deviceStarted.load(std::memory_order_relaxed))
            {
                StartNullClock();
                m_deviceStarted.store(true, std::memory_order_relaxed);
            }
            return;
        }

        std::lock_guard<std::mutex> lock{m_deviceMutex};
        if (!m_device || m_deviceStarted.load(std::memory_order_relaxed))
        {
            return;
        }

        if (ma_device_start(m_device.get()) == MA_SUCCESS)
        {
            m_deviceStarted.store(true, std::memory_order_relaxed);
        }
    }

    void AudioEngine::TryResumeAfterInterruption()
    {
        if (HasRunningContext())
        {
            EnsureDeviceRunning();
            ResumeDevice();
            SetClockSuspended(false);
        }
    }

    void AudioEngine::SetClockSuspended(bool suspended)
    {
        m_clockSuspended.store(suspended, std::memory_order_relaxed);
    }

    bool AudioEngine::HasRunningContext()
    {
        std::lock_guard<std::mutex> lock{m_graphMutex};
        for (const auto& weakContext : m_contexts)
        {
            auto context = weakContext.lock();
            if (context && context->running.load(std::memory_order_relaxed))
            {
                return true;
            }
        }

        return false;
    }

    void AudioEngine::OnContextRunningChanged()
    {
        const bool anyRunning = HasRunningContext();
        SetClockSuspended(!anyRunning);
        if (anyRunning)
        {
            EnsureDeviceRunning();
            ResumeDevice();
        }
        else
        {
            SuspendDevice();
        }
    }

    uint32_t AudioEngine::SampleRate() const
    {
        return m_sampleRate;
    }

    double AudioEngine::CurrentTime() const
    {
        return static_cast<double>(m_framesProcessed.load(std::memory_order_relaxed)) / static_cast<double>(m_sampleRate);
    }

    bool AudioEngine::IsRunning() const
    {
        return m_deviceStarted.load(std::memory_order_relaxed);
    }

    bool AudioEngine::UsesNullBackend() const
    {
        return m_usingNullBackend.load(std::memory_order_relaxed);
    }

    bool AudioEngine::DeviceInitialized() const
    {
        return m_deviceInitialized.load(std::memory_order_relaxed);
    }

    void AudioEngine::RegisterContext(const std::shared_ptr<AudioContextState>& context)
    {
        {
            std::lock_guard<std::mutex> lock{m_graphMutex};
            m_contexts.push_back(context);
        }
        OnContextRunningChanged();
    }

    void AudioEngine::UnregisterContext(AudioContextState* context)
    {
        {
            std::lock_guard<std::mutex> lock{m_graphMutex};
            m_contexts.erase(
                std::remove_if(m_contexts.begin(), m_contexts.end(), [context](const std::weak_ptr<AudioContextState>& weak) {
                    auto shared = weak.lock();
                    return !shared || shared.get() == context;
                }),
                m_contexts.end());
        }
        OnContextRunningChanged();
    }

    void AudioEngine::RegisterSource(const std::shared_ptr<BufferSourceState>& source)
    {
        std::lock_guard<std::mutex> lock{m_graphMutex};
        m_sources.push_back(source);
    }

    void AudioEngine::UnregisterSource(BufferSourceState* source)
    {
        std::lock_guard<std::mutex> lock{m_graphMutex};
        m_sources.erase(
            std::remove_if(m_sources.begin(), m_sources.end(), [source](const std::weak_ptr<BufferSourceState>& weak) {
                auto shared = weak.lock();
                return !shared || shared.get() == source;
            }),
            m_sources.end());
    }

    void AudioEngine::ScheduleOnJsThread(std::function<void()> callback)
    {
        m_runtimeScheduler([callback = std::move(callback)]() {
            callback();
        });
    }

    std::mutex& AudioEngine::GraphMutex()
    {
        return m_graphMutex;
    }

    std::function<double()> MakeCurrentTimeGetter(const std::shared_ptr<AudioContextState>& context)
    {
        return [weakContext = std::weak_ptr<AudioContextState>(context)]() {
            auto sharedContext = weakContext.lock();
            if (!sharedContext || !sharedContext->engine)
            {
                return 0.0;
            }

            return sharedContext->engine->CurrentTime();
        };
    }

    void AudioEngine::DataCallback(ma_device* device, void* output, const void* /*input*/, uint32_t frameCount)
    {
        auto* engine = static_cast<AudioEngine*>(device->pUserData);
        engine->Mix(static_cast<float*>(output), frameCount);
    }

    void AudioEngine::StartNullClock()
    {
        if (m_nullClockRunning.load(std::memory_order_relaxed) || m_shuttingDown.load(std::memory_order_relaxed))
        {
            return;
        }

        m_nullClockRunning.store(true, std::memory_order_relaxed);
        m_nullClockThread = std::thread([this]() {
            std::vector<float> buffer(static_cast<size_t>(NULL_BACKEND_FRAME_COUNT) * OUTPUT_CHANNEL_COUNT, 0.f);
            while (m_nullClockRunning.load(std::memory_order_relaxed) && !m_shuttingDown.load(std::memory_order_relaxed))
            {
                if (!m_clockSuspended.load(std::memory_order_relaxed))
                {
                    Mix(buffer.data(), NULL_BACKEND_FRAME_COUNT);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        });
    }

    void AudioEngine::StopNullClock()
    {
        if (!m_nullClockRunning.exchange(false, std::memory_order_relaxed))
        {
            return;
        }

        if (m_nullClockThread.joinable())
        {
            m_nullClockThread.detach();
        }
    }

    void AudioEngine::Mix(float* outputInterleaved, uint32_t frameCount)
    {
        std::memset(outputInterleaved, 0, sizeof(float) * frameCount * OUTPUT_CHANNEL_COUNT);

        if (m_shuttingDown.load(std::memory_order_relaxed) || m_clockSuspended.load(std::memory_order_relaxed))
        {
            return;
        }

        std::vector<float> mixL(frameCount, 0.f);
        std::vector<float> mixR(frameCount, 0.f);

        {
            std::lock_guard<std::mutex> lock{m_graphMutex};

            const double blockStartTime = static_cast<double>(m_framesProcessed.load(std::memory_order_relaxed)) / static_cast<double>(m_sampleRate);

            m_sources.erase(
                std::remove_if(m_sources.begin(), m_sources.end(), [](const std::weak_ptr<BufferSourceState>& weak) {
                    return weak.expired();
                }),
                m_sources.end());

            for (const auto& weakSource : m_sources)
            {
                auto source = weakSource.lock();
                if (!source || !source->started.load())
                {
                    continue;
                }

                auto context = source->context.lock();
                if (!context || !context->running.load())
                {
                    continue;
                }

                std::fill(mixL.begin(), mixL.end(), 0.f);
                std::fill(mixR.begin(), mixR.end(), 0.f);
                source->Mix(mixL.data(), mixR.data(), frameCount, m_sampleRate, context->listener, blockStartTime);

                for (uint32_t frame = 0; frame < frameCount; ++frame)
                {
                    outputInterleaved[frame * OUTPUT_CHANNEL_COUNT] += mixL[frame];
                    outputInterleaved[frame * OUTPUT_CHANNEL_COUNT + 1] += mixR[frame];
                }
            }
        }

        m_framesProcessed.fetch_add(frameCount, std::memory_order_relaxed);
    }
}
