#include <Babylon/Polyfills/AudioPlatform.h>

#include "AudioEngine.h"

#include <mutex>

namespace Babylon::Polyfills
{
    namespace
    {
        std::mutex g_handlerMutex{};
        AudioPreparePlaybackHandler g_preparePlaybackHandler{};
        AudioInterruptionHandler g_interruptionHandler{};
        std::weak_ptr<Internal::AudioEngine> g_engineWeak{};

        void WithEngine(const std::function<void(const std::shared_ptr<Internal::AudioEngine>&)>& fn)
        {
            std::shared_ptr<Internal::AudioEngine> engine;
            {
                std::lock_guard<std::mutex> lock{g_handlerMutex};
                engine = g_engineWeak.lock();
            }

            if (engine)
            {
                fn(engine);
            }
        }
    }

    void RegisterAudioPreparePlaybackHandler(AudioPreparePlaybackHandler handler)
    {
        std::lock_guard<std::mutex> lock{g_handlerMutex};
        g_preparePlaybackHandler = std::move(handler);
    }

    void RegisterAudioInterruptionHandler(AudioInterruptionHandler handler)
    {
        std::lock_guard<std::mutex> lock{g_handlerMutex};
        g_interruptionHandler = std::move(handler);
    }

    namespace Internal
    {
        void PreparePlayback()
        {
            AudioPreparePlaybackHandler handler;
            {
                std::lock_guard<std::mutex> lock{g_handlerMutex};
                handler = g_preparePlaybackHandler;
            }

            if (handler)
            {
                handler();
            }
        }

        void ReleasePlayback()
        {
        }

        void NotifyAudioInterruption(bool began)
        {
            AudioInterruptionHandler handler;
            {
                std::lock_guard<std::mutex> lock{g_handlerMutex};
                handler = g_interruptionHandler;
            }

            if (handler)
            {
                handler(began);
            }

            WithEngine([began](const std::shared_ptr<AudioEngine>& engine) {
                if (began)
                {
                    engine->SuspendDevice();
                }
                else
                {
                    engine->TryResumeAfterInterruption();
                }
            });
        }

        void RegisterAudioEngine(const std::shared_ptr<AudioEngine>& engine)
        {
            std::lock_guard<std::mutex> lock{g_handlerMutex};
            g_engineWeak = engine;
        }

        void UnregisterAudioEngine(AudioEngine* engine)
        {
            std::lock_guard<std::mutex> lock{g_handlerMutex};
            auto shared = g_engineWeak.lock();
            if (shared && shared.get() == engine)
            {
                g_engineWeak.reset();
            }
        }
    }
}
