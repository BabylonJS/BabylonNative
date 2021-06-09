#include <Babylon/Plugins/NativeAudio.h>

#include <Babylon/JsRuntime.h>
#include <LabSound/LabSound.h>

#include <iostream>

#if WIN32
    #include<windows.h>
    #undef GetCurrentTime()
#endif

namespace Babylon::Plugins::Internal
{
    class MediaElementAudioSourceNode : public Napi::ObjectWrap<MediaElementAudioSourceNode>
    {
    public:
        static constexpr auto JS_CLASS_NAME = "MediaElementAudioSourceNode";

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = MediaElementAudioSourceNode::DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // methods we want the javascript object to implement.
                    MediaElementAudioSourceNode::InstanceAccessor("mediaElement", &MediaElementAudioSourceNode::GetMediaElement, nullptr)
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }

        MediaElementAudioSourceNode(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<MediaElementAudioSourceNode>{info}
        {
        }

        Napi::Value GetMediaElement(const Napi::CallbackInfo& info)
        {
            // Returns the HTMLMediaElement used when constructing this MediaStreamAudioSourceNode.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"MediaElementAudioSourceNode::GetMediaElement() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }
    };

    class AudioNode : public Napi::ObjectWrap<AudioNode>
    {
        static constexpr auto JS_CLASS_NAME = "AudioNode";

    public:
        static void Initialize(Napi::Env env) 
        {
            Napi::HandleScope scope{env};
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // Properties expected of the javascript object.
                    AudioNode::InstanceAccessor("context", &AudioNode::GetContext, nullptr),
                    AudioNode::InstanceAccessor("numberOfInputs", &AudioNode::GetNumberOfInputs, nullptr),
                    AudioNode::InstanceAccessor("numberOfOutputs", &AudioNode::GetNumberOfOutputs, nullptr),
                    AudioNode::InstanceAccessor("channelCount", &AudioNode::GetChannelCount, nullptr),
                    AudioNode::InstanceAccessor("channelCountMode", &AudioNode::GetChannelCountMode, nullptr),
                    AudioNode::InstanceAccessor("channelInterpretation", &AudioNode::GetChannelInterpretation, nullptr),
                    
                    // methods we want the javascript object to implement.
                    AudioNode::InstanceMethod("connect", &AudioNode::Connect),
                    AudioNode::InstanceMethod("disconnect", &AudioNode::Disconnect),
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }

        AudioNode(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<AudioNode>{info}
        {
        }

        Napi::Value GetContext(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioNode::GetContext()");
#endif
            return Napi::Object::New(info.Env());
        }
        
        Napi::Value GetNumberOfInputs(const Napi::CallbackInfo& info)
        {
            // Returns the number of inputs feeding the node. Source nodes are defined as nodes having a numberOfInputs property with a value of 0.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioNode::GetContext()");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value GetNumberOfOutputs(const Napi::CallbackInfo& info)
        {
            // Returns the number of outputs coming out of the node. Destination nodes — like AudioDestinationNode — have a value of 0 for this attribute.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioNode::GetContext()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetChannelCount(const Napi::CallbackInfo& info)
        {
            // Represents an integer used to determine how many channels are used when up-mixing and down-mixing connections to any inputs to the node. Its usage and precise definition depend on the value of AudioNode.channelCountMode.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioNode::GetContext()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetChannelCountMode(const Napi::CallbackInfo& info)
        {
            // Represents an enumerated value describing the way channels must be matched between the node's inputs and outputs.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioNode::GetContext()");
#endif
            return Napi::String::From(info.Env(), "");
        }

        Napi::Value GetChannelInterpretation(const Napi::CallbackInfo& info)
        {
            // Represents an enumerated value describing the meaning of the channels.This interpretation will define how audio up - mixing and down - mixing will happen.The possible values are "speakers" or "discrete".
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioNode::GetContext()");
#endif
            return Napi::String::From(info.Env(), "");
        }

        //
        // AudioNode method implementations.
        //

        void Connect(const Napi::CallbackInfo& info)
        {
            // Allows us to connect the output of this node to be input into another node, either as audio data or as the value of an AudioParam.
            //TODO: Implement, as its used by Babylon.js
            // unwrap the passed Napi::Object to get a reference to its lab::AudioNode
            // connect our lab::AudioNode to the passed node.

            // there are multiple overrides for this method, do we need to handle all of them here?
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioNode::Connect()");
#endif
        }

        void Disconnect(const Napi::CallbackInfo& info)
        {
            // Allows us to disconnect the current node from another one it is already connected to.
            //TODO: Implement, as its used by Babylon.js
            
            // unwrap the passed Napi::Object to get a reference to its lab::AudioNode
            // disconnect our lab::AudioNode from the passed node.
            
            // there are multiple overrides for this method, do we need to handle all of them here?

            //const auto nodeObj = info[0].As<Napi::Object>();
            //const auto audioNode = AudioNode::Unwrap(nodeObj);
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioNode::Disconnect()");
#endif
        }

    private:
        //std::shared_ptr<lab::AudioNode> m_audioNode;
    };

    class AudioParam : public Napi::ObjectWrap<AudioParam>
    {
        static constexpr auto JS_CLASS_NAME = "AudioParam";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // Properties expected of the javascript object.
                    AudioParam::InstanceAccessor("defaultValue", &AudioParam::GetDefaultValue, nullptr),
                    AudioParam::InstanceAccessor("maxValue", &AudioParam::GetMaxValue, nullptr),
                    AudioParam::InstanceAccessor("minValue", &AudioParam::GetMinValue, nullptr),
                    AudioParam::InstanceAccessor("value", &AudioParam::GetValue, &AudioParam::SetValue),

                    // methods we want the javascript object to implement.
                    AudioParam::InstanceMethod("setValueAtTime", &AudioParam::SetValueAtTime),
                    AudioParam::InstanceMethod("linearRampToValueAtTime", &AudioParam::LinearRampToValueAtTime),
                    AudioParam::InstanceMethod("exponentialRampToValueAtTime", &AudioParam::ExponentialRampToValueAtTime),
                    AudioParam::InstanceMethod("setTargetAtTime", &AudioParam::SetTargetAtTime),
                    AudioParam::InstanceMethod("setValueCurveAtTime", &AudioParam::SetValueCurveAtTime),
                    AudioParam::InstanceMethod("cancelScheduledValues", &AudioParam::CancelScheduledValues),
                    AudioParam::InstanceMethod("cancelAndHoldAtTime", &AudioParam::CancelAndHoldAtTime),
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }

        AudioParam(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<AudioParam>{info}
        {
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0], info[1]});
        }

        //
        // AudioParam Accessor implementations
        //

        Napi::Value GetDefaultValue(const Napi::CallbackInfo& info)
        {
            // Represents the initial volume of the attribute as defined by the specific AudioNode creating the AudioParam.
            // UNUSED

#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::GetDefaultValue()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetMaxValue(const Napi::CallbackInfo& info)
        {
            // Represents the maximum possible value for the parameter's nominal (effective) range.
            // UNUSED

#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::GetMaxValue()");
#endif
            return Napi::Number::From(info.Env(), 1);
        }

        Napi::Value GetMinValue(const Napi::CallbackInfo& info)
        {
            // Represents the minimum possible value for the parameter's nominal (effective) range.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::GetMinValue()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetValue(const Napi::CallbackInfo& info)
        {
            // Represents the parameter's current value as of the current time; initially set to the value of defaultValue.
            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::GetValue()");
#endif
            return Napi::Number::From(info.Env(), m_value);
        }

        void SetValue(const Napi::CallbackInfo& info, const Napi::Value& value)
        {
            // Represents the parameter's current value as of the current time; initially set to the value of defaultValue.
            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::SetValue()");
#endif
            m_value = value.As<Napi::Number>().FloatValue();
        }

        //
        // AudioParam Method implementations
        //

        void SetValueAtTime(const Napi::CallbackInfo& info)
        {
            // Schedules an instant change to the value of the AudioParam at a precise time, as measured against AudioContext.currentTime. The new value is given by the value parameter.
            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::SetValueAtTime()");
#endif
        }

        void LinearRampToValueAtTime(const Napi::CallbackInfo& info)
        {
            // Schedules a gradual linear change in the value of the AudioParam. The change starts at the time specified for the previous event, follows a linear ramp to the new value given in the value parameter, and reaches the new value at the time given in the endTime parameter.
            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::LinearRampToValueAtTime()");
#endif
        }

        void ExponentialRampToValueAtTime(const Napi::CallbackInfo& info)
        {
            // Schedules a gradual exponential change in the value of the AudioParam. The change starts at the time specified for the previous event, follows an exponential ramp to the new value given in the value parameter, and reaches the new value at the time given in the endTime parameter.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::ExponentialRampToValueAtTime()");
#endif
        }

        void SetTargetAtTime(const Napi::CallbackInfo& info)
        {
            // Schedules the start of a change to the value of the AudioParam. The change starts at the time specified in startTime and exponentially moves towards the value given by the target parameter. The exponential decay rate is defined by the timeConstant parameter, which is a time measured in seconds.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::SetTargetAtTime()");
#endif
        }

        void SetValueCurveAtTime(const Napi::CallbackInfo& info)
        {
            // Schedules the values of the AudioParam to follow a set of values, defined by an array of floating-point numbers scaled to fit into the given interval, starting at a given start time and spanning a given duration of time.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::SetValueCurveAtTime()");
#endif
        }

        void CancelScheduledValues(const Napi::CallbackInfo& info)
        {
            // Cancels all scheduled future changes to the AudioParam.
            //TODO: Implement, as its used by Babylon.js0
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::CancelScheduledValues()");
#endif
        }

        void CancelAndHoldAtTime(const Napi::CallbackInfo& info)
        {
            // Cancels all scheduled future changes to the AudioParam but holds its value at a given time until further changes are made using other methods.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioParam::CancelAndHoldAtTime()");
#endif
        }

    private:
        float m_value{};
    };

    class AudioScheduledSourceNode : public Napi::ObjectWrap<AudioScheduledSourceNode>
    {
        static constexpr auto JS_CLASS_NAME = "AudioScheduledSourceNode";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // UNUSED
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }

        AudioScheduledSourceNode(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<AudioScheduledSourceNode>{info}
        {
        }
    };

    class OscillatorNode : public Napi::ObjectWrap<OscillatorNode>
    {
        static constexpr auto JS_CLASS_NAME = "OscillatorNode";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // UNUSED
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }
        OscillatorNode(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<OscillatorNode>{info}
        {
        }
    };

    class AudioBuffer : public Napi::ObjectWrap<AudioBuffer>
    {
        static constexpr auto JS_CLASS_NAME = "AudioBuffer";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // Properties expected of the javascript object.
                    AudioBuffer::InstanceAccessor("sampleRate", &AudioBuffer::GetSampleRate, nullptr),
                    AudioBuffer::InstanceAccessor("length", &AudioBuffer::GetLength, nullptr),
                    AudioBuffer::InstanceAccessor("duration", &AudioBuffer::GetDuration, nullptr),
                    AudioBuffer::InstanceAccessor("numberOfChannels", &AudioBuffer::GetNumberOfChannels, nullptr),
                    AudioBuffer::InstanceAccessor("getChannelData", &AudioBuffer::GetChannelData, nullptr),

                    // methods we want the javascript object to implement.
                    AudioBuffer::InstanceMethod("copyFromChannel", &AudioBuffer::CopyFromChannel),
                    AudioBuffer::InstanceMethod("copyToChannel", &AudioBuffer::CopyToChannel),
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }

        AudioBuffer(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<AudioBuffer>{info}
        {
        }

        Napi::Value GetSampleRate(const Napi::CallbackInfo& info)
        {
            // Returns a float representing the sample rate, in samples per second, of the PCM data stored in the buffer.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioBuffer::GetSampleRate()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetLength(const Napi::CallbackInfo& info)
        {
            // Returns an integer representing the length, in sample-frames, of the PCM data stored in the buffer.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioBuffer::GetLength()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetDuration(const Napi::CallbackInfo& info)
        {
            // Returns a double representing the duration, in seconds, of the PCM data stored in the buffer.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioBuffer::GetDuration()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetNumberOfChannels(const Napi::CallbackInfo& info)
        {
            // Returns a float representing the sample rate, in samples per second, of the PCM data stored in the buffer.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioBuffer::GetNumberOfChannels()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetChannelData(const Napi::CallbackInfo& info)
        {
            // Returns a Float32Array containing the PCM data associated with the channel, defined by the channel parameter (with 0 representing the first channel).
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioBuffer::GetChannelData()");
#endif
            return Napi::Float32Array::New(info.Env(), 0);
        }

        void CopyFromChannel(const Napi::CallbackInfo& info)
        {
            // Copies the samples from the specified channel of the AudioBuffer to the destination array.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioBuffer::CopyFromChannel()");
#endif
        }

        void CopyToChannel(const Napi::CallbackInfo& info)
        {
            // Copies the samples to the specified channel of the AudioBuffer, from the source array.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioBuffer::CopyToChannel()");
#endif
        }
    };

    class AudioBufferSourceNode : public Napi::ObjectWrap<AudioBufferSourceNode>
    {
        static constexpr auto JS_CLASS_NAME = "AudioBufferSourceNode ";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // Properties expected of the javascript object.
                    
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }

        AudioBufferSourceNode(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<AudioBufferSourceNode>{info}
        {
        }
    };

    class AudioDestinationNode : public Napi::ObjectWrap<AudioDestinationNode>
    {
    public:
        static constexpr auto JS_CLASS_NAME = "AudioDestinationNode";

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    AudioDestinationNode::InstanceAccessor("maxChannelCount", &AudioDestinationNode::GetMaxChannelCount, nullptr)
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }

        AudioDestinationNode(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<AudioDestinationNode>{info}
        {
            // AudioDestinationNode(AudioContext context);
            //m_audioNode = AudioContext::Unwrap(info[0].As<Napi::Object>())->GetAudioContext()->device();
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0], info[1]});
        }

        Napi::Value GetMaxChannelCount(const Napi::CallbackInfo& info)
        {
            // Returns an unsigned long defining the maximum number of channels that the physical device can handle.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioDestinationNode::GetMaxChannelCount()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }
    };

    class AudioListener : public Napi::ObjectWrap<AudioListener>
    {
        static constexpr auto JS_CLASS_NAME = "AudioListener";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = AudioListener::DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // Properties expected of the javascript object.
                    AudioListener::InstanceAccessor("positionX", &AudioListener::GetPositionX, nullptr),
                    AudioListener::InstanceAccessor("positionY", &AudioListener::GetPositionY, nullptr),
                    AudioListener::InstanceAccessor("positionZ", &AudioListener::GetPositionZ, nullptr),
                    AudioListener::InstanceAccessor("forwardX", &AudioListener::GetForwardX, nullptr),
                    AudioListener::InstanceAccessor("forwardY", &AudioListener::GetForwardY, nullptr),
                    AudioListener::InstanceAccessor("forwardZ", &AudioListener::GetForwardZ, nullptr),
                    AudioListener::InstanceAccessor("upX", &AudioListener::GetUpX, nullptr),
                    AudioListener::InstanceAccessor("upY", &AudioListener::GetUpY, nullptr),
                    AudioListener::InstanceAccessor("upZ", &AudioListener::GetUpZ, nullptr),

                    // methods we want the javascript object to implement.
                    AudioListener::InstanceMethod("setPosition", &AudioListener::SetPosition),
                    AudioListener::InstanceMethod("setOrientation", &AudioListener::SetOrientation)
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
        }

        AudioListener(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<AudioListener>{info}
        {
        }

        Napi::Value GetPositionX(const Napi::CallbackInfo& info)
        {
            // Represents the horizontal position of the listener in a right-hand cartesian coordinate system. The default is 0.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::GetPositionX()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetPositionY(const Napi::CallbackInfo& info)
        {
            // Represents the vertical position of the listener in a right-hand cartesian coordinate system. The default is 0.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::GetPositionY()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetPositionZ(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::GetPositionZ()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetUpX(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::GetUpX()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetUpY(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::GetUpY()");
#endif
            return Napi::Number::From(info.Env(), 1);
        }

        Napi::Value GetUpZ(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::GetUpZ()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetForwardX(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::GetForwardX()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetForwardY(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::GetForwardY()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetForwardZ(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::GetForwardZ()");
#endif
            return Napi::Number::From(info.Env(), -1);
        }


        void SetPosition(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::SetPosition()");
#endif
        }

        void SetOrientation(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioListener::SetOrientation()");
#endif
        }
    };

    class PannerNode : public Napi::ObjectWrap<PannerNode>
    {
        //TODO implement the currently-used portions of this...
    };

    class GainNode : public Napi::ObjectWrap<GainNode>//, public AudioNode
    {
        static constexpr auto JS_CLASS_NAME = "GainNode";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = GainNode::DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    // Properties expected of the javascript object.
                    GainNode::InstanceAccessor("gain", &GainNode::GetGain, nullptr),
                    // Properties expected of the javascript object.
                    GainNode::InstanceAccessor("context", &GainNode::GetContext, nullptr),
                    GainNode::InstanceAccessor("numberOfInputs", &GainNode::GetNumberOfInputs, nullptr),
                    GainNode::InstanceAccessor("numberOfOutputs", &GainNode::GetNumberOfOutputs, nullptr),
                    GainNode::InstanceAccessor("channelCount", &GainNode::GetChannelCount, nullptr),
                    GainNode::InstanceAccessor("channelCountMode", &GainNode::GetChannelCountMode, nullptr),
                    GainNode::InstanceAccessor("channelInterpretation", &GainNode::GetChannelInterpretation, nullptr),

                    // methods we want the javascript object to implement.
                    GainNode::InstanceMethod("connect", &GainNode::Connect),
                    GainNode::InstanceMethod("disconnect", &GainNode::Disconnect),
                });
            env.Global().Set(JS_CLASS_NAME, func);
        }

        Napi::Value GetContext(const Napi::CallbackInfo& info)
        {
            // Returns the associated AudioContext, that is the object representing the processing graph the node is participating in.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"GainNode::GetContext()");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value GetNumberOfInputs(const Napi::CallbackInfo& info)
        {
            // Returns the number of inputs feeding the node. Source nodes are defined as nodes having a numberOfInputs property with a value of 0.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"GainNode::GetContext()");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value GetNumberOfOutputs(const Napi::CallbackInfo& info)
        {
            // Returns the number of outputs coming out of the node. Destination nodes — like AudioDestinationNode — have a value of 0 for this attribute.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"GainNode::GetContext()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetChannelCount(const Napi::CallbackInfo& info)
        {
            // Represents an integer used to determine how many channels are used when up-mixing and down-mixing connections to any inputs to the node. Its usage and precise definition depend on the value of GainNode.channelCountMode.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"GainNode::GetContext()");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetChannelCountMode(const Napi::CallbackInfo& info)
        {
            // Represents an enumerated value describing the way channels must be matched between the node's inputs and outputs.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"GainNode::GetContext()");
#endif
            return Napi::String::From(info.Env(), "");
        }

        Napi::Value GetChannelInterpretation(const Napi::CallbackInfo& info)
        {
            // Represents an enumerated value describing the meaning of the channels.This interpretation will define how audio up - mixing and down - mixing will happen.The possible values are "speakers" or "discrete".
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"GainNode::GetContext()");
#endif
            return Napi::String::From(info.Env(), "");
        }

        //
        // GainNode method implementations.
        //

        void Connect(const Napi::CallbackInfo& info)
        {
            // Allows us to connect the output of this node to be input into another node, either as audio data or as the value of an AudioParam.
            //TODO: Implement, as its used by Babylon.js
            // unwrap the passed Napi::Object to get a reference to its lab::AudioNode
            // connect our lab::GainNode to the passed node.

            // there are multiple overrides for this method, do we need to handle all of them here?
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"GainNode::Connect()");
#endif
        }

        void Disconnect(const Napi::CallbackInfo& info)
        {
            // Allows us to disconnect the current node from another one it is already connected to.
            //TODO: Implement, as its used by Babylon.js

            // unwrap the passed Napi::Object to get a reference to its lab::GainNode
            // disconnect our lab::GainNode from the passed node.

            // there are multiple overrides for this method, do we need to handle all of them here?

            //const auto nodeObj = info[0].As<Napi::Object>();
            //const auto GainNode = GainNode::Unwrap(nodeObj);
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"GainNode::Disconnect()");
#endif
        }
        
        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
        }

        GainNode(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<GainNode>{info}
            //, AudioNode{info}
        {
            // TODO:
            m_audioParam_gain = Napi::Persistent(AudioParam::New(info));
        }

    private:
        Napi::Value GetGain(const Napi::CallbackInfo& info) {
            // Returns an unsigned long defining the maximum number of channels that the physical device can handle.
            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioDestinationNode::GetMaxChannelCount()");
#endif
            return m_audioParam_gain.Value();
        }

        Napi::ObjectReference m_audioParam_gain{};
    };

    class AudioContext : public Napi::ObjectWrap<AudioContext>
    {
    public:
        static constexpr auto JS_CLASS_NAME = "AudioContext";
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};
            Napi::Function func = AudioContext::DefineClass(
                env,
                JS_CLASS_NAME,
                {// methods we want the javascript object to implement.
                    AudioContext::InstanceMethod("close", &AudioContext::Close),
                    AudioContext::InstanceMethod("createMediaElementSource", &AudioContext::CreateMediaElementSource),
                    AudioContext::InstanceMethod("createMediaStreamSource", &AudioContext::CreateMediaStreamSource),
                    AudioContext::InstanceMethod("createMediaStreamDestination", &AudioContext::CreateMediaStreamDestination),
                    AudioContext::InstanceMethod("createMediaStreamTrackSource", &AudioContext::CreateMediaStreamTrackSource),
                    AudioContext::InstanceMethod("getOutputTimestamp", &AudioContext::GetOutputTimestamp),
                    AudioContext::InstanceMethod("resume", &AudioContext::Resume),
                    AudioContext::InstanceMethod("suspend", &AudioContext::Suspend),

                    AudioContext::InstanceAccessor("baseLatency", &AudioContext::GetBaseLatency, nullptr),
                    AudioContext::InstanceAccessor("outputLatency", &AudioContext::GetOutputLatency, nullptr),

                    // BaseAudioContext inherited members.
                    AudioContext::InstanceMethod("createAnalyserSource", &AudioContext::CreateAnalyser),
                    AudioContext::InstanceMethod("createBiquadFilter", &AudioContext::CreateBiquadFilter),
                    AudioContext::InstanceMethod("createBuffer", &AudioContext::CreateBuffer),
                    AudioContext::InstanceMethod("createBufferSource", &AudioContext::CreateBufferSource),
                    AudioContext::InstanceMethod("createConstantSource", &AudioContext::CreateConstantSource),
                    AudioContext::InstanceMethod("createChannelMerger", &AudioContext::CreateChannelMerger),
                    AudioContext::InstanceMethod("createChannelSplitter", &AudioContext::CreateChannelSplitter),
                    AudioContext::InstanceMethod("createConvolver", &AudioContext::CreateConvolver),
                    AudioContext::InstanceMethod("createDelay", &AudioContext::CreateDelay),
                    AudioContext::InstanceMethod("createDynamicsCompressor", &AudioContext::CreateDynamicsCompressor),
                    AudioContext::InstanceMethod("createGain", &AudioContext::CreateGain),
                    AudioContext::InstanceMethod("createIIRFilter", &AudioContext::CreateIIRFilter),
                    AudioContext::InstanceMethod("createOscillator", &AudioContext::CreateOscillator),
                    AudioContext::InstanceMethod("createPanner", &AudioContext::CreatePanner),
                    AudioContext::InstanceMethod("createPeriodicWave", &AudioContext::CreatePeriodicWave),
                    AudioContext::InstanceMethod("createScriptProcessor", &AudioContext::CreateScriptProcessor),
                    AudioContext::InstanceMethod("createStereoPanner", &AudioContext::CreateStereoPanner),
                    AudioContext::InstanceMethod("createWaveShaper", &AudioContext::CreateWaveShaper),
                    AudioContext::InstanceMethod("decodeAudioData", &AudioContext::DecodeAudioData),

                    // Event handler for onStateChange AudioContext event.
                    AudioContext::InstanceAccessor("onstatechange", &AudioContext::GetOnStateChange, nullptr /*&AudioContext::SetOnStateChange*/),

                    AudioContext::InstanceAccessor("audioWorklet", &AudioContext::GetAudioWorklet, nullptr),
                    AudioContext::InstanceAccessor("currentTime", &AudioContext::GetCurrentTime, nullptr),
                    AudioContext::InstanceAccessor("destination", &AudioContext::GetDestination, nullptr),
                    AudioContext::InstanceAccessor("listener", &AudioContext::GetListener, nullptr),
                    AudioContext::InstanceAccessor("sampleRate", &AudioContext::GetSampleRate, nullptr),
                    AudioContext::InstanceAccessor("state", &AudioContext::GetState, nullptr)});
            env.Global().Set(JS_CLASS_NAME, func);
        }

        AudioContext(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<AudioContext>{info}
        //, m_audioContext{lab::MakeRealtimeAudioContext(lab::GetDefaultOutputAudioDeviceConfiguration(), lab::GetDefaultInputAudioDeviceConfiguration())}
        {
            m_listener = Napi::Persistent(AudioListener::New(info));
            m_destination = Napi::Persistent(AudioDestinationNode::New(info));
        }

        ~AudioContext()
        {
        }

        //
        // AudioContext methods
        //

        void Close(const Napi::CallbackInfo& info)
        {
            // Closes the audio context, releasing any system audio resources that it uses.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::Close() is unimplemented.");
#endif
        }

        Napi::Value CreateMediaElementSource(const Napi::CallbackInfo& info)
        {
            // Used to create a new MediaElementAudioSourceNode object, given an existing HTML <audio> or <video> element, the audio from which can then be played and manipulated.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateMediaElementSource() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateMediaStreamSource(const Napi::CallbackInfo& info)
        {
            // Used to create a new MediaStreamAudioSourceNode object, given a media stream (say, from a MediaDevices.getUserMedia instance), the audio from which can then be played and manipulated.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateMediaStreamSource() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateMediaStreamDestination(const Napi::CallbackInfo& info)
        {
            // Used to create a new MediaStreamAudioDestinationNode object associated with a WebRTC MediaStream representing an audio stream, which may be stored in a local file or sent to another computer.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateMediaStreamDestination() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateMediaStreamTrackSource(const Napi::CallbackInfo& info)
        {
            // Creates and returns a MediaStreamTrackAudioSourceNode which represents an audio source whose data comes from the specified MediaStreamTrack.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateMediaStreamTrackSource() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value GetOutputTimestamp(const Napi::CallbackInfo& info)
        {
            // Returns a new AudioTimestamp object containing two audio timestamp values relating to the current audio context.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetOutputTimestamp() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        void Resume(const Napi::CallbackInfo& info)
        {
            // Resumes the progression of time in an audio context that has previously been suspended.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::Resume() is unimplemented.");
#endif
        }

        void Suspend(const Napi::CallbackInfo& info)
        {
            // Suspends the progression of time in the audio context, temporarily halting audio hardware access and reducing CPU/battery usage in the process

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::Suspend() is unimplemented.");
#endif
        }

        //
        // AudioContext accessors
        //

        Napi::Value GetBaseLatency(const Napi::CallbackInfo& info)
        {
            // Returns a double that represents the number of seconds of processing latency incurred by the AudioContext passing an audio buffer from the AudioDestinationNode — i.e. the end of the audio graph — into the host system's audio subsystem ready for playing.

            // UNUSED

#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetBaseLatency() is unimplemented.");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetOutputLatency(const Napi::CallbackInfo& info)
        {
            // This is the time, in seconds, between the browser passing an audio buffer out of an audio graph over to the host system's audio subsystem to play, and the time at which the first sample in the buffer is actually processed by the audio output device.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetOutputLatency() is unimplemented.");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        //
        // BaseAudioContext Implementation
        //

        Napi::Value CreateAnalyser(const Napi::CallbackInfo& info)
        {
            // Creates an AnalyserNode, which can be used to expose audio time and frequency data and for example to create data visualisations.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateAnalyser() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateBiquadFilter(const Napi::CallbackInfo& info)
        {
            // Creates a BiquadFilterNode, which represents a second order filter configurable as several different common filter types: high-pass, low-pass, band-pass, etc

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateBiquadFilter() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateBuffer(const Napi::CallbackInfo& info)
        {
            // Creates a new, empty AudioBuffer object, which can then be populated by data and played via an AudioBufferSourceNode.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateBuffer() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateBufferSource(const Napi::CallbackInfo& info)
        {
            // Creates an AudioBufferSourceNode, which can be used to play and manipulate audio data contained within an AudioBuffer object. AudioBuffers are created using AudioContext.createBuffer() or returned by AudioContext.decodeAudioData() when it successfully decodes an audio track.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateBufferSource() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateConstantSource(const Napi::CallbackInfo& info)
        {
            // Creates a ConstantSourceNode object, which is an audio source that continuously outputs a monaural (one-channel) sound signal whose samples all have the same value.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateConstantSource() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateChannelMerger(const Napi::CallbackInfo& info)
        {
            // Creates a ChannelMergerNode, which is used to combine channels from multiple audio streams into a single audio stream.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateChannelMerger() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateChannelSplitter(const Napi::CallbackInfo& info)
        {
            // Creates a ChannelSplitterNode, which is used to access the individual channels of an audio stream and process them separately.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateChannelSplitter() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateConvolver(const Napi::CallbackInfo& info)
        {
            // Creates a ConvolverNode, which can be used to apply convolution effects to your audio graph, for example a reverberation effect.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateConvolver() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateDelay(const Napi::CallbackInfo& info)
        {
            // Creates a DelayNode, which is used to delay the incoming audio signal by a certain amount. This node is also useful to create feedback loops in a Web Audio API graph.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateDelay() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateDynamicsCompressor(const Napi::CallbackInfo& info)
        {
            // Creates a DynamicsCompressorNode, which can be used to apply acoustic compression to an audio signal.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateDynamicsCompressor() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateGain(const Napi::CallbackInfo& info)
        {
            // Creates a GainNode, which can be used to control the overall volume of the audio graph.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateGain() is unimplemented.");
#endif
            return GainNode::New(info);
        }

        Napi::Value CreateIIRFilter(const Napi::CallbackInfo& info)
        {
            // Creates an IIRFilterNode, which represents a second order filter configurable as several different common filter types.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateIIRFilter() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateOscillator(const Napi::CallbackInfo& info)
        {
            // Creates an OscillatorNode, a source representing a periodic waveform. It basically generates a tone.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateOscillator() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreatePanner(const Napi::CallbackInfo& info)
        {
            // Creates a PannerNode, which is used to spatialise an incoming audio stream in 3D space.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreatePanner() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreatePeriodicWave(const Napi::CallbackInfo& info)
        {
            // Creates a PeriodicWave, used to define a periodic waveform that can be used to determine the output of an OscillatorNode.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreatePeriodicWave() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateScriptProcessor(const Napi::CallbackInfo& info)
        {
            // Creates a ScriptProcessorNode, which can be used for direct audio processing via JavaScript.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateScriptProcessor() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateStereoPanner(const Napi::CallbackInfo& info)
        {
            // Creates a StereoPannerNode, which can be used to apply stereo panning to an audio source.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateStereoPanner() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value CreateWaveShaper(const Napi::CallbackInfo& info)
        {
            // Creates a WaveShaperNode, which is used to implement non-linear distortion effects.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::CreateWaveShaper() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value DecodeAudioData(const Napi::CallbackInfo& info)
        {
            // Asynchronously decodes audio file data contained in an ArrayBuffer. In this case, the ArrayBuffer is usually loaded from an XMLHttpRequest's response attribute after setting the responseType to arraybuffer. This method only works on complete files, not fragments of audio files.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::DecodeAudioData() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        //
        // BaseAudioContext Implementation
        //

        Napi::Value GetOnStateChange(const Napi::CallbackInfo& info)
        {
            // Defines an event handler function to be called when the statechange event fires: this occurs when the audio context's state changes.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetOnStateChange() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        void SetOnStateChange(const Napi::CallbackInfo& info)
        {
            // Defines an event handler function to be called when the statechange event fires: this occurs when the audio context's state changes.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::SetOnStateChange() is unimplemented.");
#endif
        }

        Napi::Value GetAudioWorklet(const Napi::CallbackInfo& info)
        {
            // Returns the AudioWorklet object, which can be used to create and manage AudioNodes in which JavaScript code implementing the AudioWorkletProcessor interface are run in the background to process audio data.

            // UNUSED
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetAudioWorklet() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value GetCurrentTime(const Napi::CallbackInfo& info)
        {
            // Returns a double representing an ever-increasing hardware time in seconds used for scheduling. It starts at 0.

            //TODO: Implement, as its used by Babylon.js
            //return Napi::Number::From(info.Env(), m_audioContext->currentTime());
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetCurrentTime() is unimplemented.");
#endif
            return Napi::Number::From(info.Env(), 0);
        }

        Napi::Value GetDestination(const Napi::CallbackInfo& info)
        {
            // Returns an AudioDestinationNode representing the final destination of all audio in the context. It can be thought of as the audio-rendering device.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetDestination() is unimplemented.");
#endif
            return Napi::Object::New(info.Env());
        }

        Napi::Value GetListener(const Napi::CallbackInfo& info)
        {
            // Returns the AudioListener object, used for 3D spatialization.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetListener()");
#endif
            return m_listener.Value();
        }

        Napi::Value GetSampleRate(const Napi::CallbackInfo& info)
        {
            // Returns a float representing the sample rate (in samples per second) used by all nodes in this context. The sample-rate of an AudioContext cannot be changed.

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetSampleRate() is unimplemented.");
#endif
            return Napi::Value::From(info.Env(), 0);
        }

        Napi::Value GetState(const Napi::CallbackInfo& info)
        {
            // Returns the current state of the AudioContext.
            // Possible values are:
            // "suspended" : The audio context has been suspended(with the AudioContext.suspend() method.)
            // "running" : The audio context is running normally.
            // "closed" : The audio context has been closed(with the AudioContext.close() method.)

            //TODO: Implement, as its used by Babylon.js
#if WIN32
            //TODO: we need a native-side debug logging solution...
            OutputDebugStringW(L"AudioContext::GetState() is unimplemented.");
#endif
            return Napi::String::From(info.Env(), "running");
        }

        lab::AudioContext* GetAudioContext()
        {
            return m_audioContext.get();
        }

    private:
        std::unique_ptr<lab::AudioContext> m_audioContext{};
        Napi::ObjectReference m_destination{};
        Napi::ObjectReference m_listener{};
    };

    class NativeAudio : public Napi::ObjectWrap<NativeAudio>
    {
    public:
        static constexpr auto JS_CLASS_NAME = "NativeAudio";

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            // Setup all our WebAudio class implementations in the JS environment.
            // Commented component initializations here represent the WebAudio surface that the spec describes, but isn't implemented here.

            AudioContext::Initialize(env);
            //AudioContextOptions::Initialize(env);
            AudioNode::Initialize(env);
            AudioParam::Initialize(env);
            //AudioParamMap::Initialize(env);
            
            AudioScheduledSourceNode::Initialize(env);
            OscillatorNode::Initialize(env);
            AudioBuffer::Initialize(env);
            AudioBufferSourceNode::Initialize(env);
            MediaElementAudioSourceNode::Initialize(env);
            //MediaStreamAudioSourceNode::Initialize(env);
            //MediaStreamTrackAudioSourceNode::Initialize(env);

            //BiquadFilterNode::Initialize(env);
            //ConvolverNode::Initialize(env);
            //DelayNode::Initialize(env);
            //DynamicsCompressorNode::Initialize(env);
            GainNode::Initialize(env);
            //PeriodicWave::Initialize(env);
            //IIRFilterNode::Initialize(env);

            AudioDestinationNode::Initialize(env);
            //MediaStreamAudioDestinationNode::Initialize(env);

            //AnalyserNode::Initialize(env);

            //ChannelSplitterNode::Initialize(env);
            //ChannelMergerNode::Initialize(env);

            AudioListener::Initialize(env);
            //PannerNode::Initialize(env);
            //StereoPannerNode::Initialize(env);

            //AudioWorklet::Initialize(env);
            //AudioWorkletNode::Initialize(env);
            //AudioWorkletGlobalScope::Initialize(env);

            //ScriptProcessorNode::Initialize(env);

            //OfflineAudioContext::Initialize(env);
        }

        NativeAudio(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeAudio>{info}
            , m_runtime{JsRuntime::GetFromJavaScript(info.Env())}
            , m_jsData{Napi::Persistent(Napi::Object::New(info.Env()))}
        {
            Napi::Object jsData = m_jsData.Value();
            jsData.Set("data", Napi::ArrayBuffer::New(info.Env(), 0));
        }

        ~NativeAudio()
        {
        }

    private:
        void AddCallback(const Napi::CallbackInfo& info)
        {
        }

        void Dispose()
        {
        }

        void Dispose(const Napi::CallbackInfo&)
        {
            Dispose();
        }

        JsRuntime& m_runtime;
        Napi::ObjectReference m_jsData{};
    };
}


namespace Babylon::Plugins::NativeAudio
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::NativeAudio::Initialize(env);
    }
}