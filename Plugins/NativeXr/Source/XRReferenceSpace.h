#pragma once

namespace Babylon
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

        static Napi::Value Request(const Napi::CallbackInfo& info)
        {
            auto env = info.Env();
            auto deferred = Napi::Promise::Deferred::New(env);
            try
            {
                const auto type = info[0].ToString().Utf8Value();
                if (type == XRReferenceSpaceType::VIEWER || type == XRReferenceSpaceType::LOCAL || type == XRReferenceSpaceType::UNBOUNDED)
                {
                    deferred.Resolve(env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({}));
                }
                else if (type == XRReferenceSpaceType::LOCAL_FLOOR || type == XRReferenceSpaceType::BOUNDED_FLOOR)
                {
                    // The current backends expose a world origin, not a calibrated floor or bounds.
                    const auto message = "Unsupported reference space: " + type;
                    auto ctor = env.Global().Get("DOMException");
                    if (ctor.IsFunction())
                    {
                        deferred.Reject(ctor.As<Napi::Function>().New({Napi::String::New(env, message), Napi::String::New(env, "NotSupportedError")}));
                    }
                    else
                    {
                        auto error = Napi::Error::New(env, message).Value();
                        error.Set("name", "NotSupportedError");
                        deferred.Reject(error);
                    }
                }
                else
                {
                    deferred.Reject(Napi::TypeError::New(env, "Unknown reference space type: " + type).Value());
                }
            }
            catch (const Napi::Error& error)
            {
                deferred.Reject(error.Value());
            }
            return deferred.Promise();
        }

        static Napi::Object New(const Napi::Env env, Napi::Object napiTransform)
        {
            return env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({napiTransform});
        }

        XRReferenceSpace(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRReferenceSpace>{info}
        {
            if (info.Length() > 0 && info[0].IsObject())
            {
                m_jsTransform = Napi::Persistent(info[0].As<Napi::Object>());
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
} // Babylon
