#pragma once

namespace Babylon
{
    namespace
    {
        // Implementation of the XR interface: https://immersive-web.github.io/webxr/#xr-interface
        class XR : public Napi::ObjectWrap<XR>
        {
            static constexpr auto JS_CLASS_NAME = "NativeXR";
            static constexpr auto JS_NAVIGATOR_NAME = "navigator";
            static constexpr auto JS_XR_NAME = "xr";
            static constexpr auto JS_NATIVE_NAME = "native";

        public:
            static void Initialize(Napi::Env env, std::shared_ptr<Plugins::NativeXr::Impl> nativeXr)
            {
                Napi::HandleScope scope{ env };

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("isSessionSupported", &XR::IsSessionSupported),
                        InstanceMethod("requestSession", &XR::RequestSession),
                        InstanceMethod("getWebXRRenderTarget", &XR::GetWebXRRenderTarget),
                        InstanceMethod("getNativeRenderTargetProvider", &XR::GetNativeRenderTargetProvider),
                        InstanceAccessor("nativeXrContext", &XR::GetNativeXrContext, nullptr),
                        InstanceAccessor("nativeXrContextType", &XR::GetNativeXrContextType, nullptr),
                        InstanceMethod("getNativeAnchor", &XR::GetNativeAnchor),
                        InstanceMethod("declareNativeAnchor", &XR::DeclareNativeAnchor),
                        InstanceValue(JS_NATIVE_NAME, Napi::Value::From(env, true)),
                    });

                Napi::Object global = env.Global();
                Napi::Object navigator;
                if (global.Has(JS_NAVIGATOR_NAME))
                {
                    navigator = global.Get(JS_NAVIGATOR_NAME).As<Napi::Object>();
                }
                else
                {
                    navigator = Napi::Object::New(env);
                    global.Set(JS_NAVIGATOR_NAME, navigator);
                }

                auto xr = func.New({});
                XR::Unwrap(xr)->m_xr = std::move(nativeXr);
                navigator.Set(JS_XR_NAME, xr);
            }

            XR(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XR>{ info }
                , m_runtimeScheduler{ JsRuntime::GetFromJavaScript(info.Env()) }
            {
            }

        private:
            JsRuntimeScheduler m_runtimeScheduler;
            std::shared_ptr<Plugins::NativeXr::Impl> m_xr{};

            Napi::Value IsSessionSupported(const Napi::CallbackInfo& info)
            {
                std::string sessionTypeString = info[0].As<Napi::String>().Utf8Value();
                xr::SessionType sessionType{ xr::SessionType::INVALID };

                if (sessionTypeString == XRSessionType::IMMERSIVE_VR)
                {
                    sessionType = xr::SessionType::IMMERSIVE_VR;
                }
                else if (sessionTypeString == XRSessionType::IMMERSIVE_AR)
                {
                    sessionType = xr::SessionType::IMMERSIVE_AR;
                }
                else if (sessionTypeString == XRSessionType::INLINE)
                {
                    sessionType = xr::SessionType::INLINE;
                }

                auto deferred = Napi::Promise::Deferred::New(info.Env());

                // Fire off the IsSessionSupported task.
                xr::System::IsSessionSupportedAsync(sessionType)
                    .then(m_runtimeScheduler,
                        arcana::cancellation::none(),
                        [deferred, env = info.Env()](bool result) {
                    deferred.Resolve(Napi::Boolean::New(env, result));
                });

                return deferred.Promise();
            }

            Napi::Value RequestSession(const Napi::CallbackInfo& info)
            {
                return Plugins::XRSession::CreateAsync(info, m_xr);
            }

            Napi::Value GetWebXRRenderTarget(const Napi::CallbackInfo& info)
            {
                return NativeWebXRRenderTarget::New(info);
            }

            Napi::Value GetNativeRenderTargetProvider(const Napi::CallbackInfo& info)
            {
                return NativeRenderTargetProvider::New(info);
            }

            Napi::Value GetNativeXrContext(const Napi::CallbackInfo& info)
            {
                const auto nativeExtension = m_xr->GetNativeXrContext();
                if (nativeExtension)
                {
                    return Napi::Number::From(info.Env(), nativeExtension);
                }

                return info.Env().Undefined();
            }

            Napi::Value GetNativeXrContextType(const Napi::CallbackInfo& info)
            {
                const auto nativeExtensionType = m_xr->GetNativeXrContextType();
                if (!nativeExtensionType.empty())
                {
                    return Napi::String::From(info.Env(), nativeExtensionType);
                }

                return info.Env().Undefined();
            }

            Napi::Value GetNativeAnchor(const Napi::CallbackInfo& info)
            {
                if (info.Length() != 1 ||
                    !info[0].IsObject())
                {
                    throw std::runtime_error{ "A single object argument is required." };
                }

                const auto xrAnchor{ XRAnchor::Unwrap(info[0].ToObject()) };
                const auto anchor{ xrAnchor->GetNativeAnchor() };
                if (anchor.NativeAnchor != nullptr)
                {
                    return Napi::Number::From(info.Env(), reinterpret_cast<uintptr_t>(anchor.NativeAnchor));
                }

                return info.Env().Undefined();
            }

            Napi::Value DeclareNativeAnchor(const Napi::CallbackInfo& info)
            {
                if (info.Length() != 2 ||
                    !info[0].IsObject() /*XRSession*/ ||
                    !info[1].IsNumber() /*NativeAnchorPtr*/)
                {
                    throw std::runtime_error{ "Invalid argument provided." };
                }

                const auto session = Plugins::XRSession::Unwrap(info[0].As<Napi::Object>());
                const auto anchorPtr = reinterpret_cast<void*>(static_cast<uintptr_t>(info[1].As<Napi::Number>().DoubleValue()));
                return session->DeclareNativeAnchor(info.Env(), anchorPtr);
            }
        };
    }
}