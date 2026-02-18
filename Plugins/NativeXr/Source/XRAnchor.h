#pragma once

namespace Babylon
{
    // Implementation of the XRAnchor interface: https://immersive-web.github.io/anchors/#xr-anchor
    class XRAnchor : public Napi::ObjectWrap<XRAnchor>
    {
        static constexpr auto JS_CLASS_NAME = "XRAnchor";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("delete", &XRAnchor::Delete),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::Env env)
        {
            return env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
        }

        XRAnchor(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRAnchor>{info}
            , m_jsAnchorSpace{Napi::External<xr::Space>::New(info.Env(), &m_nativeAnchor.Space)}
        {
            auto jsThis = info.This().As<Napi::Object>();
            jsThis.Set("anchorSpace", m_jsAnchorSpace);
        }

        xr::Anchor& GetNativeAnchor()
        {
            return m_nativeAnchor;
        }

        void SetAnchor(xr::Anchor& nativeAnchor)
        {
            m_nativeAnchor = nativeAnchor;
        }

    private:
        // Marks the anchor as no longer valid, and should be deleted on the next pass.
        void Delete(const Napi::CallbackInfo&)
        {
            m_nativeAnchor.IsValid = false;
        }

        // The native anchor which holds the current position of the anchor, and the native ref to the anchor.
        xr::Anchor m_nativeAnchor{};
        Napi::External<xr::Space> m_jsAnchorSpace{};
    };
} // Babylon
