#pragma once

#include <napi/napi.h>

#include <atomic>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    class AudioEngine;
    struct AudioContextState;
    struct AudioListenerState;

    enum class AudioNodeKind
    {
        Destination,
        Gain,
        Panner,
        StereoPanner,
        BufferSource,
        Analyser,
    };

    struct AudioParamState
    {
        enum class EventType
        {
            SetValue,
            LinearRamp,
            ExponentialRamp,
            SetCurve,
        };

        struct Event
        {
            double time;
            EventType type;
            float value;
            std::vector<float> curve;
            double duration;
        };

        float defaultValue{0.f};
        std::vector<Event> events{};

        void SetDefault(float value);
        void CancelScheduledValues(double startTime);
        void SetValueAtTime(float value, double time);
        void LinearRampToValueAtTime(float value, double time);
        void ExponentialRampToValueAtTime(float value, double time);
        void SetValueCurveAtTime(const std::vector<float>& values, double startTime, double duration);
        void SetImmediate(float value, double currentTime);
        float GetAtTime(double t) const;
    };

    struct AudioBufferData
    {
        uint32_t sampleRate{48000};
        uint32_t length{0};
        uint32_t numberOfChannels{1};
        std::vector<std::vector<float>> channels{};

        double Duration() const
        {
            return sampleRate == 0 ? 0.0 : static_cast<double>(length) / static_cast<double>(sampleRate);
        }
    };

    struct AudioNodeState : std::enable_shared_from_this<AudioNodeState>
    {
        explicit AudioNodeState(AudioNodeKind kind)
            : kind{kind}
        {
        }

        virtual ~AudioNodeState() = default;

        const AudioNodeKind kind;
        std::weak_ptr<AudioContextState> context{};
        std::vector<std::shared_ptr<AudioNodeState>> outputs{};

        void Connect(const std::shared_ptr<AudioNodeState>& destination);
        void Disconnect();
        void Disconnect(const std::shared_ptr<AudioNodeState>& destination);

        // Applies this node's effect to a stereo sample in-place, then fans out to outputs.
        // Destination nodes accumulate into mixL/mixR.
        void RenderSample(float& left, float& right, float* mixL, float* mixR, size_t frameIndex, const AudioListenerState& listener, double sampleTime);

    protected:
        virtual void ProcessSample(float& left, float& right, const AudioListenerState& listener, double sampleTime);
    };

    struct AudioListenerState
    {
        AudioParamState positionX{};
        AudioParamState positionY{};
        AudioParamState positionZ{};
        AudioParamState forwardX{};
        AudioParamState forwardY{};
        AudioParamState forwardZ{};
        AudioParamState upX{};
        AudioParamState upY{};
        AudioParamState upZ{};

        AudioListenerState()
        {
            positionX.SetDefault(0.f);
            positionY.SetDefault(0.f);
            positionZ.SetDefault(0.f);
            forwardX.SetDefault(0.f);
            forwardY.SetDefault(0.f);
            forwardZ.SetDefault(-1.f);
            upX.SetDefault(0.f);
            upY.SetDefault(1.f);
            upZ.SetDefault(0.f);
        }
    };

    struct AudioContextState : std::enable_shared_from_this<AudioContextState>
    {
        std::shared_ptr<AudioEngine> engine{};
        std::shared_ptr<AudioNodeState> destination{};
        AudioListenerState listener{};
        std::atomic<bool> running{true};
        std::string state{"running"};
    };

    struct GainNodeState : AudioNodeState
    {
        GainNodeState()
            : AudioNodeState{AudioNodeKind::Gain}
        {
            gain.SetDefault(1.f);
        }

        AudioParamState gain{};

    protected:
        void ProcessSample(float& left, float& right, const AudioListenerState&, double sampleTime) override;
    };

    struct StereoPannerNodeState : AudioNodeState
    {
        StereoPannerNodeState()
            : AudioNodeState{AudioNodeKind::StereoPanner}
        {
            pan.SetDefault(0.f);
        }

        AudioParamState pan{};

    protected:
        void ProcessSample(float& left, float& right, const AudioListenerState&, double sampleTime) override;
    };

    struct PannerNodeState : AudioNodeState
    {
        PannerNodeState()
            : AudioNodeState{AudioNodeKind::Panner}
        {
            positionX.SetDefault(0.f);
            positionY.SetDefault(0.f);
            positionZ.SetDefault(0.f);
            orientationX.SetDefault(1.f);
            orientationY.SetDefault(0.f);
            orientationZ.SetDefault(0.f);
        }

        AudioParamState positionX{};
        AudioParamState positionY{};
        AudioParamState positionZ{};
        AudioParamState orientationX{};
        AudioParamState orientationY{};
        AudioParamState orientationZ{};

        std::string panningModel{"equalpower"};
        std::string distanceModel{"inverse"};
        float refDistance{1.f};
        float maxDistance{10000.f};
        float rolloffFactor{1.f};
        float coneInnerAngle{360.f};
        float coneOuterAngle{360.f};
        float coneOuterGain{0.f};

    protected:
        void ProcessSample(float& left, float& right, const AudioListenerState& listener, double sampleTime) override;
    };

    struct AnalyserNodeState : AudioNodeState
    {
        AnalyserNodeState()
            : AudioNodeState{AudioNodeKind::Analyser}
        {
            ResetRingBuffer();
        }

        uint32_t fftSize{2048};
        float minDecibels{-100.f};
        float maxDecibels{-30.f};
        float smoothingTimeConstant{0.8f};

        std::vector<float> ringBuffer{};
        size_t ringWriteIndex{0};
        bool ringFilled{false};
        mutable std::vector<float> smoothedMagnitudes{};

        uint32_t FrequencyBinCount() const
        {
            return fftSize / 2;
        }

        void SetFftSize(uint32_t size);
        void CopyTimeDomainSnapshot(std::vector<float>& snapshot) const;
        void ComputeFrequencyData(std::vector<float>& magnitudesDb) const;

    protected:
        void ProcessSample(float& left, float& right, const AudioListenerState&, double sampleTime) override;

    private:
        void ResetRingBuffer();
        void WriteRingSample(float sample);
    };

    struct DestinationNodeState : AudioNodeState
    {
        DestinationNodeState()
            : AudioNodeState{AudioNodeKind::Destination}
        {
        }
    };

    struct BufferSourceState : AudioNodeState
    {
        BufferSourceState()
            : AudioNodeState{AudioNodeKind::BufferSource}
        {
            playbackRate.SetDefault(1.f);
            detune.SetDefault(0.f);
        }

        std::shared_ptr<AudioBufferData> buffer{};
        bool loop{false};
        double loopStart{0.0};
        double loopEnd{0.0};
        AudioParamState playbackRate{};
        AudioParamState detune{};

        std::atomic<bool> started{false};
        std::atomic<bool> playbackActive{false};
        std::atomic<bool> ended{false};
        double scheduledStartTime{0.0};
        double playbackFrame{0.0};

        std::function<void()> onEnded{};

        void Start(double when, double offset, double duration);
        void Stop();
        bool Mix(float* mixL, float* mixR, uint32_t frameCount, uint32_t sampleRate, const AudioListenerState& listener, double blockStartTime);
    };

    void BindNodeState(Napi::Object object, const std::shared_ptr<AudioNodeState>& state);
    std::shared_ptr<AudioNodeState> UnwrapNodeState(const Napi::Value& value);
    void SetAudioNodeProperties(Napi::Object object, uint32_t numberOfInputs, uint32_t numberOfOutputs);

    // Minimal EventTarget used by AudioContext and AudioBufferSourceNode.
    class EventTargetSupport
    {
    public:
        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);
        void DispatchEvent(Napi::Env env, const std::string& type);

    private:
        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_listeners{};
    };
}
