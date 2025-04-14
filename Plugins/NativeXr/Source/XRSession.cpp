#include <Babylon/Plugins/NativeXr.h>

#include <Babylon/JsRuntimeScheduler.h>

#include <XR.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>

#include "Constants.h"
#include "PointerEvent.h"
#include "XRWebGLBinding.h"
#include "XRWebGLLayer.h"
#include "XRRigidTransform.h"
#include "XRView.h"
#include "XRViewerPose.h"
#include "XRPose.h"
#include "XRRay.h"
#include "XRReferenceSpace.h"
#include "XRAnchor.h"
#include "XRHitTestSource.h"
#include "XRHitTestResult.h"
#include "XRPlane.h"
#include "XRMesh.h"
#include "XRHand.h"
#include "NativeXrImpl.h"
#include "XRSession.h"
#include "XRFrame.h"

namespace Babylon
{
    namespace
    {
        void SetXRGamepadObjectData(Napi::Object& jsInputSource, Napi::Object& jsGamepadObject, xr::System::Session::Frame::InputSource& inputSource)
        {
            auto env = jsInputSource.Env();
            //Set Gamepad Object
            auto gamepadButtons = Napi::Array::New(env, inputSource.GamepadObject.Buttons.size());
            for (size_t i = 0; i < inputSource.GamepadObject.Buttons.size(); i++)
            {
                auto gamepadButton = Napi::Object::New(env);
                auto napiGamepadPressed = Napi::Boolean::New(env, inputSource.GamepadObject.Buttons[i].Pressed);
                auto napiGamepadTouched = Napi::Boolean::New(env, inputSource.GamepadObject.Buttons[i].Touched);
                auto napiGamepadValue = Napi::Number::New(env, inputSource.GamepadObject.Buttons[i].Value);
                gamepadButton.Set("pressed", napiGamepadPressed);
                gamepadButton.Set("touched", napiGamepadTouched);
                gamepadButton.Set("value", napiGamepadValue);
                gamepadButtons.Set(static_cast<int>(i), gamepadButton);
            }
            jsGamepadObject.Set("buttons", gamepadButtons);

            auto gamepadAxes = Napi::Array::New(env, inputSource.GamepadObject.Axes.size());
            for (size_t i = 0; i < inputSource.GamepadObject.Axes.size(); i++)
            {
                auto napiGamepadAxesValue = Napi::Number::New(env, inputSource.GamepadObject.Axes[i]);
                gamepadAxes.Set(static_cast<int>(i), napiGamepadAxesValue);
            }
            jsGamepadObject.Set("axes", gamepadAxes);
            jsInputSource.Set("gamepad", jsGamepadObject);
        }

        void CreateXRGamepadObject(Napi::Object& jsInputSource, xr::System::Session::Frame::InputSource& inputSource)
        {
            auto env = jsInputSource.Env();
            auto jsGamepadObject = Napi::Object::New(env);
            SetXRGamepadObjectData(jsInputSource, jsGamepadObject, inputSource);
        }

        void PopulateDetectionBoundary(const Napi::Object& object, xr::DetectionBoundary& detectionBoundary)
        {
            if (object.Has("type"))
            {
                const std::map<std::string, xr::DetectionBoundaryType> detectionBoundaryTypeMap{
                    {"box", xr::DetectionBoundaryType::Box},
                    {"frustum", xr::DetectionBoundaryType::Frustum},
                    {"sphere", xr::DetectionBoundaryType::Sphere} };
                detectionBoundary.Type = detectionBoundaryTypeMap.at(object.Get("type").As<Napi::String>());
            }

            switch (detectionBoundary.Type)
            {
            case xr::DetectionBoundaryType::Box:
                if (object.Has("extent"))
                {
                    const auto& vector = object.Get("extent").As<Napi::Object>();
                    xr::Vector3f boxDimensions{};
                    boxDimensions.X = vector.Get("x").As<Napi::Number>();
                    boxDimensions.Y = vector.Get("y").As<Napi::Number>();
                    boxDimensions.Z = vector.Get("z").As<Napi::Number>();
                    detectionBoundary.Data = boxDimensions;
                }
                break;
            case xr::DetectionBoundaryType::Frustum:
                if (object.Has("frustum"))
                {
                    const auto& frustum = object.Get("frustum").As<Napi::Object>();
                    xr::Frustum frustumData{};
                    frustumData.FarDistance = frustum.Get("farDistance").As<Napi::Number>();

                    const auto& vector = frustum.Get("position").As<Napi::Object>();
                    frustumData.Pose.Position.X = vector.Get("x").As<Napi::Number>();
                    frustumData.Pose.Position.Y = vector.Get("y").As<Napi::Number>();
                    frustumData.Pose.Position.Z = vector.Get("z").As<Napi::Number>();

                    const auto& quaternion = frustum.Get("orientation").As<Napi::Object>();
                    frustumData.Pose.Orientation.X = quaternion.Get("x").As<Napi::Number>();
                    frustumData.Pose.Orientation.Y = quaternion.Get("y").As<Napi::Number>();
                    frustumData.Pose.Orientation.Z = quaternion.Get("z").As<Napi::Number>();
                    frustumData.Pose.Orientation.W = quaternion.Get("w").As<Napi::Number>();

                    const auto& fov = frustum.Get("fieldOfView").As<Napi::Object>();
                    frustumData.FOV.AngleLeft = fov.Get("angleLeft").As<Napi::Number>();
                    frustumData.FOV.AngleRight = fov.Get("angleRight").As<Napi::Number>();
                    frustumData.FOV.AngleUp = fov.Get("angleUp").As<Napi::Number>();
                    frustumData.FOV.AngleDown = fov.Get("angleDown").As<Napi::Number>();

                    frustumData.FarDistance = frustum.Get("farDistance").As<Napi::Number>();

                    detectionBoundary.Data = frustumData;
                }
                break;
            case xr::DetectionBoundaryType::Sphere:
                if (object.Has("radius"))
                {
                    detectionBoundary.Data = object.Get("radius").As<Napi::Number>();
                }
                break;
            }
        }

        xr::GeometryDetectorOptions CreateDetectorOptions(const Napi::Object& object)
        {
            xr::GeometryDetectorOptions options{};
            if (object.Has("updateInterval"))
            {
                options.UpdateInterval = object.Get("updateInterval").As<Napi::Number>();
            }

            if (object.Has("detectionBoundary"))
            {
                const auto& detectionBoundary = object.Get("detectionBoundary").As<Napi::Object>();
                PopulateDetectionBoundary(detectionBoundary, options.DetectionBoundary);
            }

            return options;
        }

        void SetXRInputSourceData(Napi::Object& jsInputSource, xr::System::Session::Frame::InputSource& inputSource)
        {
            auto env = jsInputSource.Env();
            jsInputSource.Set("targetRaySpace", Napi::External<xr::Space>::New(env, &inputSource.AimSpace));
            jsInputSource.Set("gripSpace", Napi::External<xr::Space>::New(env, &inputSource.GripSpace));

            // Don't set hands up unless hand data is supported/available
            if (inputSource.HandTrackedThisFrame || inputSource.JointsTrackedThisFrame)
            {
                auto profiles = Napi::Array::New(env, 2);
                profiles.Set(uint32_t{ 0 }, Napi::String::New(env, "generic-hand-select-grasp"));
                profiles.Set(uint32_t{ 1 }, Napi::String::New(env, "generic-hand-select"));
                jsInputSource.Set("profiles", profiles);

                if (inputSource.JointsTrackedThisFrame)
                {
                    const auto shouldInitHand = !jsInputSource.Has("hand");
                    auto handJointCollection = shouldInitHand ? Napi::Array::New(env, HAND_JOINT_NAMES.size()) : jsInputSource.Get("hand").As<Napi::Array>();
                    if (shouldInitHand)
                    {
                        auto jointGetter = [handJointCollection](const Napi::CallbackInfo& info) -> Napi::Value {
                            return handJointCollection.Get(info[0].As<Napi::String>());
                        };

                        handJointCollection.Set("get", Napi::Function::New(env, jointGetter, "get"));
                        handJointCollection.Set("size", static_cast<int>(HAND_JOINT_NAMES.size()));

                        jsInputSource.Set("hand", handJointCollection);
                    }

                    for (size_t i = 0; i < HAND_JOINT_NAMES.size(); i++)
                    {
                        auto napiJoint = Napi::External<xr::System::Session::Frame::JointSpace>::New(env, &inputSource.HandJoints[i]);
                        handJointCollection.Set(HAND_JOINT_NAMES[i], napiJoint);
                    }
                }
                else
                {
                    // If hand joints aren't available on a hand input, send a null hand object
                    jsInputSource.Set("hand", env.Null());
                }
            }
        }

        Napi::ObjectReference CreateXRInputSource(xr::System::Session::Frame::InputSource& inputSource, Napi::Env& env)
        {
            constexpr std::array<const char*, 2> HANDEDNESS_STRINGS{
                "left",
                "right" };
            constexpr const char* TARGET_RAY_MODE{ "tracked-pointer" };

            auto jsInputSource = Napi::Object::New(env);

            jsInputSource.Set("handedness", Napi::String::New(env, HANDEDNESS_STRINGS[static_cast<size_t>(inputSource.Handedness)]));
            jsInputSource.Set("targetRayMode", TARGET_RAY_MODE);

            auto profiles = Napi::Array::New(env, 1);
            Napi::Value string = Napi::String::New(env, "generic-trigger-squeeze-touchpad-thumbstick");
            profiles.Set(uint32_t{ 0 }, string);
            jsInputSource.Set("profiles", profiles);

            SetXRInputSourceData(jsInputSource, inputSource);

            return Napi::Persistent(jsInputSource);
        }
    }

    // Using Plugins namespace warning C5046: 'Babylon::`anonymous-namespace'::XRFrame::xxx': Symbol involving type with internal linkage not defined
    namespace Plugins
    {
        void XRSession::Initialize(Napi::Env env)
        {
            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceAccessor("inputSources", &XRSession::GetInputSources, nullptr),
                    InstanceMethod("addEventListener", &XRSession::AddEventListener),
                    InstanceMethod("removeEventListener", &XRSession::RemoveEventListener),
                    InstanceMethod("requestReferenceSpace", &XRSession::RequestReferenceSpace),
                    InstanceMethod("updateRenderState", &XRSession::UpdateRenderState),
                    InstanceMethod("requestAnimationFrame", &XRSession::RequestAnimationFrame),
                    InstanceMethod("end", &XRSession::End),
                    InstanceMethod("requestHitTestSource", &XRSession::RequestHitTestSource),
                    InstanceMethod("updateWorldTrackingState", &XRSession::UpdateWorldTrackingState),
                    InstanceMethod("trySetFeaturePointCloudEnabled", &XRSession::TrySetFeaturePointCloudEnabled),
                    InstanceMethod("trySetPreferredPlaneDetectorOptions", &XRSession::TrySetPreferredPlaneDetectorOptions),
                    InstanceMethod("trySetMeshDetectorEnabled", &XRSession::TrySetMeshDetectorEnabled),
                    InstanceMethod("trySetPreferredMeshDetectorOptions", &XRSession::TrySetPreferredMeshDetectorOptions),
                    InstanceMethod("getTrackedImageScores", &XRSession::GetTrackedImageScores),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        Napi::Promise XRSession::CreateAsync(const Napi::CallbackInfo& info, std::shared_ptr<Plugins::NativeXr::Impl> nativeXr)
        {
            auto jsSession{ Napi::Persistent(info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]})) };
            auto& session{ *XRSession::Unwrap(jsSession.Value()) };
            session.m_xr = std::move(nativeXr);

            auto featureObject = info[1].As<Napi::Object>();
            if (featureObject.Has("trackedImages"))
            {
                const auto napiTrackedImages{ featureObject.Get("trackedImages").As<Napi::Array>() };
                session.m_imageTrackingRequests.resize(napiTrackedImages.Length());

                // Create the tracked image buffer.
                for (uint32_t idx = 0; idx < napiTrackedImages.Length(); idx++)
                {
                    // Pull out native values from the JS object.
                    const auto napiImageRequest{ napiTrackedImages.Get(idx).As<Napi::Object>() };
                    const auto napiImage{ napiImageRequest.Get("image").As<Napi::Object>() };
                    const auto napiBuffer{ napiImage.Get("data").As<Napi::Uint8Array>() };
                    const uint32_t bufferSize{ (uint32_t)napiBuffer.ByteLength() };
                    const uint32_t imageHeight{ napiImage.Get("height").ToNumber().Uint32Value() };
                    const uint32_t imageWidth{ napiImage.Get("width").ToNumber().Uint32Value() };
                    const uint32_t imageDepth{ napiImage.Get("depth").ToNumber().Uint32Value() };
                    const uint32_t stride{ bufferSize / imageHeight };
                    const float estimatedWidth{ napiImageRequest.Get("widthInMeters").ToNumber().FloatValue() };

                    // Construct the image tracking request object.
                    session.m_imageTrackingRequests[idx] =
                    {
                        napiBuffer.Data(),
                        imageWidth,
                        imageHeight,
                        imageDepth,
                        stride,
                        estimatedWidth };
                }
            }

            auto deferred{ Napi::Promise::Deferred::New(info.Env()) };
            session.m_xr->BeginSessionAsync()
                .then(session.m_runtimeScheduler, arcana::cancellation::none(),
                    [deferred, jsSession{ std::move(jsSession) }, env{ info.Env() }](const arcana::expected<void, std::exception_ptr>& result) {
                if (result.has_error())
                {
                    deferred.Reject(Napi::Error::New(env, result.error()).Value());
                }
                else
                {
                    deferred.Resolve(jsSession.Value());
                }
            });
            return deferred.Promise();
        }

        XRSession::XRSession(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRSession>{ info }
            , m_runtimeScheduler{ JsRuntime::GetFromJavaScript(info.Env()) }
            , m_jsXRFrame{ Napi::Persistent(Plugins::XRFrame::New(info)) }
            , m_xrFrame{ *Plugins::XRFrame::Unwrap(m_jsXRFrame.Get("_nativeImpl").As<Napi::Object>()) }
            , m_jsInputSources{ Napi::Persistent(Napi::Array::New(info.Env())) }
        {
            // Currently only immersive VR and immersive AR are supported.
            assert(info[0].As<Napi::String>().Utf8Value() == XRSessionType::IMMERSIVE_VR ||
                info[0].As<Napi::String>().Utf8Value() == XRSessionType::IMMERSIVE_AR);
        }

        void XRSession::InitializeXrLayer(Napi::Object layer)
        {
            // NOTE: We currently only support rendering to the entire frame. Because the following values
            // are only used in the context of each other, width and hight as used here don't need to have
            // anything to do with actual pixel widths. This behavior is permitted by the draft WebXR spec,
            // which states that the, "exact interpretation of the viewport values depends on the conventions
            // of the graphics API the viewport is associated with." Since Babylon.js is here doing the
            // the interpretation for our graphics API, we are able to provide Babylon.js with simple values
            // that will communicate the correct behavior. In theory, for partial texture rendering, the
            // only part of this that will need to be fixed is the viewport (the layer will need one for
            // each view, not just the one that currently exists).
            // Spec reference: https://immersive-web.github.io/webxr/#dom-xrviewport-width
            constexpr size_t WIDTH = 1;
            constexpr size_t HEIGHT = 1;

            auto env = layer.Env();
            auto viewport = Napi::Object::New(env);
            viewport.Set("x", Napi::Value::From(env, 0));
            viewport.Set("y", Napi::Value::From(env, 0));
            viewport.Set("width", Napi::Value::From(env, WIDTH));
            viewport.Set("height", Napi::Value::From(env, HEIGHT));
            layer.Set("viewport", viewport);

            layer.Set("framebufferWidth", Napi::Value::From(env, WIDTH));
            layer.Set("framebufferHeight", Napi::Value::From(env, HEIGHT));
        }

        Napi::Value XRSession::GetRenderTargetForEye(const std::string& eye) const
        {
            return m_xr->GetRenderTargetForViewIndex(XREye::EyeToIndex(eye));
        }

        void XRSession::SetRenderTextureFunctions(const Napi::Function& createFunction, const Napi::Function& destroyFunction)
        {
            return m_xr->SetRenderTextureFunctions(createFunction, destroyFunction);
        }

        Napi::Value XRSession::DeclareNativeAnchor(const Napi::Env& env, xr::NativeAnchorPtr nativeAnchor)
        {
            return m_xrFrame.DeclareNativeAnchor(env, nativeAnchor);
        }

        Napi::Value XRSession::UpdateRenderState(const Napi::CallbackInfo& info)
        {
            auto renderState = info[0].As<Napi::Object>();
            info.This().As<Napi::Object>().Set("renderState", renderState);

            float depthNear = renderState.Get("depthNear").As<Napi::Number>().FloatValue();
            float depthFar = renderState.Get("depthFar").As<Napi::Number>().FloatValue();
            m_xr->SetDepthsNarFar(depthNear, depthFar);

            auto deferred = Napi::Promise::Deferred::New(info.Env());
            deferred.Resolve(info.Env().Undefined());
            return deferred.Promise();
        }

        void XRSession::ProcessEyeInputSource(const xr::System::Session::Frame& frame, Napi::Env env)
        {
            if (frame.EyeTrackerSpace.has_value() && m_jsEyeTrackedSource.IsEmpty())
            {
                m_jsEyeTrackedSource = Napi::Persistent(Napi::Object::New(env));
                m_jsEyeTrackedSource.Set("gazeSpace", Napi::External<xr::Space>::New(env, &frame.EyeTrackerSpace.value()));

                for (const auto& [name, callback] : m_eventNamesAndCallbacks)
                {
                    if (name == JS_EVENT_NAME_EYE_TRACKING_START)
                    {
                        Napi::Object obj = m_jsEyeTrackedSource.Value();
                        callback.Call({ obj });
                    }
                }
            }
            else if (!frame.EyeTrackerSpace.has_value() && !m_jsEyeTrackedSource.IsEmpty())
            {
                for (const auto& [name, callback] : m_eventNamesAndCallbacks)
                {
                    if (name == JS_EVENT_NAME_EYE_TRACKING_END)
                    {
                        callback.Call({});
                    }
                }

                m_jsEyeTrackedSource.Reset();
            }
        }

        void XRSession::ProcessControllerInputSources(const xr::System::Session::Frame& frame, Napi::Env env)
        {
            // Figure out the new state.
            std::set<xr::System::Session::Frame::InputSource::Identifier> added{};
            std::set<xr::System::Session::Frame::InputSource::Identifier> current{};
            std::set<xr::System::Session::Frame::InputSource::Identifier> removed{};

            std::vector<xr::System::Session::Frame::InputSource::Identifier> selectStarts{};
            std::vector<xr::System::Session::Frame::InputSource::Identifier> selectEnds{};
            std::vector<xr::System::Session::Frame::InputSource::Identifier> squeezeStarts{};
            std::vector<xr::System::Session::Frame::InputSource::Identifier> squeezeEnds{};

            // Process the controller-based input sources
            for (auto& inputSource : frame.InputSources)
            {
                if (!inputSource.TrackedThisFrame)
                {
                    continue;
                }

                current.insert(inputSource.ID);

                auto inputSourceFound = m_idToInputSource.find(inputSource.ID);
                if (inputSourceFound == m_idToInputSource.end())
                {
                    // Create the new input source, which will have the correct spaces associated with it.
                    m_idToInputSource.insert({ inputSource.ID, CreateXRInputSource(inputSource, env) });

                    // Now that input Source is created, create a gamepad object if enabled for the input source. Hand data also uses the gamepad component.
                    inputSourceFound = m_idToInputSource.find(inputSource.ID);
                    if (inputSource.GamepadTrackedThisFrame || inputSource.HandTrackedThisFrame)
                    {
                        auto inputSourceVal = inputSourceFound->second.Value();
                        CreateXRGamepadObject(inputSourceVal, inputSource);
                    }

                    added.insert(inputSource.ID);
                }
                else
                {
                    // Ensure the correct spaces are associated with the existing input source.
                    auto inputSourceVal = inputSourceFound->second.Value();
                    SetXRInputSourceData(inputSourceVal, inputSource);

                    // inputSource already exists, find the corresponding gamepad object if enabled and set to correct values
                    if (inputSourceVal.Has("gamepad"))
                    {
                        auto gamepadObject = inputSourceVal.Get("gamepad").As<Napi::Object>();
                        SetXRGamepadObjectData(inputSourceVal, gamepadObject, inputSource);
                    }
                }

                // Handle gestures. Sources that do not support a gesture will always return false
                const bool isSelected = inputSource.GamepadObject.Buttons[0].Pressed;
                const bool isSqueezed = inputSource.GamepadObject.Buttons[1].Pressed;
                const bool wasSelected = std::find(m_activeSelects.begin(), m_activeSelects.end(), inputSource.ID) != m_activeSelects.end();
                const bool wasSqueezed = std::find(m_activeSqueezes.begin(), m_activeSqueezes.end(), inputSource.ID) != m_activeSqueezes.end();

                if (isSelected && !wasSelected)
                {
                    selectStarts.push_back(inputSource.ID);
                }
                else if (!isSelected && wasSelected)
                {
                    selectEnds.push_back(inputSource.ID);
                }

                if (isSqueezed && !wasSqueezed)
                {
                    squeezeStarts.push_back(inputSource.ID);
                }
                else if (!isSqueezed && wasSqueezed)
                {
                    squeezeEnds.push_back(inputSource.ID);
                }
            }
            for (const auto& [id, ref] : m_idToInputSource)
            {
                if (current.find(id) == current.end())
                {
                    // Process select and squeeze for lost sources before we send the source lost event
                    auto inputSourceVal = ref.Value();
                    Napi::Object inputSourceEvent = GenerateXRInputSourceEvent(inputSourceVal, env);
                    const auto& activeSelectIter = std::find(m_activeSelects.begin(), m_activeSelects.end(), id);
                    const auto& activeSqueezeIter = std::find(m_activeSqueezes.begin(), m_activeSqueezes.end(), id);

                    if (activeSelectIter != m_activeSelects.end())
                    {
                        FireInputSourceEvent(inputSourceEvent, JS_EVENT_NAME_SELECT_END);
                        m_activeSelects.erase(activeSelectIter);
                    }

                    if (activeSqueezeIter != m_activeSqueezes.end())
                    {
                        FireInputSourceEvent(inputSourceEvent, JS_EVENT_NAME_SQUEEZE_END);
                        m_activeSqueezes.erase(activeSqueezeIter);
                    }
                    // Do not update space association since said spaces no longer exist.
                    removed.insert(id);
                }
            }

            // Only need to do more if there's been a change. Note that this block of code assumes
            // that ALL known input sources -- including ones added AND REMOVED this frame -- are
            // currently up-to-date and accessible though m_idToInputSource.
            if (added.size() > 0 || removed.size() > 0)
            {
                // Update the input sources array.
                auto jsCurrent = Napi::Array::New(env);
                for (const auto id : current)
                {
                    jsCurrent.Set(jsCurrent.Length(), m_idToInputSource[id].Value());
                }
                m_jsInputSources = Napi::Persistent(jsCurrent);

                // Create and send the sources changed event.
                Napi::Array jsAdded = Napi::Array::New(env);
                for (const auto id : added)
                {
                    jsAdded.Set(jsAdded.Length(), m_idToInputSource[id].Value());
                }
                Napi::Array jsRemoved = Napi::Array::New(env);
                for (const auto id : removed)
                {
                    jsRemoved.Set(jsRemoved.Length(), m_idToInputSource[id].Value());
                }
                auto sourcesChangeEvent = Napi::Object::New(env);
                sourcesChangeEvent.Set("added", jsAdded);
                sourcesChangeEvent.Set("removed", jsRemoved);
                for (const auto& [name, callback] : m_eventNamesAndCallbacks)
                {
                    if (name == JS_EVENT_NAME_INPUT_SOURCES_CHANGE)
                    {
                        callback.Call({ sourcesChangeEvent });
                    }
                }

                // Finally, remove the removed.
                for (const auto id : removed)
                {
                    m_idToInputSource.erase(id);
                }
            }

            // Process active selects after firing off any new source added events
            UpdateInputSourceEventValues(selectStarts, selectEnds, squeezeStarts, squeezeEnds, env);
        }

        Napi::Value XRSession::RequestAnimationFrame(const Napi::CallbackInfo& info)
        {
            Napi::Function callback{ info[0].As<Napi::Function>() };

            m_xr->ScheduleFrame([this, callbackPtr{ std::make_shared<Napi::FunctionReference>(Napi::Persistent(callback)) }](const std::shared_ptr<const xr::System::Session::Frame>& frame) {
                ProcessEyeInputSource(*frame.get(), Env());
                ProcessControllerInputSources(*frame.get(), Env());

                m_xrFrame.Update(Env(), frame, m_timestamp);

                if (m_imageTrackingRequests.size() > 0)
                {
                    // Kick off creation of the augmented image database.
                    m_xr->CreateAugmentedImageDatabase(m_imageTrackingRequests);

                    // Clean up image tracking requests
                    m_imageTrackingRequests.clear();
                }

                callbackPtr->Value().Call({ Napi::Value::From(Env(), m_timestamp), m_jsXRFrame.Value() });
            });

            // The return value should be a request ID to allow for requesting cancellation, this is unused in Babylon.js currently.
            // For now just pass our "timestamp" as that uniquely identifies the frame.
            return Napi::Value::From(info.Env(), m_timestamp++);
        }

        void XRSession::UpdateWorldTrackingState(const Napi::CallbackInfo& info)
        {
            auto optionsObj = info[0].As<Napi::Object>();
            if (optionsObj.Has("planeDetectionState"))
            {
                bool planeDetectionEnabled = optionsObj.Get("planeDetectionState").As<Napi::Object>().Get("enabled").ToBoolean();
                m_xr->SetPlaneDetectionEnabled(planeDetectionEnabled);
            }
        }

        Napi::Object XRSession::GenerateXRInputSourceEvent(Napi::Object& inputSource, Napi::Env env)
        {
            auto inputSourceEvent = Napi::Object::New(env);
            inputSourceEvent.Set("frame", m_jsXRFrame.Value());
            inputSourceEvent.Set("inputSource", inputSource);

            return inputSourceEvent;
        }

        void XRSession::FireInputSourceEvent(Napi::Object& inputSourceEvent, std::string eventName)
        {
            for (const auto& [name, callback] : m_eventNamesAndCallbacks)
            {
                if (name == eventName)
                {
                    callback.Call({ inputSourceEvent });
                }
            }
        }

        void XRSession::UpdateInputSourceEventValues(
            const std::vector<xr::System::Session::Frame::InputSource::Identifier>& selectStarts,
            const std::vector<xr::System::Session::Frame::InputSource::Identifier>& selectEnds,
            const std::vector<xr::System::Session::Frame::InputSource::Identifier>& squeezeStarts,
            const std::vector<xr::System::Session::Frame::InputSource::Identifier>& squeezeEnds,
            Napi::Env env)
        {
            for (const auto& id : selectStarts)
            {
                auto inputSourceIter = m_idToInputSource.find(id);
                if (inputSourceIter != m_idToInputSource.end())
                {
                    auto inputSourceVal = inputSourceIter->second.Value();
                    Napi::Object inputSourceEvent = GenerateXRInputSourceEvent(inputSourceVal, env);
                    FireInputSourceEvent(inputSourceEvent, JS_EVENT_NAME_SELECT_START);

                    m_activeSelects.push_back(id);
                }
            }

            for (const auto& id : squeezeStarts)
            {
                auto inputSourceIter = m_idToInputSource.find(id);
                if (inputSourceIter != m_idToInputSource.end())
                {
                    auto inputSourceVal = inputSourceIter->second.Value();
                    Napi::Object inputSourceEvent = GenerateXRInputSourceEvent(inputSourceVal, env);
                    FireInputSourceEvent(inputSourceEvent, JS_EVENT_NAME_SQUEEZE_START);

                    m_activeSqueezes.push_back(id);
                }
            }

            for (const auto& id : selectEnds)
            {
                auto inputSourceIter = m_idToInputSource.find(id);
                if (inputSourceIter != m_idToInputSource.end())
                {
                    auto inputSourceVal = inputSourceIter->second.Value();
                    Napi::Object inputSourceEvent = GenerateXRInputSourceEvent(inputSourceVal, env);

                    // WebXR API dictates the select event fires before the select end
                    FireInputSourceEvent(inputSourceEvent, JS_EVENT_NAME_SELECT);
                    FireInputSourceEvent(inputSourceEvent, JS_EVENT_NAME_SELECT_END);

                    m_activeSelects.erase(std::find(m_activeSelects.begin(), m_activeSelects.end(), id));
                }
            }

            for (const auto& id : squeezeEnds)
            {
                auto inputSourceIter = m_idToInputSource.find(id);
                if (inputSourceIter != m_idToInputSource.end())
                {
                    auto inputSourceVal = inputSourceIter->second.Value();
                    Napi::Object inputSourceEvent = GenerateXRInputSourceEvent(inputSourceVal, env);

                    // WebXR API dictates the squeeze event fires before the squeeze end
                    FireInputSourceEvent(inputSourceEvent, JS_EVENT_NAME_SQUEEZE);
                    FireInputSourceEvent(inputSourceEvent, JS_EVENT_NAME_SQUEEZE_END);

                    m_activeSqueezes.erase(std::find(m_activeSqueezes.begin(), m_activeSqueezes.end(), id));
                }
            }
        }

        Napi::Value XRSession::TrySetFeaturePointCloudEnabled(const Napi::CallbackInfo& info)
        {
            bool featurePointCloudEnabled = info[0].ToBoolean();
            bool enabled = m_xr->TrySetFeaturePointCloudEnabled(featurePointCloudEnabled);

            return Napi::Value::From(info.Env(), enabled);
        }

        Napi::Value XRSession::End(const Napi::CallbackInfo& info)
        {
            auto deferred{ Napi::Promise::Deferred::New(info.Env()) };
            m_xr->EndSessionAsync().then(m_runtimeScheduler, arcana::cancellation::none(),
                [this, deferred](const arcana::expected<void, std::exception_ptr>& result) {
                    if (result.has_error())
                    {
                        deferred.Reject(Napi::Error::New(Env(), result.error()).Value());
                        return;
                    }

                    for (const auto& [name, callback] : m_eventNamesAndCallbacks)
                    {
                        if (name == JS_EVENT_NAME_END)
                        {
                            callback.Call({});
                        }
                    }

                    deferred.Resolve(Env().Undefined());
                });

            return deferred.Promise();
        }

        Napi::Value XRSession::RequestHitTestSource(const Napi::CallbackInfo& info)
        {
            auto deferred = Napi::Promise::Deferred::New(info.Env());
            deferred.Resolve(XRHitTestSource::New(info));
            return deferred.Promise();
        }

        Napi::Value XRSession::TrySetPreferredPlaneDetectorOptions(const Napi::CallbackInfo& info)
        {
            if (info.Length() != 1 ||
                !info[0].IsObject())
            {
                throw std::exception(/*invalid arguments*/);
            }

            const auto options = CreateDetectorOptions(info[0].As<Napi::Object>());
            const auto result = m_xr->TrySetPreferredPlaneDetectorOptions(options);
            return Napi::Value::From(info.Env(), result);
        }

        Napi::Value XRSession::TrySetMeshDetectorEnabled(const Napi::CallbackInfo& info)
        {
            if (info.Length() != 1 ||
                !info[0].IsBoolean())
            {
                throw std::exception(/*invalid arguments*/);
            }

            const auto enabled = info[0].As<Napi::Boolean>();
            const auto result = m_xr->TrySetMeshDetectorEnabled(enabled);
            return Napi::Value::From(info.Env(), result);
        }

        Napi::Value XRSession::TrySetPreferredMeshDetectorOptions(const Napi::CallbackInfo& info)
        {
            if (info.Length() != 1 ||
                !info[0].IsObject())
            {
                throw std::exception(/*invalid arguments*/);
            }

            const auto options = CreateDetectorOptions(info[0].As<Napi::Object>());
            const auto result = m_xr->TrySetPreferredMeshDetectorOptions(options);
            return Napi::Value::From(info.Env(), result);
        }

        Napi::Value XRSession::GetTrackedImageScores(const Napi::CallbackInfo& info)
        {
            std::vector<xr::ImageTrackingScore>* imageTrackingScores{ m_xr->GetImageTrackingScores() };
            if (imageTrackingScores == nullptr)
            {
                return info.Env().Undefined();
            }

            auto results{ Napi::Array::New(info.Env(), imageTrackingScores->size()) };
            uint32_t index{ 0 };

            // Loop over the list of tracked image tracking results, and add them to the array.
            for (const auto& score : *imageTrackingScores)
            {
                const std::string scoreString = score == xr::ImageTrackingScore::TRACKABLE ? XRImageTrackingScore::TRACKABLE : XRImageTrackingScore::UNTRACKABLE;
                results.Set(index++, Napi::Value::From(info.Env(), scoreString));
            }

            return results;
        }
     } // Plugins
} // Babylon