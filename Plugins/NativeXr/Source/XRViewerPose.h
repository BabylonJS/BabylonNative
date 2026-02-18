#pragma once

#include "XRView.h"

namespace Babylon
{
    class XRViewerPose : public Napi::ObjectWrap<XRViewerPose>
    {
        static constexpr auto JS_CLASS_NAME = "XRViewerPose";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceAccessor("transform", &XRViewerPose::GetTransform, nullptr),
                    InstanceAccessor("views", &XRViewerPose::GetViews, nullptr),
                    InstanceAccessor("emulatedPosition", &XRViewerPose::GetEmulatedPosition, nullptr),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
        }

        XRViewerPose(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRViewerPose>{info}
            , m_jsTransform{Napi::Persistent(XRRigidTransform::New(info.Env()))}
            , m_jsViews{Napi::Persistent(Napi::Array::New(info.Env(), 0))}
            , m_transform{*XRRigidTransform::Unwrap(m_jsTransform.Value())}
            , m_isEmulatedPosition{true}
        {
        }

        void Update(const Napi::CallbackInfo& info, const xr::System::Session::Frame& frame)
        {
            // Update the transform, for now assume that the pose of the first view if it exists represents the viewer transform.
            // This is correct for devices with a single view, but is likely incorrect for devices with multiple views (eg. VR/AR headsets with binocular views).
            if (frame.Views.size() > 0)
            {
                m_transform.Update(frame.Views[0].Space, true);
            }

            // Update the views array if necessary.
            const auto oldSize = static_cast<uint32_t>(m_views.size());
            const auto newSize = static_cast<uint32_t>(frame.Views.size());
            if (oldSize != newSize)
            {
                auto newViews = Napi::Array::New(m_jsViews.Env(), newSize);
                m_views.resize(newSize);

                for (uint32_t idx = 0; idx < newSize; ++idx)
                {
                    if (idx < oldSize)
                    {
                        newViews.Set(idx, m_jsViews.Value().Get(idx));
                    }
                    else
                    {
                        newViews.Set(idx, XRView::New(info));
                    }

                    m_views[idx] = XRView::Unwrap(newViews.Get(idx).As<Napi::Object>());
                }

                m_jsViews = Napi::Persistent(newViews);
            }

            // Update the individual views.
            for (uint32_t idx = 0; idx < static_cast<uint32_t>(frame.Views.size()); ++idx)
            {
                const auto& view = frame.Views[idx];
                m_views[idx]->Update(idx, view.ProjectionMatrix, view.Space, view.IsFirstPersonObserver);
            }

            // Check the frame to see if it has valid tracking, if it does not then the position should
            // be flagged as being emulated.
            m_isEmulatedPosition = !frame.IsTracking;
        }

    private:
        Napi::ObjectReference m_jsTransform{};
        Napi::Reference<Napi::Array> m_jsViews{};

        XRRigidTransform& m_transform;
        std::vector<XRView*> m_views{};
        bool m_isEmulatedPosition;

        Napi::Value GetTransform(const Napi::CallbackInfo& /*info*/)
        {
            return m_jsTransform.Value();
        }

        Napi::Value GetViews(const Napi::CallbackInfo& /*info*/)
        {
            return m_jsViews.Value();
        }

        Napi::Value GetEmulatedPosition(const Napi::CallbackInfo& info)
        {
            return Napi::Boolean::New(info.Env(), m_isEmulatedPosition);
        }
    };
} // Babylon
