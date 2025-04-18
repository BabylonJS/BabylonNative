#include <Babylon/Plugins/NativeXr.h>

#include <Babylon/JsRuntimeScheduler.h>

#include <XR.h>

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

#include "XRFrame.h"

namespace Babylon
{
    // Using Plugins namespace warning C5046: 'Babylon::`anonymous-namespace'::XRFrame::xxx': Symbol involving type with internal linkage not defined
    namespace Plugins
    {
        void XRFrame::Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{ env };

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("getViewerPose", &XRFrame::GetViewerPose),
                    InstanceMethod("getPoseData", &XRFrame::GetPoseData),
                    InstanceMethod("getHitTestResults", &XRFrame::GetHitTestResults),
                    InstanceMethod("createAnchor", &XRFrame::CreateAnchor),
                    InstanceMethod("getJointPose", &XRFrame::GetJointPose),
                    InstanceMethod("fillPoses", &XRFrame::FillPoses),
                    InstanceMethod("fillJointRadii", &XRFrame::FillJointRadii),
                    InstanceMethod("getImageTrackingResults", &XRFrame::GetImageTrackingResults),
                    InstanceAccessor("trackedAnchors", &XRFrame::GetTrackedAnchors, nullptr),
                    InstanceAccessor("worldInformation", &XRFrame::GetWorldInformation, nullptr),
                    InstanceAccessor("featurePointCloud", &XRFrame::GetFeaturePointCloud, nullptr),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        Napi::Object XRFrame::New(const Napi::CallbackInfo& info)
        {
            // Instead of creating just a C++ XRFrame object, wrap it in a JS NativeXRFrame object (see nativeXRFrame.ts in Babylon.JS)
            const auto nativeImpl = info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            const auto nativeXRFrame = JsRuntime::NativeObject::GetFromJavaScript(info.Env()).Get("NativeXRFrame").As<Napi::Function>().New({ nativeImpl }).As<Napi::Object>();
            return nativeXRFrame;
        }

        XRFrame::XRFrame(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRFrame>{ info }
            , m_jsXRViewerPose{ Napi::Persistent(XRViewerPose::New(info)) }
            , m_xrViewerPose{ *XRViewerPose::Unwrap(m_jsXRViewerPose.Value()) }
            , m_imageTrackingResultsArray{ Napi::Persistent(Napi::Array::New(info.Env())) }
            , m_jsTransform{ Napi::Persistent(XRRigidTransform::New(info.Env())) }
            , m_transform{ *XRRigidTransform::Unwrap(m_jsTransform.Value()) }
            , m_jsPose{ Napi::Persistent(Napi::Object::New(info.Env())) }
            , m_jsJointPose{ Napi::Persistent(Napi::Object::New(info.Env())) }
        {
            info.This().As<Napi::Object>().Set("_imageTrackingResults", m_imageTrackingResultsArray.Value());
            m_jsPose.Set("transform", m_jsTransform.Value());
            m_jsJointPose.Set("transform", m_jsTransform.Value());
        }

        void XRFrame::Update(const Napi::Env& env, const std::shared_ptr<const xr::System::Session::Frame>& frame, uint32_t timestamp)
        {
            // Store off a pointer to the frame so that the viewer pose can be updated later. We cannot
            // update the viewer pose here because we don't yet know the desired reference space.
            m_frame = frame;

            // Update anchor positions.
            UpdateAnchors();

            // Update scene objects.
            UpdateSceneObjects(env);

            // Update planes.
            UpdatePlanes(env, timestamp);

            // Update meshes.
            UpdateMeshes(env, timestamp);

            // Update image tracking results.
            UpdateImageTrackingResults(env);
        }

        Napi::Promise XRFrame::CreateNativeAnchor(const Napi::CallbackInfo& info, xr::Pose pose, xr::NativeTrackablePtr nativeTrackable)
        {
            // Create the native anchor.
            auto nativeAnchor = m_frame->CreateAnchor(pose, nativeTrackable);

            // Create the XRAnchor object, and initialize its members.
            auto napiAnchor = Napi::Persistent(XRAnchor::New(info.Env()));
            auto* xrAnchor = XRAnchor::Unwrap(napiAnchor.Value());
            xrAnchor->SetAnchor(nativeAnchor);

            // Add the anchor to the list of tracked anchors.
            m_trackedAnchors.emplace_back(std::move(napiAnchor));

            // Resolve the promise with the newly created anchor.
            auto deferred = Napi::Promise::Deferred::New(info.Env());
            deferred.Resolve(m_trackedAnchors.back().Value());
            return deferred.Promise();
        }

        Napi::Value XRFrame::DeclareNativeAnchor(const Napi::Env& env, xr::NativeAnchorPtr nativeAnchor)
        {
            for (const auto& anchor : m_trackedAnchors)
            {
                const auto xrAnchor = XRAnchor::Unwrap(anchor.Value());
                if (xrAnchor->GetNativeAnchor().NativeAnchor == nativeAnchor)
                {
                    return anchor.Value();
                }
            }

            // Provide the native anchor to the native frame.
            auto newAnchor = m_frame->DeclareAnchor(nativeAnchor);

            // Create and populate the napi object.
            auto napiAnchor = Napi::Persistent(XRAnchor::New(env));
            auto xrAnchor = XRAnchor::Unwrap(napiAnchor.Value());
            xrAnchor->SetAnchor(newAnchor);

            // Track the napi object.
            m_trackedAnchors.emplace_back(std::move(napiAnchor));

            // Return the napi object.
            return napiAnchor.Value();
        }

        Napi::Value XRFrame::GetJSSceneObjectFromID(const Napi::CallbackInfo& info, const xr::System::Session::Frame::SceneObject::Identifier objectID)
        {
            if (objectID == xr::System::Session::Frame::SceneObject::INVALID_ID)
            {
                return info.Env().Undefined();
            }

            assert(m_sceneObjects.count(objectID) > 0);
            return m_sceneObjects.at(objectID).Value();
        }

        Napi::Value XRFrame::GetViewerPose(const Napi::CallbackInfo& info)
        {
            // To match the WebXR implementation we should return undefined here until we have gotten
            // initial tracking. After that point we can just continue returning the position
            // as it will be marked as estimated if tracking is lost.
            if (!m_hasBegunTracking && !m_frame->IsTracking)
            {
                return info.Env().Undefined();
            }
            else
            {
                // We've received initial tracking, update the flag
                m_hasBegunTracking = true;
            }

            // TODO: Support reference spaces.
            // auto& space = *XRReferenceSpace::Unwrap(info[0].As<Napi::Object>());

            // Updating the reference space is currently not supported. Until it is, we assume the
            // reference space is unmoving at identity (which is usually true).
            m_xrViewerPose.Update(info, *m_frame);

            return m_jsXRViewerPose.Value();
        }

        Napi::Value XRFrame::GetPoseData(const Napi::CallbackInfo& info)
        {
            assert(XRReferenceSpace::Unwrap(info[1].As<Napi::Object>()) != nullptr);
            const auto& space = *info[0].As<Napi::External<xr::Space>>().Data();
            auto vectorBuffer = info[2].As<Napi::ArrayBuffer>();
            auto matrixBuffer = info[3].As<Napi::ArrayBuffer>();
            m_transform.Update(space, vectorBuffer, matrixBuffer, false);
            return Napi::Boolean::From(info.Env(), true);
        }

        Napi::Value XRFrame::GetJointPose(const Napi::CallbackInfo& info)
        {
            assert(info[0].IsExternal());

            const auto& jointSpace = *info[0].As<Napi::External<xr::System::Session::Frame::JointSpace>>().Data();

            if (jointSpace.PoseTracked)
            {
                m_transform.Update(jointSpace, false);
                m_jsJointPose.Set("radius", jointSpace.PoseRadius);
                return m_jsJointPose.Value();
            }
            else
            {
                return info.Env().Undefined();
            }
        }

        Napi::Value XRFrame::FillPoses(const Napi::CallbackInfo& info)
        {
            const auto spaces = info[0].As<Napi::Array>();
            auto transforms = info[2].As<Napi::Float32Array>();
            if (spaces.Length() != (transforms.ElementLength() >> 4))
            {
                throw std::runtime_error{ "Number of spaces doesn't match number of transforms * 16." };
            }

            for (uint32_t spaceIdx = 0; spaceIdx < spaces.Length(); spaceIdx++)
            {
                const auto& jointSpace = *spaces[spaceIdx].As<Napi::External<xr::System::Session::Frame::JointSpace>>().Data();
                const auto transformMatrix = CreateTransformMatrix(jointSpace, false);
                std::memcpy(transforms.Data() + (spaceIdx << 4), transformMatrix.data(), sizeof(float) << 4);
            }

            return Napi::Value::From(info.Env(), true);
        }

        Napi::Value XRFrame::FillJointRadii(const Napi::CallbackInfo& info)
        {
            const auto spaces = info[0].As<Napi::Array>();
            auto radii = info[1].As<Napi::Float32Array>();
            if (spaces.Length() != radii.ElementLength())
            {
                throw std::runtime_error{ "Number of spaces doesn't match number of radii." };
            }

            for (uint32_t spaceIdx = 0; spaceIdx < spaces.Length(); spaceIdx++)
            {
                const auto& jointSpace = *spaces[spaceIdx].As<Napi::External<xr::System::Session::Frame::JointSpace>>().Data();
                const auto jointRadius = jointSpace.PoseRadius;
                radii.Data()[spaceIdx] = jointRadius;
            }

            return Napi::Value::From(info.Env(), true);
        }

        Napi::Value XRFrame::GetHitTestResults(const Napi::CallbackInfo& info)
        {
            XRHitTestSource* hitTestSource = XRHitTestSource::Unwrap(info[0].As<Napi::Object>());
            XRRay* offsetRay = hitTestSource->OffsetRay();
            xr::Ray nativeRay{};
            if (offsetRay != nullptr)
            {
                nativeRay = offsetRay->GetNativeRay();
            }
            else
            {
                nativeRay = { {0, 0, 0}, {0, 0, -1} };
            }

            // Get the native results
            std::vector<xr::HitResult> nativeHitResults{};
            m_frame->GetHitTestResults(nativeHitResults, nativeRay, hitTestSource->GetEntityTypes());

            // Translate those results into a napi array.
            auto results = Napi::Array::New(info.Env(), nativeHitResults.size());
            uint32_t i{ 0 };
            for (std::vector<xr::HitResult>::iterator it = nativeHitResults.begin(); it != nativeHitResults.end(); ++it)
            {
                Napi::Object currentResult = XRHitTestResult::New(info);
                XRHitTestResult* xrResult = XRHitTestResult::Unwrap(currentResult);
                xrResult->SetHitResult(*it);
                xrResult->SetXRFrame(this);

                results[i++] = currentResult;
            }

            return results;
        }

        Napi::Value XRFrame::CreateAnchor(const Napi::CallbackInfo& info)
        {
            XRRigidTransform* transform = XRRigidTransform::Unwrap(info[0].As<Napi::Object>());
            return CreateNativeAnchor(info, transform->GetNativePose(), nullptr);
        }

        Napi::Value XRFrame::GetTrackedAnchors(const Napi::CallbackInfo& info)
        {
            // Create a JavaScript set to store all currently tracked anchors.
            Napi::Object anchorSet = info.Env().Global().Get("Set").As<Napi::Function>().New({});

            // Loop over the list of tracked anchors, and add them to the set.
            for (const Napi::ObjectReference& napiAnchorRef : m_trackedAnchors)
            {
                anchorSet.Get("add").As<Napi::Function>().Call(anchorSet, { napiAnchorRef.Value() });
            }

            return anchorSet;
        }

        void XRFrame::UpdateAnchors()
        {
            // Loop over all anchors and update their state.
            std::vector<Napi::ObjectReference>::iterator anchorIter = m_trackedAnchors.begin();
            while (anchorIter != m_trackedAnchors.end())
            {
                XRAnchor* xrAnchor = XRAnchor::Unwrap((*anchorIter).Value());
                xr::Anchor& nativeAnchor = xrAnchor->GetNativeAnchor();

                // Update the anchor if it has not been marked for deletion.
                if (nativeAnchor.IsValid)
                {
                    m_frame->UpdateAnchor(nativeAnchor);
                }

                // If the anchor has been marked for deletion, delete the anchor from the session
                // and remove it from the list of tracked anchors.
                if (!nativeAnchor.IsValid)
                {
                    m_frame->DeleteAnchor(nativeAnchor);
                    anchorIter = m_trackedAnchors.erase(anchorIter);
                }
                else
                {
                    ++anchorIter;
                }
            }
        }

        Napi::Value XRFrame::GetWorldInformation(const Napi::CallbackInfo& info)
        {
            // Create a JavaScript object that stores all world information.
            Napi::Object worldInformationObj = Napi::Object::New(info.Env());

            // Create a set to contain all of the currently tracked planes.
            Napi::Object planeSet = info.Env().Global().Get("Set").As<Napi::Function>().New({});

            // Loop over the list of tracked planes, and add them to the set.
            for (const auto& [plane, planeNapiValue] : m_trackedPlanes)
            {
                planeSet.Get("add").As<Napi::Function>().Call(planeSet, { planeNapiValue.Value() });
            }

            // Pass the world information object back to the caller.
            worldInformationObj.Set("detectedPlanes", planeSet);

            if (m_meshSet)
            {
                worldInformationObj.Set("detectedMeshes", m_meshSet.Value());
            }

            return worldInformationObj;
        }

        Napi::Value XRFrame::GetFeaturePointCloud(const Napi::CallbackInfo& info)
        {
            // Get feature points from native.
            std::vector<xr::FeaturePoint>& pointCloud = m_frame->FeaturePointCloud;
            auto featurePointArray = Napi::Array::New(info.Env(), pointCloud.size() * 5);
            for (size_t i = 0; i < pointCloud.size(); i++)
            {
                int pointIndex = (int)i * 5;
                auto& featurePoint = pointCloud[i];
                featurePointArray.Set(pointIndex, Napi::Value::From(info.Env(), featurePoint.X));
                featurePointArray.Set(pointIndex + 1, Napi::Value::From(info.Env(), featurePoint.Y));
                featurePointArray.Set(pointIndex + 2, Napi::Value::From(info.Env(), featurePoint.Z));
                featurePointArray.Set(pointIndex + 3, Napi::Value::From(info.Env(), featurePoint.ConfidenceValue));
                featurePointArray.Set(pointIndex + 4, Napi::Value::From(info.Env(), featurePoint.ID));
            }

            return featurePointArray;
        }

        void XRFrame::UpdateSceneObjects(const Napi::Env& env)
        {
            for (const auto& sceneObjectID : m_frame->UpdatedSceneObjects)
            {
                if (m_sceneObjects.count(sceneObjectID) == 0)
                {
                    m_sceneObjects[sceneObjectID] = Napi::Persistent(Napi::Object::New(env));
                }

                const auto& sceneObject = m_frame->GetSceneObjectByID(sceneObjectID);
                m_sceneObjects.at(sceneObjectID).Value().Set("type", xr::SceneObjectTypeNames.at(sceneObject.Type));
            }

            for (const auto& removedObjectID : m_frame->RemovedSceneObjects)
            {
                m_sceneObjects.erase(removedObjectID);
            }
        }

        void XRFrame::UpdatePlanes(const Napi::Env& env, uint32_t timestamp)
        {
            // First loop over the list of updated planes, check if they exist in our map if not create them otherwise update them.
            for (auto planeID : m_frame->UpdatedPlanes)
            {
                XRPlane* xrPlane{};
                auto trackedPlaneIterator = m_trackedPlanes.find(planeID);

                // Plane does not yet exist create the JS object and insert it into the map.
                if (trackedPlaneIterator == m_trackedPlanes.end())
                {
                    auto napiPlane = Napi::Persistent(XRPlane::New(env));
                    xrPlane = XRPlane::Unwrap(napiPlane.Value());
                    xrPlane->SetNativePlaneId(planeID);
                    xrPlane->SetXRFrame(this);
                    m_trackedPlanes.insert({ planeID, std::move(napiPlane) });
                }
                else
                {
                    xrPlane = XRPlane::Unwrap(trackedPlaneIterator->second.Value());
                }

                xrPlane->Update(timestamp);
            }

            // Next go over removed planes and remove them from our mapping.
            for (auto planeID : m_frame->RemovedPlanes)
            {
                auto trackedPlaneIterator = m_trackedPlanes.find(planeID);
                assert(trackedPlaneIterator != m_trackedPlanes.end());
                m_trackedPlanes.erase(trackedPlaneIterator);
            }
        }

        void XRFrame::UpdateMeshes(const Napi::Env& env, uint32_t timestamp)
        {
            for (auto meshID : m_frame->UpdatedMeshes)
            {
                XRMesh* xrMesh{};
                auto trackedMeshIterator = m_trackedMeshes.find(meshID);

                if (trackedMeshIterator == m_trackedMeshes.end())
                {
                    auto napiMesh = Napi::Persistent(XRMesh::New(env));
                    xrMesh = XRMesh::Unwrap(napiMesh.Value());
                    xrMesh->SetNativeMeshId(meshID);
                    xrMesh->SetXRFrame(this);
                    m_trackedMeshes.insert({ meshID, std::move(napiMesh) });
                }
                else
                {
                    xrMesh = XRMesh::Unwrap(trackedMeshIterator->second.Value());
                }

                xrMesh->SetLastUpdatedTime(timestamp);
            }

            for (auto meshID : m_frame->RemovedMeshes)
            {
                m_trackedMeshes.erase(meshID);
            }

            // Create a new mesh set every frame, detected meshes are assumed immutable
            m_meshSet = Napi::Persistent(env.Global().Get("Set").As<Napi::Function>().New({}));
            for (const auto& [meshID, meshNapiValue] : m_trackedMeshes)
            {
                m_meshSet.Value().Get("add").As<Napi::Function>().Call(m_meshSet.Value(), { meshNapiValue.Value() });
            }
        }

        void XRFrame::UpdateImageTrackingResults(const Napi::Env& env)
        {
            // Loop over the list of updated image tracking results, check if they exist in our map if not create them otherwise update them.
            for (auto imageTrackingResultID : m_frame->UpdatedImageTrackingResults)
            {
                const auto trackedImageTrackingResultIterator{ m_trackedImageIDToResultMap.find(imageTrackingResultID) };

                // Get the matching native result
                xr::System::Session::Frame::ImageTrackingResult& nativeResult{ m_frame->GetImageTrackingResultByID(imageTrackingResultID) };

                // Convert the image tracking state from the native result to a string.
                const std::string trackingStateString = nativeResult.TrackingState == xr::ImageTrackingState::TRACKED ? XRImageTrackingState::TRACKED : XRImageTrackingState::EMULATED;

                // Result does not yet exist, create the JS object and insert it into the map.
                if (trackedImageTrackingResultIterator == m_trackedImageIDToResultMap.end())
                {
                    // Don't add untracked images.
                    if (nativeResult.TrackingState == xr::ImageTrackingState::UNTRACKED)
                    {
                        continue;
                    }

                    auto napiResult{ Napi::Object::New(env) };
                    napiResult.Set("index", Napi::Value::From(env, nativeResult.Index));
                    napiResult.Set("trackingState", Napi::Value::From(env, trackingStateString));
                    napiResult.Set("measuredWidthInMeters", Napi::Value::From(env, nativeResult.MeasuredWidthInMeters));
                    napiResult.Set("imageSpace", Napi::External<xr::Space>::New(env, &nativeResult.ImageSpace));

                    auto napiResultRef{ Napi::Weak(napiResult) };
                    auto imageTrackingArray{ m_imageTrackingResultsArray.Value() };
                    imageTrackingArray.Set(imageTrackingArray.Length(), napiResultRef.Value());
                    m_trackedImageIDToResultMap.insert({ imageTrackingResultID, std::move(napiResultRef) });
                }
                else
                {
                    // Update the tracked image.
                    auto napiResult{ trackedImageTrackingResultIterator->second.Value().As<Napi::Object>() };
                    napiResult.Set("trackingState", Napi::Value::From(env, trackingStateString));
                    napiResult.Set("measuredWidthInMeters", Napi::Value::From(env, nativeResult.MeasuredWidthInMeters));
                }
            }
        }
    } // Plugins
} // Babylon
