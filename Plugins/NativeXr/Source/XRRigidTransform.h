#pragma once

#include <bx/math.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>

namespace Babylon
{
    // clang-format off
    constexpr std::array<float, 16> IDENTITY_MATRIX{
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
    // clang-format on

    inline std::array<float, 16> CreateTransformMatrix(const xr::Space& space, bool viewSpace = true)
    {
        auto& quat = space.Pose.Orientation;
        auto& pos = space.Pose.Position;

        // Quaternion to matrix from https://github.com/BabylonJS/Babylon.js/blob/v4.0.0/src/Maths/math.ts#L6245-L6283
        const float xx{ quat.X * quat.X };
        const float yy{ quat.Y * quat.Y };
        const float zz{ quat.Z * quat.Z };
        const float xy{ quat.X * quat.Y };
        const float zw{ quat.Z * quat.W };
        const float zx{ quat.Z * quat.X };
        const float yw{ quat.Y * quat.W };
        const float yz{ quat.Y * quat.Z };
        const float xw{ quat.X * quat.W };

        auto worldSpaceTransform{ IDENTITY_MATRIX };

        worldSpaceTransform[0] = 1.f - (2.f * (yy + zz));
        worldSpaceTransform[1] = 2.f * (xy + zw);
        worldSpaceTransform[2] = 2.f * (zx - yw);
        worldSpaceTransform[3] = 0.f;

        worldSpaceTransform[4] = 2.f * (xy - zw);
        worldSpaceTransform[5] = 1.f - (2.f * (zz + xx));
        worldSpaceTransform[6] = 2.f * (yz + xw);
        worldSpaceTransform[7] = 0.f;

        worldSpaceTransform[8] = 2.f * (zx + yw);
        worldSpaceTransform[9] = 2.f * (yz - xw);
        worldSpaceTransform[10] = 1.f - (2.f * (yy + xx));
        worldSpaceTransform[11] = 0.f;

        // Insert position into rotation matrix.
        worldSpaceTransform[12] = pos.X;
        worldSpaceTransform[13] = pos.Y;
        worldSpaceTransform[14] = pos.Z;
        worldSpaceTransform[15] = 1.f;

        if (viewSpace)
        {
            // Invert to get the view space transform.
            std::array<float, 16> viewSpaceTransform{};
            bx::mtxInverse(viewSpaceTransform.data(), worldSpaceTransform.data());

            return viewSpaceTransform;
        }
        else
        {
            return worldSpaceTransform;
        }
    }

    class XRRigidTransform : public Napi::ObjectWrap<XRRigidTransform>
    {
        static constexpr auto JS_CLASS_NAME = "XRRigidTransform";
        // static constexpr size_t VECTOR_SIZE = 4;
        static constexpr size_t MATRIX_SIZE = 16;

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceAccessor("position", &XRRigidTransform::Position, nullptr),
                    InstanceAccessor("orientation", &XRRigidTransform::Orientation, nullptr),
                    InstanceAccessor("matrix", &XRRigidTransform::Matrix, nullptr),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::Env& env)
        {
            return env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
        }

        XRRigidTransform(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRRigidTransform>{info}
            , m_matrix{Napi::Persistent(Napi::Float32Array::New(info.Env(), MATRIX_SIZE))}
        {
            const auto env = info.Env();
            if (info.Length() == 0)
            {
                m_position = Napi::Persistent(MakePoint(env, {0, 0, 0, 1}));
                m_orientation = Napi::Persistent(MakePoint(env, {0, 0, 0, 1}));
                std::memcpy(m_matrix.Value().Data(), IDENTITY_MATRIX.data(), sizeof(IDENTITY_MATRIX));
                return;
            }
            const auto position = ReadPoint(info[0]);
            auto orientation = ReadPoint(info[1]);
            if (position[3] != 1.0)
            {
                throw Napi::TypeError::New(env, "XRRigidTransform position.w must be 1.");
            }
            const auto finite = [](double value) { return std::isfinite(value); };
            if (!std::all_of(position.begin(), position.end(), finite) || !std::all_of(orientation.begin(), orientation.end(), finite))
            {
                throw Napi::TypeError::New(env, "XRRigidTransform components must be finite.");
            }
            if (orientation != Point{0, 0, 0, 1})
            {
                // Scale before squaring so even very large or subnormal finite quaternions normalize.
                const double scale = std::max({std::abs(orientation[0]), std::abs(orientation[1]), std::abs(orientation[2]), std::abs(orientation[3])});
                if (scale == 0.0)
                {
                    const auto message = "XRRigidTransform orientation cannot have zero length.";
                    const auto ctor = env.Global().Get("DOMException");
                    if (ctor.IsFunction())
                    {
                        throw Napi::Error{env, ctor.As<Napi::Function>().New({Napi::String::New(env, message), Napi::String::New(env, "InvalidStateError")})};
                    }
                    auto error = Napi::Error::New(env, message);
                    error.Value().Set("name", "InvalidStateError");
                    throw error;
                }
                double squaredLength{};
                for (auto& component : orientation)
                {
                    component /= scale;
                    squaredLength += component * component;
                }
                const double length = std::sqrt(squaredLength);
                for (auto& component : orientation)
                {
                    component /= length;
                }
            }
            m_position = Napi::Persistent(MakePoint(env, position));
            m_orientation = Napi::Persistent(MakePoint(env, orientation));
            const xr::Space space{{{static_cast<float>(position[0]), static_cast<float>(position[1]), static_cast<float>(position[2])},
                {static_cast<float>(orientation[0]), static_cast<float>(orientation[1]), static_cast<float>(orientation[2]), static_cast<float>(orientation[3])}}};
            const auto matrix = CreateTransformMatrix(space, false);
            std::memcpy(m_matrix.Value().Data(), matrix.data(), sizeof(matrix));
        }

        void Update(XRRigidTransform* transform)
        {
            Update({transform->GetNativePose()}, false);
        }

        void Update(const xr::Space& space, bool isViewSpace)
        {
            auto position = m_position.Value();
            position.Set("x", space.Pose.Position.X);
            position.Set("y", space.Pose.Position.Y);
            position.Set("z", space.Pose.Position.Z);
            position.Set("w", 1.f);

            auto orientation = m_orientation.Value();
            orientation.Set("x", space.Pose.Orientation.X);
            orientation.Set("y", space.Pose.Orientation.Y);
            orientation.Set("z", space.Pose.Orientation.Z);
            orientation.Set("w", space.Pose.Orientation.W);

            std::memcpy(m_matrix.Value().Data(), CreateTransformMatrix(space, isViewSpace).data(), m_matrix.Value().ByteLength());
        }

        void Update(const xr::Space& space, Napi::ArrayBuffer& outVectorData, Napi::ArrayBuffer& outMatrixData, bool isViewSpace)
        {
            float posAndOrientationData[8];
            posAndOrientationData[0] = space.Pose.Position.X;
            posAndOrientationData[1] = space.Pose.Position.Y;
            posAndOrientationData[2] = space.Pose.Position.Z;
            posAndOrientationData[3] = 1.f;
            posAndOrientationData[4] = space.Pose.Orientation.X;
            posAndOrientationData[5] = space.Pose.Orientation.Y;
            posAndOrientationData[6] = space.Pose.Orientation.Z;
            posAndOrientationData[7] = space.Pose.Orientation.W;

            std::memcpy(outVectorData.Data(), posAndOrientationData, sizeof(float) * 8);
            std::memcpy(outMatrixData.Data(), CreateTransformMatrix(space, isViewSpace).data(), sizeof(float) * 16);
        }

        void Update(const xr::Pose& pose)
        {
            xr::Space space{{pose}};
            Update(space, true);
        }

        xr::Pose GetNativePose()
        {
            auto position = m_position.Value();
            auto orientation = m_orientation.Value();
            return {
                {position.Get("x").ToNumber().FloatValue(), position.Get("y").ToNumber().FloatValue(), position.Get("z").ToNumber().FloatValue()},
                {orientation.Get("x").ToNumber().FloatValue(), orientation.Get("y").ToNumber().FloatValue(), orientation.Get("z").ToNumber().FloatValue(), orientation.Get("w").ToNumber().FloatValue()}};
        }

    private:
        using Point = std::array<double, 4>;

        static Point ReadPoint(const Napi::Value& value)
        {
            Point point{0, 0, 0, 1};
            if (value.IsUndefined() || value.IsNull())
            {
                return point;
            }
            if (!value.IsObject() && !value.IsFunction())
            {
                throw Napi::TypeError::New(value.Env(), "Expected a DOMPointInit dictionary.");
            }
            const auto object = value.As<Napi::Object>();
            // Web IDL reads dictionary members once, in lexicographic order.
            const auto read = [&](const char* name, size_t index) {
                const auto member = object.Get(name);
                if (!member.IsUndefined())
                {
                    point[index] = member.ToNumber().DoubleValue();
                }
            };
            read("w", 3);
            read("x", 0);
            read("y", 1);
            read("z", 2);
            return point;
        }

        static Napi::Object MakePoint(Napi::Env env, const Point& point)
        {
            auto result = Napi::Object::New(env);
            result.Set("x", point[0]);
            result.Set("y", point[1]);
            result.Set("z", point[2]);
            result.Set("w", point[3]);
            return result;
        }

        Napi::ObjectReference m_position{};
        Napi::ObjectReference m_orientation{};
        Napi::Reference<Napi::Float32Array> m_matrix{};

        Napi::Value Position(const Napi::CallbackInfo&)
        {
            return m_position.Value();
        }

        Napi::Value Orientation(const Napi::CallbackInfo&)
        {
            return m_orientation.Value();
        }

        Napi::Value Matrix(const Napi::CallbackInfo&)
        {
            return m_matrix.Value();
        }
    };
} // Babylon
