#pragma once

#include "XRCamera.h"

namespace Babylon
{
    class XRView : public Napi::ObjectWrap<XRView>
    {
        static constexpr auto JS_CLASS_NAME = "XRView";
        static constexpr size_t MATRIX_SIZE = 16;

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceAccessor("eye", &XRView::GetEye, nullptr),
                    InstanceAccessor("projectionMatrix", &XRView::GetProjectionMatrix, nullptr),
                    InstanceAccessor("transform", &XRView::GetTransform, nullptr),
                    InstanceAccessor("isFirstPersonObserver", &XRView::IsFirstPersonObserver, nullptr),
                    InstanceAccessor("camera", &XRView::GetCamera, nullptr),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
        }

        XRView(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRView>{info}
            , m_eyeIdx{0}
            , m_eye{XREye::IndexToEye(m_eyeIdx)}
            , m_projectionMatrix{Napi::Persistent(Napi::Float32Array::New(info.Env(), MATRIX_SIZE))}
            , m_rigidTransform{Napi::Persistent(XRRigidTransform::New(info.Env()))}
            , m_isFirstPersonObserver{false}
        {
        }

        void Update(const Napi::CallbackInfo& info, size_t eyeIdx, gsl::span<const float, 16> projectionMatrix, const xr::Space& space, bool isFirstPersonObserver,
            void* cameraTexturePointer, size_t cameraTextureWidth, size_t cameraTextureHeight)
        {
            if (eyeIdx != m_eyeIdx)
            {
                m_eyeIdx = eyeIdx;
                m_eye = XREye::IndexToEye(m_eyeIdx);
            }

            std::memcpy(m_projectionMatrix.Value().Data(), projectionMatrix.data(), m_projectionMatrix.Value().ByteLength());

            XRRigidTransform::Unwrap(m_rigidTransform.Value())->Update(space, false);

            m_isFirstPersonObserver = isFirstPersonObserver;

            // WebXR raw camera access: keep the per-view XRCamera in sync with the
            // platform camera texture (null on platforms/backends that don't provide one).
            if (cameraTexturePointer != nullptr)
            {
                if (m_camera.IsEmpty())
                {
                    m_camera = Napi::Persistent(XRCamera::New(info));
                }
                XRCamera::Unwrap(m_camera.Value())->Update(info.Env(), cameraTexturePointer, cameraTextureWidth, cameraTextureHeight);
            }
            else if (!m_camera.IsEmpty())
            {
                XRCamera::Unwrap(m_camera.Value())->Update(info.Env(), nullptr, 0, 0);
            }
        }

    private:
        size_t m_eyeIdx{};
        gsl::czstring m_eye{};
        Napi::Reference<Napi::Float32Array> m_projectionMatrix{};
        Napi::ObjectReference m_rigidTransform{};
        bool m_isFirstPersonObserver{};
        Napi::ObjectReference m_camera{};

        Napi::Value GetEye(const Napi::CallbackInfo& info)
        {
            return Napi::String::From(info.Env(), m_eye);
        }

        Napi::Value GetProjectionMatrix(const Napi::CallbackInfo&)
        {
            return m_projectionMatrix.Value();
        }

        Napi::Value GetTransform(const Napi::CallbackInfo&)
        {
            return m_rigidTransform.Value();
        }

        Napi::Value IsFirstPersonObserver(const Napi::CallbackInfo& info)
        {
            return Napi::Boolean::From(info.Env(), m_isFirstPersonObserver);
        }

        Napi::Value GetCamera(const Napi::CallbackInfo& info)
        {
            if (m_camera.IsEmpty() || !XRCamera::Unwrap(m_camera.Value())->HasTexture())
            {
                return info.Env().Undefined();
            }
            return m_camera.Value();
        }
    };
} // Babylon
