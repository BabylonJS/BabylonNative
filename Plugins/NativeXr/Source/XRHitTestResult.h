#pragma once

#include "XRPose.h"
#include "XRFrame.h"

namespace Babylon
{
    namespace Plugins
    {
        // Implementation of the XRHitTestResult interface: https://immersive-web.github.io/hit-test/#xr-hit-test-result-interface
        class XRHitTestResult : public Napi::ObjectWrap<XRHitTestResult>
        {
            static constexpr auto JS_CLASS_NAME = "XRHitTestResult";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getPose", &XRHitTestResult::GetPose),
                        InstanceMethod("createAnchor", &XRHitTestResult::CreateAnchor),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRHitTestResult(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRHitTestResult>{info}
            {
            }

            // Sets the value of the hit pose and native entity via struct copy.
            void SetHitResult(const xr::HitResult& hitResult)
            {
                m_hitResult = hitResult;
            }

            void SetXRFrame(Plugins::XRFrame* frame)
            {
                m_frame = frame;
            }

        private:
            // The hit hit result, which contains the pose in default AR Space, as well as the native entity.
            xr::HitResult m_hitResult{};
            Plugins::XRFrame* m_frame{};

            Napi::Value GetPose(const Napi::CallbackInfo& info)
            {
                // TODO: Once multiple reference views are supported, we need to convert the values into the passed in reference space.
                Napi::Object napiPose = XRPose::New(info);
                XRPose* pose = XRPose::Unwrap(napiPose);
                pose->Update(info, m_hitResult.Pose);

                return napiPose;
            }

            Napi::Value CreateAnchor(const Napi::CallbackInfo& info)
            {
                return m_frame->CreateNativeAnchor(info, m_hitResult.Pose, m_hitResult.NativeTrackable);
            }
        };
    } // Plugins
} // Babylon
