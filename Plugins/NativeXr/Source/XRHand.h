#pragma once

namespace Babylon
{
    constexpr std::array<const char*, 25> HAND_JOINT_NAMES{
        "wrist",

        "thumb-metacarpal",
        "thumb-phalanx-proximal",
        "thumb-phalanx-distal",
        "thumb-tip",

        "index-finger-metacarpal",
        "index-finger-phalanx-proximal",
        "index-finger-phalanx-intermediate",
        "index-finger-phalanx-distal",
        "index-finger-tip",

        "middle-finger-metacarpal",
        "middle-finger-phalanx-proximal",
        "middle-finger-phalanx-intermediate",
        "middle-finger-phalanx-distal",
        "middle-finger-tip",

        "ring-finger-metacarpal",
        "ring-finger-phalanx-proximal",
        "ring-finger-phalanx-intermediate",
        "ring-finger-phalanx-distal",
        "ring-finger-tip",

        "pinky-finger-metacarpal",
        "pinky-finger-phalanx-proximal",
        "pinky-finger-phalanx-intermediate",
        "pinky-finger-phalanx-distal",
        "pinky-finger-tip" };

    class XRHand : public Napi::ObjectWrap<XRHand>
    {
        static constexpr auto JS_CLASS_NAME = "XRHand";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            std::vector<XRHand::PropertyDescriptor> initList{};
            initList.reserve(HAND_JOINT_NAMES.size() + 1);

            for (size_t idx = 0; idx < HAND_JOINT_NAMES.size(); idx++)
            {
                initList.push_back(StaticValue(HAND_JOINT_NAMES[idx], Napi::Value::From(env, idx)));
            }

            initList.push_back(StaticAccessor("size", &XRHand::GetSize, nullptr));

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                initList);

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
        }

        XRHand(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRHand>{info}
        {
        }

    private:
        static Napi::Value GetSize(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(info.Env(), HAND_JOINT_NAMES.size());
        }
    };
} // Babylon
