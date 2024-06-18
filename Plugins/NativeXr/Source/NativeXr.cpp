#include <Babylon/Plugins/NativeXr.h>

#include <Babylon/JsRuntimeScheduler.h>

#include <XR.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>


#include <napi/napi.h>

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
#include "NativeXrImpl.h"
#include "XRSession.h"
#include "NativeWebXRRenderTarget.h"
#include "NativeRenderTargetProvider.h"
#include "XR.h"

// NativeXr implementation proper.
namespace Babylon
{
    namespace Plugins
    {
        // Creates an anchor from a hit result.
        Napi::Value XRHitTestResult::CreateAnchor(const Napi::CallbackInfo& info)
        {
            return m_frame->CreateNativeAnchor(info, m_hitResult.Pose, m_hitResult.NativeTrackable);
        }

        xr::System::Session::Frame::Plane& XRPlane::GetPlane()
        {
            return m_frame->GetPlaneFromID(m_nativePlaneID);
        }

        Napi::Value XRPlane::GetParentSceneObject(const Napi::CallbackInfo& info)
        {
            const auto& plane = GetPlane();
            return m_frame->GetJSSceneObjectFromID(info, plane.ParentSceneObjectID);
        }

        xr::System::Session::Frame::Mesh& XRMesh::GetMesh()
        {
            return m_frame->GetMeshFromID(m_nativeMeshID);
        }

        Napi::Value XRMesh::GetParentSceneObject(const Napi::CallbackInfo& info)
        {
            const auto& mesh = GetMesh();
            return m_frame->GetJSSceneObjectFromID(info, mesh.ParentSceneObjectID);
        }
    }

    namespace Plugins
    {
        NativeXr::NativeXr(std::shared_ptr<Impl> impl)
            : m_impl{std::move(impl)}
        {
        }

        NativeXr::~NativeXr()
        {
        }

        NativeXr BABYLON_API NativeXr::Initialize(Napi::Env env)
        {
            auto impl{std::make_shared<Impl>(env)};

            PointerEvent::Initialize(env);

            XRWebGLBinding::Initialize(env);
            XRWebGLLayer::Initialize(env);
            XRRigidTransform::Initialize(env);
            XRView::Initialize(env);
            XRViewerPose::Initialize(env);
            XRPose::Initialize(env);
            XRReferenceSpace::Initialize(env);
            XRFrame::Initialize(env);
            XRHand::Initialize(env);
            XRPlane::Initialize(env);
            XRMesh::Initialize(env);
            XRAnchor::Initialize(env);
            XRHitTestSource::Initialize(env);
            XRHitTestResult::Initialize(env);
            XRRay::Initialize(env);
            XRSession::Initialize(env);
            NativeWebXRRenderTarget::Initialize(env);
            NativeRenderTargetProvider::Initialize(env);
            XR::Initialize(env, impl);

            return {impl};
        }

        void NativeXr::UpdateWindow(void* windowPtr)
        {
            m_impl->UpdateWindow(windowPtr);
        }

        void NativeXr::SetSessionStateChangedCallback(std::function<void(bool)> callback)
        {
            m_impl->SetSessionStateChangedCallback(std::move(callback));
        }
    }
}
