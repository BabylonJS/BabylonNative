#pragma once

namespace Babylon
{
    namespace
    {
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
    }
} // Babylon
