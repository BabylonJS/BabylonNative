#pragma once

namespace Babylon
{
    namespace
    {
        class NativeRenderTargetProvider : public Napi::ObjectWrap<NativeRenderTargetProvider>
        {
            static constexpr auto JS_CLASS_NAME = "NativeRenderTargetProvider";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getRenderTargetForEye", &NativeRenderTargetProvider::GetRenderTargetForEye),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0], info[1], info[2]});
            }

            NativeRenderTargetProvider(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<NativeRenderTargetProvider>{info}
                , m_jsSession{Napi::Persistent(info[0].As<Napi::Object>())}
                , m_session{*Plugins::XRSession::Unwrap(m_jsSession.Value())}
            {
                auto createRenderTexture{info[1].As<Napi::Function>()};
                auto destroyRenderTexture{info[2].As<Napi::Function>()};
                m_session.SetRenderTextureFunctions(createRenderTexture, destroyRenderTexture);
            }

        private:
            Napi::ObjectReference m_jsSession{};
            Plugins::XRSession& m_session;

            Napi::Value GetRenderTargetForEye(const Napi::CallbackInfo& info)
            {
                const std::string eye{info[0].As<Napi::String>().Utf8Value()};
                return m_session.GetRenderTargetForEye(eye);
            }
        };
    }
} // Babylon
