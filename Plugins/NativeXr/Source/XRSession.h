#pragma once

namespace Babylon
{
    namespace Plugins
    {
        // Implementation of the XRSession interface: https://immersive-web.github.io/webxr/#xrsession-interface
        class XRSession : public Napi::ObjectWrap<XRSession>
        {
            static constexpr auto JS_CLASS_NAME = "XRSession";
            static constexpr auto JS_EVENT_NAME_END = "end";
            static constexpr auto JS_EVENT_NAME_INPUT_SOURCES_CHANGE = "inputsourceschange";
            static constexpr auto JS_EVENT_NAME_SELECT = "select";
            static constexpr auto JS_EVENT_NAME_SELECT_START = "selectstart";
            static constexpr auto JS_EVENT_NAME_SELECT_END = "selectend";
            static constexpr auto JS_EVENT_NAME_SQUEEZE = "squeeze";
            static constexpr auto JS_EVENT_NAME_SQUEEZE_START = "squeezestart";
            static constexpr auto JS_EVENT_NAME_SQUEEZE_END = "squeezeend";
            static constexpr auto JS_EVENT_NAME_EYE_TRACKING_START = "eyetrackingstart";
            static constexpr auto JS_EVENT_NAME_EYE_TRACKING_END = "eyetrackingend";
            struct XRImageTrackingScore
            {
                static constexpr auto UNTRACKABLE{ "untrackable" };
                static constexpr auto TRACKABLE{ "trackable" };
            };

        public:
            static void Initialize(Napi::Env env);
            static Napi::Promise CreateAsync(const Napi::CallbackInfo& info, std::shared_ptr<Plugins::NativeXr::Impl> nativeXr);

            XRSession(const Napi::CallbackInfo& info);
            void InitializeXrLayer(Napi::Object layer);

            Napi::Value GetRenderTargetForEye(const std::string& eye) const;
            void SetRenderTextureFunctions(const Napi::Function& createFunction, const Napi::Function& destroyFunction);
            Napi::Value DeclareNativeAnchor(const Napi::Env& env, xr::NativeAnchorPtr nativeAnchor);

        private:

            JsRuntimeScheduler m_runtimeScheduler;
            std::shared_ptr<Plugins::NativeXr::Impl> m_xr;
            Napi::ObjectReference m_jsXRFrame{};
            Plugins::XRFrame& m_xrFrame;
            uint32_t m_timestamp{ 0 };

            std::vector<std::pair<std::string, Napi::FunctionReference>> m_eventNamesAndCallbacks{};

            Napi::Reference<Napi::Array> m_jsInputSources{};
            std::map<xr::System::Session::Frame::InputSource::Identifier, Napi::ObjectReference> m_idToInputSource{};
            Napi::ObjectReference m_jsEyeTrackedSource{};
            std::vector<xr::System::Session::Frame::InputSource::Identifier> m_activeSelects{};
            std::vector<xr::System::Session::Frame::InputSource::Identifier> m_activeSqueezes{};

            std::vector<xr::System::Session::ImageTrackingRequest> m_imageTrackingRequests{};

            Napi::Value GetInputSources(const Napi::CallbackInfo& /*info*/)
            {
                return m_jsInputSources.Value();
            }

            void AddEventListener(const Napi::CallbackInfo& info)
            {
                m_eventNamesAndCallbacks.emplace_back(
                    info[0].As<Napi::String>().Utf8Value(),
                    Napi::Persistent(info[1].As<Napi::Function>()));
            }

            void RemoveEventListener(const Napi::CallbackInfo& info)
            {
                auto name = info[0].As<Napi::String>().Utf8Value();
                auto callback = info[1].As<Napi::Function>();
                m_eventNamesAndCallbacks.erase(
                    std::remove_if(
                        m_eventNamesAndCallbacks.begin(),
                        m_eventNamesAndCallbacks.end(),
                        [&name, &callback](const std::pair<std::string, Napi::FunctionReference>& listener) {
                            return listener.first == name && listener.second.Value() == callback;
                        }),
                    m_eventNamesAndCallbacks.end());
            }

            Napi::Value RequestReferenceSpace(const Napi::CallbackInfo& info)
            {
                auto deferred = Napi::Promise::Deferred::New(info.Env());
                deferred.Resolve(XRReferenceSpace::New(info));
                return deferred.Promise();
            }

            Napi::Value UpdateRenderState(const Napi::CallbackInfo& info);

            void ProcessEyeInputSource(const xr::System::Session::Frame& frame, Napi::Env env);
            void ProcessControllerInputSources(const xr::System::Session::Frame& frame, Napi::Env env);

            Napi::Value RequestAnimationFrame(const Napi::CallbackInfo& info);
            void UpdateWorldTrackingState(const Napi::CallbackInfo& info);
            Napi::Object GenerateXRInputSourceEvent(Napi::Object& inputSource, Napi::Env env);
            void FireInputSourceEvent(Napi::Object& inputSourceEvent, std::string eventName);
            void UpdateInputSourceEventValues(
                const std::vector<xr::System::Session::Frame::InputSource::Identifier>& selectStarts,
                const std::vector<xr::System::Session::Frame::InputSource::Identifier>& selectEnds,
                const std::vector<xr::System::Session::Frame::InputSource::Identifier>& squeezeStarts,
                const std::vector<xr::System::Session::Frame::InputSource::Identifier>& squeezeEnds,
                Napi::Env env);
            Napi::Value TrySetFeaturePointCloudEnabled(const Napi::CallbackInfo& info);
            Napi::Value End(const Napi::CallbackInfo& info);
            Napi::Value RequestHitTestSource(const Napi::CallbackInfo& info);
            Napi::Value TrySetPreferredPlaneDetectorOptions(const Napi::CallbackInfo& info);
            Napi::Value TrySetMeshDetectorEnabled(const Napi::CallbackInfo& info);
            Napi::Value TrySetPreferredMeshDetectorOptions(const Napi::CallbackInfo& info);
            Napi::Value GetTrackedImageScores(const Napi::CallbackInfo& info);
        };
    } // Plugins
} // Babylon
