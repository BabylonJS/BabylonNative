#pragma once

namespace Babylon
{
    // Implementation of XRRay: https://immersive-web.github.io/hit-test/#xrray-interface
    class XRRay : public Napi::ObjectWrap<XRRay>
    {
        static constexpr auto JS_CLASS_NAME = "XRRay";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceAccessor("origin", &XRRay::Origin, nullptr),
                    InstanceAccessor("direction", &XRRay::Direction, nullptr),
                    InstanceAccessor("matrix", &XRRay::Matrix, nullptr),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
        }

        XRRay(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRRay>{info}
            , m_origin{Napi::Persistent(Napi::Object::New(info.Env()))}
            , m_direction{Napi::Persistent(Napi::Object::New(info.Env()))}
        {
            auto argLength{info.Length()};
            xr::Ray tempVals{};

            tempVals.Direction.Z = -1.0;

            // Currently the constructor is either sent a BABYLON.Vector3, {}, an XRRigidTransform, or {x,y,z,w},{x,y,z,w}
            if (argLength > 0 && info[0].IsObject())
            {
                auto argumentObject{info[0].As<Napi::Object>()};

                // check properties of XRRigidTransform are present
                if (argumentObject.Has("position") && argumentObject.Has("orientation") && argumentObject.Has("matrix"))
                {
                    XRRigidTransform* transform{XRRigidTransform::Unwrap(argumentObject)};
                    // The value passed in to the constructor is an XRRigidTransform
                    xr::Pose pose{transform->GetNativePose()};
                    tempVals.Origin = pose.Position;

                    // Grab forward direction from quaternion
                    tempVals.Direction.X = 2 * ((pose.Orientation.X * pose.Orientation.Z) + (pose.Orientation.W * pose.Orientation.Y));
                    tempVals.Direction.Y = 2 * ((pose.Orientation.Y * pose.Orientation.Z) - (pose.Orientation.W * pose.Orientation.X));
                    tempVals.Direction.Z = 1 - (2 * ((pose.Orientation.X * pose.Orientation.X) + (pose.Orientation.Y * pose.Orientation.Y)));
                }
                else
                {
                    if (argumentObject.Has("x"))
                    {
                        tempVals.Origin.X = argumentObject.Get("x").ToNumber().FloatValue();
                    }
                    if (argumentObject.Has("y"))
                    {
                        tempVals.Origin.Y = argumentObject.Get("y").ToNumber().FloatValue();
                    }
                    if (argumentObject.Has("z"))
                    {
                        tempVals.Origin.Z = argumentObject.Get("z").ToNumber().FloatValue();
                    }
                    if (argumentObject.Has("w") && argumentObject.Get("w").ToNumber().FloatValue() != 1.0)
                    {
                        throw Napi::Error::New(info.Env(), "TypeError: w-axis provided for XRRay's Origin is not 1");
                    }
                }
            }
            if (argLength >= 2 && info[1].IsObject())
            {
                auto argumentObject{info[1].As<Napi::Object>()};

                if (argumentObject.Has("x"))
                {
                    tempVals.Direction.X = argumentObject.Get("x").ToNumber().FloatValue();
                }
                if (argumentObject.Has("y"))
                {
                    tempVals.Direction.Y = argumentObject.Get("y").ToNumber().FloatValue();
                }
                if (argumentObject.Has("z"))
                {
                    tempVals.Direction.Z = argumentObject.Get("z").ToNumber().FloatValue();
                }
                if (argumentObject.Has("w") && argumentObject.Get("w").ToNumber().FloatValue() != 0.0)
                {
                    throw Napi::Error::New(info.Env(), "TypeError: w-axis provided for XRRay's Direction is not 0");
                }
            }

            // Normalize the direction
            auto norm{bx::normalize(bx::Vec3(tempVals.Direction.X, tempVals.Direction.Y, tempVals.Direction.Z))};
            tempVals.Direction = {norm.x, norm.y, norm.z};

            m_origin.Set("x", Napi::Value::From(info.Env(), tempVals.Origin.X));
            m_origin.Set("y", Napi::Value::From(info.Env(), tempVals.Origin.Y));
            m_origin.Set("z", Napi::Value::From(info.Env(), tempVals.Origin.Z));
            m_origin.Set("w", Napi::Value::From(info.Env(), 1.0));
            m_direction.Set("x", Napi::Value::From(info.Env(), tempVals.Direction.X));
            m_direction.Set("y", Napi::Value::From(info.Env(), tempVals.Direction.Y));
            m_direction.Set("z", Napi::Value::From(info.Env(), tempVals.Direction.Z));
            m_direction.Set("w", Napi::Value::From(info.Env(), 0));
        }

        xr::Ray GetNativeRay()
        {
            xr::Ray nativeRay{{0, 0, 0}, {0, 0, -1}};
            auto originObject{m_origin.Value()};
            nativeRay.Origin.X = originObject.Get("x").ToNumber().FloatValue();
            nativeRay.Origin.Y = originObject.Get("y").ToNumber().FloatValue();
            nativeRay.Origin.Z = originObject.Get("z").ToNumber().FloatValue();

            auto directionObject{m_direction.Value()};
            nativeRay.Direction.X = directionObject.Get("x").ToNumber().FloatValue();
            nativeRay.Direction.Y = directionObject.Get("y").ToNumber().FloatValue();
            nativeRay.Direction.Z = directionObject.Get("z").ToNumber().FloatValue();

            return nativeRay;
        }

    private:
        Napi::ObjectReference m_origin{};
        Napi::ObjectReference m_direction{};

        Napi::Value Origin(const Napi::CallbackInfo&)
        {
            return m_origin.Value();
        }

        Napi::Value Direction(const Napi::CallbackInfo&)
        {
            return m_direction.Value();
        }

        Napi::Value Matrix(const Napi::CallbackInfo& info)
        {
            throw Napi::Error::New(info.Env(), "XRRay.matrix is not implemented");
        }
    };
} // Babylon
