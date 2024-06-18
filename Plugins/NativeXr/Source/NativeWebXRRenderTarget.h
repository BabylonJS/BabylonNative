#pragma once

#include "XRWebGLLayer.h"
#include "XRSession.h"

namespace Babylon
{
    namespace
    {
        class NativeWebXRRenderTarget : public Napi::ObjectWrap<NativeWebXRRenderTarget>
        {
            static constexpr auto JS_CLASS_NAME = "NativeWebXRRenderTarget";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("initializeXRLayerAsync", &NativeWebXRRenderTarget::InitializeXRLayerAsync),
                        InstanceMethod("dispose", &NativeWebXRRenderTarget::Dispose),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
            }

            NativeWebXRRenderTarget(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<NativeWebXRRenderTarget>{info}
                , m_jsEngineReference{Napi::Persistent(info[0].As<Napi::Object>())}
            {
            }

        private:
            // Lifetime control to prevent the cleanup of the NativeEngine while XR is still alive.
            Napi::ObjectReference m_jsEngineReference{};

            Napi::Value InitializeXRLayerAsync(const Napi::CallbackInfo& info)
            {
                auto& session = *Plugins::XRSession::Unwrap(info[0].As<Napi::Object>());

                auto xrLayer = XRWebGLLayer::New(info);
                session.InitializeXrLayer(xrLayer);
                info.This().As<Napi::Object>().Set("xrLayer", xrLayer);

                auto deferred = Napi::Promise::Deferred::New(info.Env());
                deferred.Resolve(info.Env().Undefined());
                return deferred.Promise();
            }

            Napi::Value Dispose(const Napi::CallbackInfo& info)
            {
                return info.Env().Undefined();
            }
        };
    } // Plugins
} // Babylon
