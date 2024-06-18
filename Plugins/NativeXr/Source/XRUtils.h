#pragma once

#include <bx/math.h>
#include <bgfx/bgfx.h>

namespace
{
    bgfx::TextureFormat::Enum XrTextureFormatToBgfxFormat(xr::TextureFormat format)
    {
        switch (format)
        {
            // Color Formats
            // NOTE: Use linear formats even though XR requests sRGB to match what happens on the web.
            //       WebGL shaders expect sRGB output while native shaders expect linear output.
            case xr::TextureFormat::BGRA8_SRGB:
                return bgfx::TextureFormat::BGRA8;
            case xr::TextureFormat::RGBA8_SRGB:
                return bgfx::TextureFormat::RGBA8;

            // Depth Formats
            case xr::TextureFormat::D24S8:
                return bgfx::TextureFormat::D24S8;
            case xr::TextureFormat::D16:
                return bgfx::TextureFormat::D16;

            default:
                throw std::runtime_error{"Unsupported texture format"};
        }
    }

    // clang-format off
    constexpr std::array<float, 16> IDENTITY_MATRIX{
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
    // clang-format on

    std::array<float, 16> CreateTransformMatrix(const xr::Space& space, bool viewSpace = true)
    {
        auto& quat = space.Pose.Orientation;
        auto& pos = space.Pose.Position;

        // Quaternion to matrix from https://github.com/BabylonJS/Babylon.js/blob/v4.0.0/src/Maths/math.ts#L6245-L6283
        const float xx{quat.X * quat.X};
        const float yy{quat.Y * quat.Y};
        const float zz{quat.Z * quat.Z};
        const float xy{quat.X * quat.Y};
        const float zw{quat.Z * quat.W};
        const float zx{quat.Z * quat.X};
        const float yw{quat.Y * quat.W};
        const float yz{quat.Y * quat.Z};
        const float xw{quat.X * quat.W};

        auto worldSpaceTransform{IDENTITY_MATRIX};

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

    constexpr std::array<const char*, 25> HAND_JOINT_NAMES{
        "wrist",

        "thumb-metacarpal",
        "thumb-phalanx-proximal",
        "thumb-phalanx-distal",
        "thumb-tip",

        "index-finger-metacarpal",
        "index-finger-phalanx-proximal",
        "index-finger-phalanx-intermediate",
        "index-finger-phalanx-distal",
        "index-finger-tip",

        "middle-finger-metacarpal",
        "middle-finger-phalanx-proximal",
        "middle-finger-phalanx-intermediate",
        "middle-finger-phalanx-distal",
        "middle-finger-tip",

        "ring-finger-metacarpal",
        "ring-finger-phalanx-proximal",
        "ring-finger-phalanx-intermediate",
        "ring-finger-phalanx-distal",
        "ring-finger-tip",

        "pinky-finger-metacarpal",
        "pinky-finger-phalanx-proximal",
        "pinky-finger-phalanx-intermediate",
        "pinky-finger-phalanx-distal",
        "pinky-finger-tip"};

    void SetXRInputSourceData(Napi::Object& jsInputSource, xr::System::Session::Frame::InputSource& inputSource)
    {
        auto env = jsInputSource.Env();
        jsInputSource.Set("targetRaySpace", Napi::External<xr::Space>::New(env, &inputSource.AimSpace));
        jsInputSource.Set("gripSpace", Napi::External<xr::Space>::New(env, &inputSource.GripSpace));

        // Don't set hands up unless hand data is supported/available
        if (inputSource.HandTrackedThisFrame || inputSource.JointsTrackedThisFrame)
        {
            auto profiles = Napi::Array::New(env, 2);
            profiles.Set(uint32_t{0}, Napi::String::New(env, "generic-hand-select-grasp"));
            profiles.Set(uint32_t{1}, Napi::String::New(env, "generic-hand-select"));
            jsInputSource.Set("profiles", profiles);

            if (inputSource.JointsTrackedThisFrame)
            {
                const auto shouldInitHand = !jsInputSource.Has("hand");
                auto handJointCollection = shouldInitHand ? Napi::Array::New(env, HAND_JOINT_NAMES.size()) : jsInputSource.Get("hand").As<Napi::Array>();
                if (shouldInitHand)
                {
                    auto jointGetter = [handJointCollection](const Napi::CallbackInfo& info) -> Napi::Value {
                        return handJointCollection.Get(info[0].As<Napi::String>());
                    };

                    handJointCollection.Set("get", Napi::Function::New(env, jointGetter, "get"));
                    handJointCollection.Set("size", static_cast<int>(HAND_JOINT_NAMES.size()));

                    jsInputSource.Set("hand", handJointCollection);
                }

                for (size_t i = 0; i < HAND_JOINT_NAMES.size(); i++)
                {
                    auto napiJoint = Napi::External<xr::System::Session::Frame::JointSpace>::New(env, &inputSource.HandJoints[i]);
                    handJointCollection.Set(HAND_JOINT_NAMES[i], napiJoint);
                }
            }
            else
            {
                // If hand joints aren't available on a hand input, send a null hand object
                jsInputSource.Set("hand", env.Null());
            }
        }
    }

    void SetXRGamepadObjectData(Napi::Object& jsInputSource, Napi::Object& jsGamepadObject, xr::System::Session::Frame::InputSource& inputSource)
    {
        auto env = jsInputSource.Env();
        //Set Gamepad Object
        auto gamepadButtons = Napi::Array::New(env, inputSource.GamepadObject.Buttons.size());
        for (size_t i = 0; i < inputSource.GamepadObject.Buttons.size(); i++)
        {
            auto gamepadButton = Napi::Object::New(env);
            auto napiGamepadPressed = Napi::Boolean::New(env, inputSource.GamepadObject.Buttons[i].Pressed);
            auto napiGamepadTouched = Napi::Boolean::New(env, inputSource.GamepadObject.Buttons[i].Touched);
            auto napiGamepadValue = Napi::Number::New(env, inputSource.GamepadObject.Buttons[i].Value);
            gamepadButton.Set("pressed", napiGamepadPressed);
            gamepadButton.Set("touched", napiGamepadTouched);
            gamepadButton.Set("value", napiGamepadValue);
            gamepadButtons.Set(static_cast<int>(i), gamepadButton);
        }
        jsGamepadObject.Set("buttons", gamepadButtons);

        auto gamepadAxes = Napi::Array::New(env, inputSource.GamepadObject.Axes.size());
        for (size_t i = 0; i < inputSource.GamepadObject.Axes.size(); i++)
        {
            auto napiGamepadAxesValue = Napi::Number::New(env, inputSource.GamepadObject.Axes[i]);
            gamepadAxes.Set(static_cast<int>(i), napiGamepadAxesValue);
        }
        jsGamepadObject.Set("axes", gamepadAxes);
        jsInputSource.Set("gamepad", jsGamepadObject);
    }

    Napi::ObjectReference CreateXRInputSource(xr::System::Session::Frame::InputSource& inputSource, Napi::Env& env)
    {
        constexpr std::array<const char*, 2> HANDEDNESS_STRINGS{
            "left",
            "right"};
        constexpr const char* TARGET_RAY_MODE{"tracked-pointer"};

        auto jsInputSource = Napi::Object::New(env);

        jsInputSource.Set("handedness", Napi::String::New(env, HANDEDNESS_STRINGS[static_cast<size_t>(inputSource.Handedness)]));
        jsInputSource.Set("targetRayMode", TARGET_RAY_MODE);

        auto profiles = Napi::Array::New(env, 1);
        Napi::Value string = Napi::String::New(env, "generic-trigger-squeeze-touchpad-thumbstick");
        profiles.Set(uint32_t{0}, string);
        jsInputSource.Set("profiles", profiles);

        SetXRInputSourceData(jsInputSource, inputSource);

        return Napi::Persistent(jsInputSource);
    }

    void PopulateDetectionBoundary(const Napi::Object& object, xr::DetectionBoundary& detectionBoundary)
    {
        if (object.Has("type"))
        {
            const std::map<std::string, xr::DetectionBoundaryType> detectionBoundaryTypeMap{
                {"box", xr::DetectionBoundaryType::Box},
                {"frustum", xr::DetectionBoundaryType::Frustum},
                {"sphere", xr::DetectionBoundaryType::Sphere}};
            detectionBoundary.Type = detectionBoundaryTypeMap.at(object.Get("type").As<Napi::String>());
        }

        switch (detectionBoundary.Type)
        {
            case xr::DetectionBoundaryType::Box:
                if (object.Has("extent"))
                {
                    const auto& vector = object.Get("extent").As<Napi::Object>();
                    xr::Vector3f boxDimensions{};
                    boxDimensions.X = vector.Get("x").As<Napi::Number>();
                    boxDimensions.Y = vector.Get("y").As<Napi::Number>();
                    boxDimensions.Z = vector.Get("z").As<Napi::Number>();
                    detectionBoundary.Data = boxDimensions;
                }
                break;
            case xr::DetectionBoundaryType::Frustum:
                if (object.Has("frustum"))
                {
                    const auto& frustum = object.Get("frustum").As<Napi::Object>();
                    xr::Frustum frustumData{};
                    frustumData.FarDistance = frustum.Get("farDistance").As<Napi::Number>();

                    const auto& vector = frustum.Get("position").As<Napi::Object>();
                    frustumData.Pose.Position.X = vector.Get("x").As<Napi::Number>();
                    frustumData.Pose.Position.Y = vector.Get("y").As<Napi::Number>();
                    frustumData.Pose.Position.Z = vector.Get("z").As<Napi::Number>();

                    const auto& quaternion = frustum.Get("orientation").As<Napi::Object>();
                    frustumData.Pose.Orientation.X = quaternion.Get("x").As<Napi::Number>();
                    frustumData.Pose.Orientation.Y = quaternion.Get("y").As<Napi::Number>();
                    frustumData.Pose.Orientation.Z = quaternion.Get("z").As<Napi::Number>();
                    frustumData.Pose.Orientation.W = quaternion.Get("w").As<Napi::Number>();

                    const auto& fov = frustum.Get("fieldOfView").As<Napi::Object>();
                    frustumData.FOV.AngleLeft = fov.Get("angleLeft").As<Napi::Number>();
                    frustumData.FOV.AngleRight = fov.Get("angleRight").As<Napi::Number>();
                    frustumData.FOV.AngleUp = fov.Get("angleUp").As<Napi::Number>();
                    frustumData.FOV.AngleDown = fov.Get("angleDown").As<Napi::Number>();

                    frustumData.FarDistance = frustum.Get("farDistance").As<Napi::Number>();

                    detectionBoundary.Data = frustumData;
                }
                break;
            case xr::DetectionBoundaryType::Sphere:
                if (object.Has("radius"))
                {
                    detectionBoundary.Data = object.Get("radius").As<Napi::Number>();
                }
                break;
        }
    }

    xr::GeometryDetectorOptions CreateDetectorOptions(const Napi::Object& object)
    {
        xr::GeometryDetectorOptions options{};
        if (object.Has("updateInterval"))
        {
            options.UpdateInterval = object.Get("updateInterval").As<Napi::Number>();
        }

        if (object.Has("detectionBoundary"))
        {
            const auto& detectionBoundary = object.Get("detectionBoundary").As<Napi::Object>();
            PopulateDetectionBoundary(detectionBoundary, options.DetectionBoundary);
        }

        return options;
    }

    void CreateXRGamepadObject(Napi::Object& jsInputSource, xr::System::Session::Frame::InputSource& inputSource)
    {
        auto env = jsInputSource.Env();
        auto jsGamepadObject = Napi::Object::New(env);
        SetXRGamepadObjectData(jsInputSource, jsGamepadObject, inputSource);
    }
}
