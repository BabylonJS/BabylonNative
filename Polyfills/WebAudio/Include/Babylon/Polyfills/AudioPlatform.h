#pragma once

#include <Babylon/Api.h>

#include <functional>
#include <memory>

namespace Babylon::Polyfills
{
    using AudioPreparePlaybackHandler = std::function<void()>;
    using AudioInterruptionHandler = std::function<void(bool began)>;

    void BABYLON_API RegisterAudioPreparePlaybackHandler(AudioPreparePlaybackHandler handler);
    void BABYLON_API RegisterAudioInterruptionHandler(AudioInterruptionHandler handler);

    namespace Internal
    {
        class AudioEngine;

        void PreparePlayback();
        void ReleasePlayback();
        void NotifyAudioInterruption(bool began);

        void RegisterAudioEngine(const std::shared_ptr<AudioEngine>& engine);
        void UnregisterAudioEngine(AudioEngine* engine);
    }
}
