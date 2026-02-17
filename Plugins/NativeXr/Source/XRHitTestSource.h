#pragma once

#include "XRRay.h"

namespace Babylon
{
    // Implementation of the XRHitTestSource interface: https://immersive-web.github.io/hit-test/#hit-test-source-interface
    class XRHitTestSource : public Napi::ObjectWrap<XRHitTestSource>
    {
        static constexpr auto JS_CLASS_NAME = "XRHitTestSource";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("cancel", &XRHitTestSource::Cancel),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
        }

        XRHitTestSource(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRHitTestSource>{info}
        {
            auto options = info[0].As<Napi::Object>();

            if (options.Has("space"))
            {
                auto spaceValue = options.Get("space");

                if (spaceValue.IsObject())
                {
                    m_space = Napi::Persistent(spaceValue.As<Napi::Object>());
                    hasSpace = true;
                }
            }

            if (options.Has("offsetRay"))
            {
                m_offsetRay = Napi::Persistent(options.Get("offsetRay").As<Napi::Object>());
                hasOffsetRay = true;
            }

            if (options.Has("entityTypes"))
            {
                const auto entityTypeArray = options.Get("entityTypes").As<Napi::Array>();
                for (uint32_t i = 0; i < entityTypeArray.Length(); i++)
                {
                    const auto entityType = entityTypeArray.Get(i).As<Napi::String>().Utf8Value();
                    if (entityType == XRHitTestTrackableType::POINT)
                    {
                        m_entityTypes |= xr::HitTestTrackableType::POINT;
                    }
                    else if (entityType == XRHitTestTrackableType::PLANE)
                    {
                        m_entityTypes |= xr::HitTestTrackableType::PLANE;
                    }
                    else if (entityType == XRHitTestTrackableType::MESH)
                    {
                        m_entityTypes |= xr::HitTestTrackableType::MESH;
                    }
                }
            }

            // Default to MESH if unspecified.
            if (m_entityTypes == xr::HitTestTrackableType::NONE)
            {
                m_entityTypes = xr::HitTestTrackableType::MESH;
            }
        }

        XRRay* OffsetRay()
        {
            return hasOffsetRay ? XRRay::Unwrap(m_offsetRay.Value()) : nullptr;
        }

        XRReferenceSpace* Space()
        {
            return hasSpace ? XRReferenceSpace::Unwrap(m_space.Value()) : nullptr;
        }

        xr::HitTestTrackableType GetEntityTypes()
        {
            return m_entityTypes;
        }

    private:
        void Cancel(const Napi::CallbackInfo& /*info*/)
        {
            // no-op we don't keep a persistent list of active XRHitTestSources..
        }

        bool hasSpace = false;
        Napi::ObjectReference m_space;

        bool hasOffsetRay = false;
        Napi::ObjectReference m_offsetRay;

        xr::HitTestTrackableType m_entityTypes{xr::HitTestTrackableType::NONE};
    };
} // Babylon
