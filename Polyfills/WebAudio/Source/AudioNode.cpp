#include "AudioNode.h"

#include <algorithm>
#include <cmath>

namespace
{
    constexpr auto JS_NODE_STATE_PROPERTY = "_audioNodeState";
}

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        constexpr float PI = 3.14159265358979323846f;

        float Clamp(float value, float minValue, float maxValue)
        {
            return std::max(minValue, std::min(value, maxValue));
        }

        double ClampDouble(double value, double minValue, double maxValue)
        {
            return std::max(minValue, std::min(value, maxValue));
        }

        float Length3(float x, float y, float z)
        {
            return std::sqrt(x * x + y * y + z * z);
        }

        void Normalize3(float& x, float& y, float& z)
        {
            const float length = Length3(x, y, z);
            if (length > 1.0e-6f)
            {
                x /= length;
                y /= length;
                z /= length;
            }
        }

        float Dot3(float ax, float ay, float az, float bx, float by, float bz)
        {
            return ax * bx + ay * by + az * bz;
        }
    }

    void AudioNodeState::Connect(const std::shared_ptr<AudioNodeState>& destination)
    {
        if (!destination)
        {
            return;
        }

        if (std::find(outputs.begin(), outputs.end(), destination) == outputs.end())
        {
            outputs.push_back(destination);
        }
    }

    void AudioNodeState::Disconnect()
    {
        outputs.clear();
    }

    void AudioNodeState::Disconnect(const std::shared_ptr<AudioNodeState>& destination)
    {
        if (!destination)
        {
            Disconnect();
            return;
        }

        outputs.erase(std::remove(outputs.begin(), outputs.end(), destination), outputs.end());
    }

    void AudioNodeState::ProcessSample(float&, float&, const AudioListenerState&, double)
    {
    }

    void AudioNodeState::RenderSample(float& left, float& right, float* mixL, float* mixR, size_t frameIndex, const AudioListenerState& listener, double sampleTime)
    {
        if (kind == AudioNodeKind::Destination)
        {
            mixL[frameIndex] += left;
            mixR[frameIndex] += right;
            return;
        }

        ProcessSample(left, right, listener, sampleTime);

        for (const auto& output : outputs)
        {
            if (!output)
            {
                continue;
            }

            float outLeft = left;
            float outRight = right;
            output->RenderSample(outLeft, outRight, mixL, mixR, frameIndex, listener, sampleTime);
        }
    }

    void GainNodeState::ProcessSample(float& left, float& right, const AudioListenerState&, double sampleTime)
    {
        const float g = gain.GetAtTime(sampleTime);
        left *= g;
        right *= g;
    }

    void StereoPannerNodeState::ProcessSample(float& left, float& right, const AudioListenerState&, double sampleTime)
    {
        const float panValue = Clamp(pan.GetAtTime(sampleTime), -1.f, 1.f);
        const float angle = (panValue + 1.f) * 0.25f * PI;
        const float gainL = std::cos(angle);
        const float gainR = std::sin(angle);
        const float mono = 0.5f * (left + right);
        left = mono * gainL;
        right = mono * gainR;
    }

    void PannerNodeState::ProcessSample(float& left, float& right, const AudioListenerState& listener, double sampleTime)
    {
        const float sourceX = positionX.GetAtTime(sampleTime);
        const float sourceY = positionY.GetAtTime(sampleTime);
        const float sourceZ = positionZ.GetAtTime(sampleTime);

        float listenerX = listener.positionX.GetAtTime(sampleTime);
        float listenerY = listener.positionY.GetAtTime(sampleTime);
        float listenerZ = listener.positionZ.GetAtTime(sampleTime);

        float dx = sourceX - listenerX;
        float dy = sourceY - listenerY;
        float dz = sourceZ - listenerZ;
        const float distance = Length3(dx, dy, dz);

        float distanceGain = 1.f;
        if (distanceModel == "linear")
        {
            distanceGain = 1.f - rolloffFactor * (distance - refDistance) / std::max(maxDistance - refDistance, 1.0e-6f);
            distanceGain = Clamp(distanceGain, 0.f, 1.f);
        }
        else if (distanceModel == "exponential")
        {
            const float ratio = std::max(distance, refDistance) / std::max(refDistance, 1.0e-6f);
            distanceGain = std::pow(ratio, -rolloffFactor);
        }
        else
        {
            // inverse (default)
            distanceGain = refDistance / (refDistance + rolloffFactor * std::max(distance - refDistance, 0.f));
        }

        if (distance > maxDistance)
        {
            distanceGain = 0.f;
        }

        float coneGain = 1.f;
        if (coneInnerAngle < 360.f || coneOuterAngle < 360.f)
        {
            float orientX = orientationX.GetAtTime(sampleTime);
            float orientY = orientationY.GetAtTime(sampleTime);
            float orientZ = orientationZ.GetAtTime(sampleTime);
            Normalize3(orientX, orientY, orientZ);

            float toListenerX = listenerX - sourceX;
            float toListenerY = listenerY - sourceY;
            float toListenerZ = listenerZ - sourceZ;
            Normalize3(toListenerX, toListenerY, toListenerZ);

            const float angle = std::acos(Clamp(Dot3(orientX, orientY, orientZ, toListenerX, toListenerY, toListenerZ), -1.f, 1.f)) * (180.f / PI);
            const float absAngle = std::abs(angle);
            const float halfInner = coneInnerAngle * 0.5f;
            const float halfOuter = coneOuterAngle * 0.5f;
            if (absAngle > halfOuter)
            {
                coneGain = coneOuterGain;
            }
            else if (absAngle > halfInner)
            {
                const float t = (absAngle - halfInner) / std::max(halfOuter - halfInner, 1.0e-6f);
                coneGain = (1.f - t) + t * coneOuterGain;
            }
        }

        const float gain = distanceGain * coneGain;
        left *= gain;
        right *= gain;

        // Equal-power azimuth panning relative to the listener's forward/right axes.
        float forwardX = listener.forwardX.GetAtTime(sampleTime);
        float forwardY = listener.forwardY.GetAtTime(sampleTime);
        float forwardZ = listener.forwardZ.GetAtTime(sampleTime);
        float upX = listener.upX.GetAtTime(sampleTime);
        float upY = listener.upY.GetAtTime(sampleTime);
        float upZ = listener.upZ.GetAtTime(sampleTime);
        Normalize3(forwardX, forwardY, forwardZ);
        Normalize3(upX, upY, upZ);

        float rightX = forwardY * upZ - forwardZ * upY;
        float rightY = forwardZ * upX - forwardX * upZ;
        float rightZ = forwardX * upY - forwardY * upX;
        Normalize3(rightX, rightY, rightZ);

        if (distance > 1.0e-6f)
        {
            dx /= distance;
            dy /= distance;
            dz /= distance;
        }

        const float azimuth = Clamp(Dot3(dx, dy, dz, rightX, rightY, rightZ), -1.f, 1.f);
        const float panAngle = (azimuth + 1.f) * 0.25f * PI;
        const float gainL = std::cos(panAngle);
        const float gainR = std::sin(panAngle);
        const float mono = 0.5f * (left + right);
        left = mono * gainL;
        right = mono * gainR;
    }

    void BufferSourceState::Start(double when, double offset, double duration)
    {
        if (!buffer || buffer->length == 0 || buffer->sampleRate == 0)
        {
            ended.store(true);
            started.store(false);
            playbackActive.store(false);
            return;
        }

        const double bufferDuration = buffer->Duration();
        const double startOffset = std::max(0.0, std::min(offset, bufferDuration));
        playbackFrame = startOffset * static_cast<double>(buffer->sampleRate);
        scheduledStartTime = when;

        if (duration >= 0.0)
        {
            const double endFrame = playbackFrame + duration * static_cast<double>(buffer->sampleRate);
            loopEnd = endFrame / static_cast<double>(buffer->sampleRate);
            loop = false;
        }

        ended.store(false);
        playbackActive.store(false);
        started.store(true);
    }

    void BufferSourceState::Stop()
    {
        started.store(false);
        playbackActive.store(false);
        if (!ended.exchange(true) && onEnded)
        {
            onEnded();
        }
    }

    bool BufferSourceState::Mix(float* mixL, float* mixR, uint32_t frameCount, uint32_t outputSampleRate, const AudioListenerState& listener, double blockStartTime)
    {
        if (!started.load() || !buffer || buffer->length == 0)
        {
            return false;
        }

        const double outputRate = static_cast<double>(outputSampleRate);
        const double sourceRate = static_cast<double>(buffer->sampleRate);

        const double bufferLength = static_cast<double>(buffer->length);
        double loopStartFrame = loopStart * sourceRate;
        double loopEndFrame = loopEnd > 0.0 ? loopEnd * sourceRate : bufferLength;
        loopStartFrame = ClampDouble(loopStartFrame, 0.0, bufferLength);
        loopEndFrame = ClampDouble(loopEndFrame, loopStartFrame, bufferLength);

        const auto& channels = buffer->channels;
        const uint32_t channelCount = buffer->numberOfChannels;

        bool finished = false;
        for (uint32_t frame = 0; frame < frameCount; ++frame)
        {
            const double sampleTime = blockStartTime + static_cast<double>(frame) / outputRate;
            if (sampleTime < scheduledStartTime)
            {
                continue;
            }

            if (!playbackActive.load(std::memory_order_relaxed))
            {
                playbackActive.store(true, std::memory_order_relaxed);
            }

            const double rate = static_cast<double>(playbackRate.GetAtTime(sampleTime)) * std::pow(2.0, static_cast<double>(detune.GetAtTime(sampleTime)) / 1200.0);
            const double frameStep = rate * (sourceRate / outputRate);

            if (playbackFrame >= bufferLength)
            {
                if (loop)
                {
                    playbackFrame = loopStartFrame;
                }
                else
                {
                    finished = true;
                    break;
                }
            }

            if (loop && playbackFrame >= loopEndFrame)
            {
                playbackFrame = loopStartFrame;
            }

            const size_t index = static_cast<size_t>(playbackFrame);
            const size_t nextIndex = std::min(index + 1, static_cast<size_t>(buffer->length - 1));
            const float frac = static_cast<float>(playbackFrame - static_cast<double>(index));

            auto sampleChannel = [&](uint32_t channel) {
                if (channel >= channelCount || channels[channel].empty())
                {
                    return 0.f;
                }
                const float a = channels[channel][index];
                const float b = channels[channel][nextIndex];
                return a + (b - a) * frac;
            };

            float left = sampleChannel(0);
            float right = channelCount > 1 ? sampleChannel(1) : left;

            RenderSample(left, right, mixL, mixR, frame, listener, sampleTime);
            playbackFrame += frameStep;
        }

        if (finished)
        {
            started.store(false);
            playbackActive.store(false);
            if (!ended.exchange(true) && onEnded)
            {
                onEnded();
            }
            return true;
        }

        return false;
    }

    namespace
    {
        bool IsPowerOfTwo(uint32_t value)
        {
            return value >= 2 && (value & (value - 1)) == 0;
        }

        void FftInPlace(std::vector<float>& real, std::vector<float>& imag)
        {
            const size_t n = real.size();
            if (n < 2)
            {
                return;
            }

            for (size_t i = 1, j = 0; i < n; ++i)
            {
                size_t bit = n >> 1;
                for (; j & bit; bit >>= 1)
                {
                    j ^= bit;
                }
                j ^= bit;
                if (i < j)
                {
                    std::swap(real[i], real[j]);
                    std::swap(imag[i], imag[j]);
                }
            }

            for (size_t length = 2; length <= n; length <<= 1)
            {
                const double angle = -2.0 * PI / static_cast<double>(length);
                const float wReal = static_cast<float>(std::cos(angle));
                const float wImag = static_cast<float>(std::sin(angle));

                for (size_t i = 0; i < n; i += length)
                {
                    float currentWReal = 1.f;
                    float currentWImag = 0.f;
                    const size_t half = length / 2;
                    for (size_t j = 0; j < half; ++j)
                    {
                        const size_t evenIndex = i + j;
                        const size_t oddIndex = i + j + half;
                        const float oddReal = real[oddIndex] * currentWReal - imag[oddIndex] * currentWImag;
                        const float oddImag = real[oddIndex] * currentWImag + imag[oddIndex] * currentWReal;

                        real[oddIndex] = real[evenIndex] - oddReal;
                        imag[oddIndex] = imag[evenIndex] - oddImag;
                        real[evenIndex] += oddReal;
                        imag[evenIndex] += oddImag;

                        const float nextWReal = currentWReal * wReal - currentWImag * wImag;
                        currentWImag = currentWReal * wImag + currentWImag * wReal;
                        currentWReal = nextWReal;
                    }
                }
            }
        }
    }

    void AnalyserNodeState::ResetRingBuffer()
    {
        ringBuffer.assign(fftSize, 0.f);
        ringWriteIndex = 0;
        ringFilled = false;
        smoothedMagnitudes.assign(FrequencyBinCount(), minDecibels);
    }

    void AnalyserNodeState::SetFftSize(uint32_t size)
    {
        if (!IsPowerOfTwo(size))
        {
            return;
        }

        fftSize = size;
        ResetRingBuffer();
    }

    void AnalyserNodeState::WriteRingSample(float sample)
    {
        if (ringBuffer.empty())
        {
            return;
        }

        ringBuffer[ringWriteIndex] = sample;
        ringWriteIndex = (ringWriteIndex + 1) % ringBuffer.size();
        if (ringWriteIndex == 0)
        {
            ringFilled = true;
        }
    }

    void AnalyserNodeState::ProcessSample(float& left, float& right, const AudioListenerState&, double)
    {
        WriteRingSample(0.5f * (left + right));
    }

    void AnalyserNodeState::CopyTimeDomainSnapshot(std::vector<float>& snapshot) const
    {
        snapshot.resize(fftSize);
        if (ringBuffer.empty())
        {
            std::fill(snapshot.begin(), snapshot.end(), 0.f);
            return;
        }

        if (!ringFilled)
        {
            const size_t available = ringWriteIndex;
            const size_t silenceCount = fftSize - available;
            std::fill(snapshot.begin(), snapshot.begin() + silenceCount, 0.f);
            std::copy(ringBuffer.begin(), ringBuffer.begin() + static_cast<std::ptrdiff_t>(available), snapshot.begin() + static_cast<std::ptrdiff_t>(silenceCount));
            return;
        }

        const size_t start = ringWriteIndex;
        for (size_t index = 0; index < fftSize; ++index)
        {
            snapshot[index] = ringBuffer[(start + index) % fftSize];
        }
    }

    void AnalyserNodeState::ComputeFrequencyData(std::vector<float>& magnitudesDb) const
    {
        const uint32_t binCount = FrequencyBinCount();
        magnitudesDb.resize(binCount);

        std::vector<float> snapshot;
        CopyTimeDomainSnapshot(snapshot);

        std::vector<float> real(fftSize, 0.f);
        std::vector<float> imag(fftSize, 0.f);
        for (uint32_t index = 0; index < fftSize; ++index)
        {
            const float window = 0.5f * (1.f - std::cos((2.f * PI * static_cast<float>(index)) / static_cast<float>(fftSize)));
            real[index] = snapshot[index] * window;
        }

        FftInPlace(real, imag);

        const float normalization = 1.f / static_cast<float>(fftSize);
        for (uint32_t bin = 0; bin < binCount; ++bin)
        {
            const float magnitude = std::sqrt(real[bin] * real[bin] + imag[bin] * imag[bin]) * normalization;
            const float db = magnitude > 1.0e-12f ? 20.f * std::log10(magnitude) : minDecibels;
            magnitudesDb[bin] = Clamp(db, minDecibels, maxDecibels);
        }

        if (smoothedMagnitudes.size() != binCount)
        {
            smoothedMagnitudes.assign(binCount, minDecibels);
        }

        const float smoothing = Clamp(smoothingTimeConstant, 0.f, 1.f);
        for (uint32_t bin = 0; bin < binCount; ++bin)
        {
            smoothedMagnitudes[bin] = smoothing * smoothedMagnitudes[bin] + (1.f - smoothing) * magnitudesDb[bin];
            magnitudesDb[bin] = smoothedMagnitudes[bin];
        }
    }

    void BindNodeState(Napi::Object object, const std::shared_ptr<AudioNodeState>& state)
    {
        auto* holder = new std::shared_ptr<AudioNodeState>(state);
        object.Set(JS_NODE_STATE_PROPERTY, Napi::External<std::shared_ptr<AudioNodeState>>::New(object.Env(), holder, [](Napi::Env, std::shared_ptr<AudioNodeState>* ptr) {
            delete ptr;
        }));
    }

    void SetAudioNodeProperties(Napi::Object object, uint32_t numberOfInputs, uint32_t numberOfOutputs)
    {
        object.Set("numberOfInputs", numberOfInputs);
        object.Set("numberOfOutputs", numberOfOutputs);
    }

    std::shared_ptr<AudioNodeState> UnwrapNodeState(const Napi::Value& value)
    {
        if (!value.IsObject())
        {
            return nullptr;
        }

        auto external = value.As<Napi::Object>().Get(JS_NODE_STATE_PROPERTY);
        if (external.IsUndefined() || !external.IsExternal())
        {
            return nullptr;
        }

        auto* holder = external.As<Napi::External<std::shared_ptr<AudioNodeState>>>().Data();
        return holder ? *holder : nullptr;
    }

    void EventTargetSupport::AddEventListener(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 2 || !info[0].IsString() || !info[1].IsFunction())
        {
            return;
        }

        const auto type = info[0].As<Napi::String>().Utf8Value();
        m_listeners[type].emplace_back(Napi::Persistent(info[1].As<Napi::Function>()));
    }

    void EventTargetSupport::RemoveEventListener(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 2 || !info[0].IsString() || !info[1].IsFunction())
        {
            return;
        }

        const auto type = info[0].As<Napi::String>().Utf8Value();
        auto it = m_listeners.find(type);
        if (it == m_listeners.end())
        {
            return;
        }

        auto& listeners = it->second;
        const auto function = info[1].As<Napi::Function>();
        for (auto listenerIt = listeners.begin(); listenerIt != listeners.end(); ++listenerIt)
        {
            if (listenerIt->Value() == function)
            {
                listeners.erase(listenerIt);
                break;
            }
        }
    }

    void EventTargetSupport::DispatchEvent(Napi::Env env, const std::string& type)
    {
        auto it = m_listeners.find(type);
        if (it == m_listeners.end())
        {
            return;
        }

        auto event = Napi::Object::New(env);
        event.Set("type", Napi::String::New(env, type));

        // Iterate by index in case a listener mutates the list.
        for (size_t index = 0; index < it->second.size(); ++index)
        {
            it->second[index].Call({event});
        }
    }
}
