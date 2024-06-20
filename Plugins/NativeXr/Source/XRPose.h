#pragma once

#include "XRRigidTransform.h"

namespace Babylon
{
    namespace
    {
        // Implementation of vanilla XRPose: https://immersive-web.github.io/webxr/#xrpose-interface
        class XRPose : public Napi::ObjectWrap<XRPose>
        {
            static constexpr auto JS_CLASS_NAME = "XRPose";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("transform", &XRPose::GetTransform, nullptr),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRPose(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRPose>{info}
                , m_jsTransform{Napi::Persistent(XRRigidTransform::New(info.Env()))}
                , m_transform{*XRRigidTransform::Unwrap(m_jsTransform.Value())}
            {
            }

            void Update(const Napi::CallbackInfo& /*info*/, const xr::Pose& pose)
            {
                // Update the transform.
                m_transform.Update(pose);
            }

            void Update(XRRigidTransform* transform)
            {
                // Update the transform.
                m_transform.Update(transform);
            }

        private:
            Napi::ObjectReference m_jsTransform{};
            XRRigidTransform& m_transform;

            Napi::Value GetTransform(const Napi::CallbackInfo& /*info*/)
            {
                return m_jsTransform.Value();
            }
        };
    }
} // Babylon
