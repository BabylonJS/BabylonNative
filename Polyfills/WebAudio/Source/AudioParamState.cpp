#include "AudioNode.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        void InsertEvent(std::vector<AudioParamState::Event>& events, AudioParamState::Event event)
        {
            auto it = std::lower_bound(
                events.begin(),
                events.end(),
                event.time,
                [](const AudioParamState::Event& existing, double time) { return existing.time < time; });

            if (it != events.end() && it->time == event.time)
            {
                *it = std::move(event);
            }
            else
            {
                events.insert(it, std::move(event));
            }
        }
    }

    void AudioParamState::SetDefault(float value)
    {
        defaultValue = value;
    }

    void AudioParamState::CancelScheduledValues(double startTime)
    {
        events.erase(
            std::remove_if(events.begin(), events.end(), [startTime](const Event& event) { return event.time >= startTime; }),
            events.end());
    }

    void AudioParamState::SetValueAtTime(float value, double time)
    {
        InsertEvent(events, Event{time, EventType::SetValue, value, {}, 0.0});
    }

    void AudioParamState::LinearRampToValueAtTime(float value, double time)
    {
        InsertEvent(events, Event{time, EventType::LinearRamp, value, {}, 0.0});
    }

    void AudioParamState::ExponentialRampToValueAtTime(float value, double time)
    {
        InsertEvent(events, Event{time, EventType::ExponentialRamp, value, {}, 0.0});
    }

    void AudioParamState::SetValueCurveAtTime(const std::vector<float>& values, double startTime, double duration)
    {
        if (values.empty() || duration <= 0.0)
        {
            return;
        }

        InsertEvent(events, Event{startTime, EventType::SetCurve, 0.f, values, duration});
    }

    void AudioParamState::SetImmediate(float value, double currentTime)
    {
        CancelScheduledValues(currentTime);
        SetValueAtTime(value, currentTime);
    }

    float AudioParamState::GetAtTime(double t) const
    {
        if (events.empty())
        {
            return defaultValue;
        }

        float value = defaultValue;

        for (size_t index = 0; index < events.size(); ++index)
        {
            const Event& event = events[index];

            if (event.type == EventType::SetCurve)
            {
                if (t < event.time)
                {
                    break;
                }

                if (t <= event.time + event.duration)
                {
                    const auto& curve = event.curve;
                    if (curve.empty())
                    {
                        break;
                    }

                    if (curve.size() == 1)
                    {
                        return curve[0];
                    }

                    const double progress = (t - event.time) / std::max(event.duration, 1.0e-12);
                    const double floatIndex = progress * static_cast<double>(curve.size() - 1);
                    const size_t index0 = static_cast<size_t>(floatIndex);
                    const size_t index1 = std::min(index0 + 1, curve.size() - 1);
                    const float fraction = static_cast<float>(floatIndex - static_cast<double>(index0));
                    return curve[index0] + fraction * (curve[index1] - curve[index0]);
                }

                value = event.curve.empty() ? value : event.curve.back();
                continue;
            }

            if (t < event.time)
            {
                if (index == 0)
                {
                    return value;
                }

                const double startTime = events[index - 1].time;
                const double endTime = event.time;
                if (endTime <= startTime)
                {
                    return value;
                }

                const float alpha = static_cast<float>((t - startTime) / (endTime - startTime));
                switch (event.type)
                {
                case EventType::SetValue:
                    return value;
                case EventType::LinearRamp:
                    return value + alpha * (event.value - value);
                case EventType::ExponentialRamp:
                {
                    if (value == 0.f || event.value == 0.f || (value < 0.f) != (event.value < 0.f))
                    {
                        return value + alpha * (event.value - value);
                    }

                    return static_cast<float>(value * std::pow(event.value / value, (t - startTime) / (endTime - startTime)));
                }
                default:
                    return value;
                }
            }

            switch (event.type)
            {
            case EventType::SetValue:
                value = event.value;
                break;
            case EventType::LinearRamp:
                value = event.value;
                break;
            case EventType::ExponentialRamp:
                value = event.value;
                break;
            default:
                break;
            }
        }

        return value;
    }
}
