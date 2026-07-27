#pragma once

#include "AudioNode.h"

#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    class NativePannerNode final : public Napi::ObjectWrap<NativePannerNode>
    {
    public:
        static void Initialize(Napi::Env env);
        static Napi::Object CreateInstance(Napi::Env env, const std::shared_ptr<AudioContextState>& context);

        explicit NativePannerNode(const Napi::CallbackInfo& info);

    private:
        static constexpr auto JS_CONSTRUCTOR_NAME = "PannerNode";

        void InitializeState(const std::shared_ptr<AudioContextState>& context);

        Napi::Value Connect(const Napi::CallbackInfo& info);
        Napi::Value Disconnect(const Napi::CallbackInfo& info);
        void SetPosition(const Napi::CallbackInfo& info);
        void SetOrientation(const Napi::CallbackInfo& info);

        Napi::Value GetPositionX(const Napi::CallbackInfo& info);
        Napi::Value GetPositionY(const Napi::CallbackInfo& info);
        Napi::Value GetPositionZ(const Napi::CallbackInfo& info);
        Napi::Value GetOrientationX(const Napi::CallbackInfo& info);
        Napi::Value GetOrientationY(const Napi::CallbackInfo& info);
        Napi::Value GetOrientationZ(const Napi::CallbackInfo& info);

        Napi::Value GetPanningModel(const Napi::CallbackInfo& info);
        void SetPanningModel(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDistanceModel(const Napi::CallbackInfo& info);
        void SetDistanceModel(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRefDistance(const Napi::CallbackInfo& info);
        void SetRefDistance(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetMaxDistance(const Napi::CallbackInfo& info);
        void SetMaxDistance(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetRolloffFactor(const Napi::CallbackInfo& info);
        void SetRolloffFactor(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetConeInnerAngle(const Napi::CallbackInfo& info);
        void SetConeInnerAngle(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetConeOuterAngle(const Napi::CallbackInfo& info);
        void SetConeOuterAngle(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetConeOuterGain(const Napi::CallbackInfo& info);
        void SetConeOuterGain(const Napi::CallbackInfo& info, const Napi::Value& value);

        std::shared_ptr<PannerNodeState> m_state{};
        Napi::ObjectReference m_positionX{};
        Napi::ObjectReference m_positionY{};
        Napi::ObjectReference m_positionZ{};
        Napi::ObjectReference m_orientationX{};
        Napi::ObjectReference m_orientationY{};
        Napi::ObjectReference m_orientationZ{};
    };
}
