#pragma once

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

        void Update(size_t eyeIdx, gsl::span<const float, 16> projectionMatrix, const xr::Space& space, bool isFirstPersonObserver)
        {
            if (eyeIdx != m_eyeIdx)
            {
                m_eyeIdx = eyeIdx;
                m_eye = XREye::IndexToEye(m_eyeIdx);
            }

            std::memcpy(m_projectionMatrix.Value().Data(), projectionMatrix.data(), m_projectionMatrix.Value().ByteLength());

            XRRigidTransform::Unwrap(m_rigidTransform.Value())->Update(space, false);

            m_isFirstPersonObserver = isFirstPersonObserver;
        }

    private:
        size_t m_eyeIdx{};
        gsl::czstring m_eye{};
        Napi::Reference<Napi::Float32Array> m_projectionMatrix{};
        Napi::ObjectReference m_rigidTransform{};
        bool m_isFirstPersonObserver{};

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
    };
} // Babylon
