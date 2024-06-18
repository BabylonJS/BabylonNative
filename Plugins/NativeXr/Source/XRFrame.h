#pragma once
namespace Babylon
{
    namespace Plugins
    {
        class XRFrame : public Napi::ObjectWrap<XRFrame>
        {
            static constexpr auto JS_CLASS_NAME = "XRFrame";

        public:
            static void Initialize(Napi::Env env);
            static Napi::Object New(const Napi::CallbackInfo& info);

            XRFrame(const Napi::CallbackInfo& info);
            void Update(const Napi::Env& env, const xr::System::Session::Frame& frame, uint32_t timestamp);
            Napi::Promise CreateNativeAnchor(const Napi::CallbackInfo& info, xr::Pose pose, xr::NativeTrackablePtr nativeTrackable);
            Napi::Value DeclareNativeAnchor(const Napi::Env& env, xr::NativeAnchorPtr nativeAnchor);

            xr::System::Session::Frame::Plane& GetPlaneFromID(xr::System::Session::Frame::Plane::Identifier planeID)
            {
                return m_frame->GetPlaneByID(planeID);
            }

            xr::System::Session::Frame::Mesh& GetMeshFromID(xr::System::Session::Frame::Mesh::Identifier meshID)
            {
                return m_frame->GetMeshByID(meshID);
            }

            Napi::Value GetJSSceneObjectFromID(const Napi::CallbackInfo& info, const xr::System::Session::Frame::SceneObject::Identifier objectID);

        private:
            const xr::System::Session::Frame* m_frame{};
            Napi::ObjectReference m_jsXRViewerPose{};
            XRViewerPose& m_xrViewerPose;
            std::vector<Napi::ObjectReference> m_trackedAnchors{};
            std::unordered_map<xr::System::Session::Frame::Plane::Identifier, Napi::ObjectReference> m_trackedPlanes{};
            std::unordered_map<xr::System::Session::Frame::Mesh::Identifier, Napi::ObjectReference> m_trackedMeshes{};
            std::unordered_map<xr::System::Session::Frame::ImageTrackingResult::Identifier, Napi::ObjectReference> m_trackedImageIDToResultMap{};
            Napi::Reference<Napi::Array> m_imageTrackingResultsArray{};
            Napi::ObjectReference m_meshSet{};
            std::unordered_map<xr::System::Session::Frame::SceneObject::Identifier, Napi::ObjectReference> m_sceneObjects{};

            Napi::ObjectReference m_jsTransform{};
            XRRigidTransform& m_transform;
            Napi::ObjectReference m_jsPose{};
            Napi::ObjectReference m_jsJointPose{};

            bool m_hasBegunTracking{ false };

            Napi::Value GetViewerPose(const Napi::CallbackInfo& info);
            Napi::Value GetPoseData(const Napi::CallbackInfo& info);
            Napi::Value GetJointPose(const Napi::CallbackInfo& info);
            Napi::Value FillPoses(const Napi::CallbackInfo& info);
            Napi::Value FillJointRadii(const Napi::CallbackInfo& info);

            // NativeXRFrame on the JS side expects getImageTrackingResults to be defined at XR initialization time.
            // This dummy implementation is a placeholder until WebXR Image Tracking support is completed: https://github.com/BabylonJS/BabylonNative/issues/619
            Napi::Value GetImageTrackingResults(const Napi::CallbackInfo& info)
            {
                return info.Env().Undefined();
            }

            Napi::Value GetHitTestResults(const Napi::CallbackInfo& info);
            Napi::Value CreateAnchor(const Napi::CallbackInfo& info);
            Napi::Value GetTrackedAnchors(const Napi::CallbackInfo& info);
            void UpdateAnchors();
            Napi::Value GetWorldInformation(const Napi::CallbackInfo& info);
            Napi::Value GetFeaturePointCloud(const Napi::CallbackInfo& info);
            void UpdateSceneObjects(const Napi::Env& env);
            void UpdatePlanes(const Napi::Env& env, uint32_t timestamp);
            void UpdateMeshes(const Napi::Env& env, uint32_t timestamp);
            void UpdateImageTrackingResults(const Napi::Env& env);
        };
    } // Plugins
} // Babylon
