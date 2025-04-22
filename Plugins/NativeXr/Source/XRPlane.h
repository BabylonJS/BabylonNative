#pragma once

namespace Babylon
{
    namespace Plugins
    {
        // Implementation of the XRPlane interface: https://github.com/immersive-web/real-world-geometry/blob/master/plane-detection-explainer.md
        class XRPlane : public Napi::ObjectWrap<XRPlane>
        {
            static constexpr auto JS_CLASS_NAME = "XRPlane";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("polygon", &XRPlane::GetPolygon, nullptr),
                        InstanceAccessor("lastChangedTime", &XRPlane::GetLastChangedTime, nullptr),
                        InstanceAccessor("parentSceneObject", &XRPlane::GetParentSceneObject, nullptr),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::Env& env)
            {
                return env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRPlane(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRPlane>{info}
                , m_jsThis{Napi::Persistent(info.This().As<Napi::Object>())}
                , m_jsPlaneSpace{Napi::External<xr::Space>::New(info.Env(), &m_planeSpace)}
            {
                m_jsThis.Set("planeSpace", m_jsPlaneSpace);
            }

            void SetNativePlaneId(xr::System::Session::Frame::Plane::Identifier planeID)
            {
                m_nativePlaneID = planeID;
            }

            void SetXRFrame(Plugins::XRFrame* frame)
            {
                m_frame = frame;
            }

            void Update(uint32_t timestamp)
            {
                m_planeSpace.Pose = GetPlane().Center;
                m_lastUpdatedTimestamp = timestamp;
            }

        private:
            xr::System::Session::Frame::Plane& GetPlane()
            {
                return m_frame->GetPlaneFromID(m_nativePlaneID);
            }

            Napi::Value GetPolygon(const Napi::CallbackInfo& info)
            {
                // Translate the polygon from a native array to a JS array.
                auto& nativePlane = GetPlane();
                auto polygonArray = Napi::Array::New(info.Env(), nativePlane.PolygonSize);
                for (size_t i = 0; i < nativePlane.PolygonSize; i++)
                {
                    auto polygonPoint = Napi::Object::New(info.Env());
                    if (nativePlane.PolygonFormat == xr::PolygonFormat::XZ)
                    {
                        size_t polygonIndex = 2 * i;
                        polygonPoint.Set("x", nativePlane.Polygon[polygonIndex]);
                        polygonPoint.Set("y", 0);
                        polygonPoint.Set("z", nativePlane.Polygon[polygonIndex + 1]);
                    }
                    else
                    {
                        size_t polygonIndex = 3 * i;
                        polygonPoint.Set("x", nativePlane.Polygon[polygonIndex]);
                        polygonPoint.Set("y", nativePlane.Polygon[polygonIndex + 1]);
                        polygonPoint.Set("z", nativePlane.Polygon[polygonIndex + 2]);
                    }

                    polygonArray.Set((int)i, polygonPoint);
                }

                return polygonArray;
            }

            Napi::Value GetLastChangedTime(const Napi::CallbackInfo& info)
            {
                return Napi::Value::From(info.Env(), m_lastUpdatedTimestamp);
            }

            Napi::Value GetParentSceneObject(const Napi::CallbackInfo& info)
            {
                const auto& plane = GetPlane();
                return m_frame->GetJSSceneObjectFromID(info, plane.ParentSceneObjectID);
            }

            Napi::ObjectReference m_jsThis;
            xr::Space m_planeSpace{};
            const Napi::External<xr::Space> m_jsPlaneSpace;

            // The last timestamp when this frame was updated (Pulled in from RequestAnimationFrame).
            uint32_t m_lastUpdatedTimestamp{0};

            // The underlying native plane.
            xr::System::Session::Frame::Plane::Identifier m_nativePlaneID{};

            // Pointer to the XRFrame object.
            Plugins::XRFrame* m_frame{};
        };
    } // Plugins
} // Babylon
