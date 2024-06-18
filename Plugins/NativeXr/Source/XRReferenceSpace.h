#pragma once

namespace Babylon
{
    namespace
    {
        // Implementation of the XRReferenceSpace interface: https://immersive-web.github.io/webxr/#xrreferencespace-interface
        class XRReferenceSpace : public Napi::ObjectWrap<XRReferenceSpace>
        {
            static constexpr auto JS_CLASS_NAME = "XRReferenceSpace";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getOffsetReferenceSpace", &XRReferenceSpace::GetOffsetReferenceSpace),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
            }

            static Napi::Object New(const Napi::Env env, Napi::Object napiTransform)
            {
                return env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({napiTransform});
            }

            XRReferenceSpace(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRReferenceSpace>{info}
            {
                if (info.Length() > 0)
                {
                    if (info[0].IsString())
                    {
                        // TODO: Actually support the different types of reference spaces.
                        const auto referenceSpaceType = info[0].As<Napi::String>().Utf8Value();
                        assert(referenceSpaceType == XRReferenceSpaceType::UNBOUNDED ||
                               referenceSpaceType == XRReferenceSpaceType::VIEWER);
                        (void)XRReferenceSpaceType::UNBOUNDED;
                        (void)XRReferenceSpaceType::VIEWER;
                    }
                    else
                    {
                        m_jsTransform = Napi::Persistent(info[0].As<Napi::Object>());
                    }
                }
            }

            void SetTransform(Napi::Object transformObj)
            {
                m_jsTransform = Napi::Persistent(transformObj);
            }

            XRRigidTransform* GetTransform()
            {
                return XRRigidTransform::Unwrap(m_jsTransform.Value());
            }

        private:
            Napi::Value GetOffsetReferenceSpace(const Napi::CallbackInfo& info)
            {
                // TODO: Handle XRBoundedReferenceSpace case
                // https://immersive-web.github.io/webxr/#dom-xrreferencespace-getoffsetreferencespace

                return XRReferenceSpace::New(info);
            }

            Napi::ObjectReference m_jsTransform{};
        };
    } // Plugins
} // Babylon
