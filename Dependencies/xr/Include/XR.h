#pragma once

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <arcana/threading/task.h>

namespace xr
{
    enum class TextureFormat
    {
        RGBA8_SRGB,
        BGRA8_SRGB,
        D24S8
    };

    enum class SessionType
    {
        IMMERSIVE_VR,
        IMMERSIVE_AR,
        INLINE,
        INVALID
    };

    enum class PolygonFormat
    {
        XZ,
        XYZ
    };

    enum class HitTestTrackableType {
        NONE = 0,
        POINT = 1 << 0,
        PLANE = 1 << 1,
        MESH = 1 << 2,
    };

    constexpr enum HitTestTrackableType operator |(const enum HitTestTrackableType selfValue, const enum HitTestTrackableType inValue)
    {
        return static_cast<const enum HitTestTrackableType>(std::underlying_type_t<HitTestTrackableType>(selfValue) | std::underlying_type_t<HitTestTrackableType>(inValue));
    }

    constexpr enum HitTestTrackableType operator &(const enum HitTestTrackableType selfValue, const enum HitTestTrackableType inValue)
    {
        return static_cast<const enum HitTestTrackableType>(std::underlying_type_t<HitTestTrackableType>(selfValue) & std::underlying_type_t<HitTestTrackableType>(inValue));
    }

    constexpr enum HitTestTrackableType& operator |=(enum HitTestTrackableType& selfValue, const enum HitTestTrackableType inValue)
    {
        selfValue = selfValue | inValue;
        return selfValue;
    }

    struct Size
    {
        size_t Width{};
        size_t Height{};
    };

    struct Vector3f
    {
        float X{0.f};
        float Y{0.f};
        float Z{0.f};
    };

    struct Vector4f
    {
        float X{0.f};
        float Y{0.f};
        float Z{0.f};
        float W{1.f};
    };

    struct Pose
    {
        Vector3f Position;
        Vector4f Orientation;
    };

    using NativeTrackablePtr = void*;
    struct HitResult
    {
        Pose Pose{};
        NativeTrackablePtr NativeTrackable{};
    };

    struct Ray
    {
        Vector3f Origin;
        Vector3f Direction;
    };

    using NativeAnchorPtr = void*;
    struct Anchor
    {
        Pose Pose{};
        NativeAnchorPtr NativeAnchor{};
        bool IsValid{true};
    };

    struct FeaturePoint
    {
        using Identifier = size_t;

        float X{};
        float Y{};
        float Z{};
        float ConfidenceValue{};
        Identifier ID{};
    };

    struct FieldOfView
    {
        float AngleLeft;
        float AngleRight;
        float AngleUp;
        float AngleDown;
    };

    enum class DetectionBoundaryType
    {
        Box,
        Frustum,
        Sphere
    };

    struct Frustum
    {
        Pose Pose{};
        FieldOfView FOV{};
        float FarDistance;
    };

    struct DetectionBoundary
    {
        DetectionBoundaryType Type{ DetectionBoundaryType::Sphere };
        std::variant<float, Frustum, Vector3f> Data{ 5.f };
    };

    struct GeometryDetectorOptions
    {
        xr::DetectionBoundary DetectionBoundary{};
        double UpdateInterval{10};
    };

    enum class SceneObjectType
    {
        Unknown,
        Background,
        Wall,
        Floor,
        Ceiling,
        Platform,
        Undefined
    };

    const std::map<xr::SceneObjectType, std::string> SceneObjectTypeNames
    {
        {xr::SceneObjectType::Unknown, "unknown" },
        {xr::SceneObjectType::Background, "background" },
        {xr::SceneObjectType::Ceiling, "ceiling" },
        {xr::SceneObjectType::Floor, "floor" },
        {xr::SceneObjectType::Platform, "platform" },
        {xr::SceneObjectType::Wall, "wall" }
    };

    class System
    {
    public:
        static constexpr float DEFAULT_DEPTH_NEAR_Z{ 0.5f };
        static constexpr float DEFAULT_DEPTH_FAR_Z{ 1000.f };
        static constexpr uint32_t DEFAULT_CONTROLLER_BUTTONS{ 4 };
        static constexpr uint32_t DEFAULT_CONTROLLER_AXES{ 4 };

        class Session
        {
            friend class System;
            struct Impl;

        public:
            class Frame
            {
            public:
                struct Space
                {
                    Pose Pose;
                };

                struct JointSpace : Space
                {
                    float PoseRadius{};
                    bool PoseTracked{ false };
                };

                struct GamePad
                {
                    struct Button
                    {
                        bool Pressed{ false };
                        bool Touched{ false };
                        float Value{0};
                    };

                    std::array<float, DEFAULT_CONTROLLER_AXES> Axes;
                    std::array<Button, DEFAULT_CONTROLLER_BUTTONS> Buttons;
                };

                struct View
                {
                    Space Space{};
                    std::array<float, 16> ProjectionMatrix{};

                    TextureFormat ColorTextureFormat{};
                    void* ColorTexturePointer{};
                    Size ColorTextureSize;

                    TextureFormat DepthTextureFormat{};
                    void* DepthTexturePointer{};
                    Size DepthTextureSize;

                    float DepthNearZ{};
                    float DepthFarZ{};

                    bool IsFirstPersonObserver{ false };
                };

                struct InputSource
                {
                    using Identifier = size_t;

                    enum class HandednessEnum
                    {
                        Left = 0,
                        Right = 1
                    };

                    const Identifier ID{ NEXT_ID++ };
                    bool TrackedThisFrame{};
                    bool JointsTrackedThisFrame{};
                    bool GamepadTrackedThisFrame{};
                    GamePad GamepadObject{};
                    Space GripSpace{};
                    Space AimSpace{};
                    HandednessEnum Handedness{};
                    std::vector<JointSpace> HandJoints{};

                private:
                    static inline Identifier NEXT_ID{ 0 };
                };

                struct SceneObject
                {
                    using Identifier = int32_t;
                    const static Identifier INVALID_ID = -1;

                    Identifier ID{ INVALID_ID };
                    SceneObjectType Type{ SceneObjectType::Undefined };
                };

                struct Plane
                {
                    using Identifier = size_t;
                    const Identifier ID{ NEXT_ID++ };
                    Pose Center{};
                    std::vector<float> Polygon{};
                    size_t PolygonSize{0};
                    PolygonFormat PolygonFormat{};
                    SceneObject::Identifier ParentSceneObjectID{ SceneObject::INVALID_ID };

                private:
                    static inline Identifier NEXT_ID{ 0 };
                };

                struct Mesh
                {
                    using IndexType = uint32_t;
                    using Identifier = size_t;
                    const Identifier ID{ NEXT_ID++ };
                    std::vector<xr::Vector3f> Positions{};
                    std::vector<IndexType> Indices{};
                    bool HasNormals{ false };
                    std::vector<xr::Vector3f> Normals;
                    SceneObject::Identifier ParentSceneObjectID{ SceneObject::INVALID_ID };

                private:
                    static inline Identifier NEXT_ID{ 0 };
                };

                std::vector<View>& Views;
                std::vector<InputSource>& InputSources;
                std::vector<Plane>& Planes;
                std::vector<Mesh>& Meshes;
                std::vector<FeaturePoint>& FeaturePointCloud;

                std::vector<SceneObject::Identifier>UpdatedSceneObjects;
                std::vector<SceneObject::Identifier>RemovedSceneObjects;
                std::vector<Plane::Identifier>UpdatedPlanes;
                std::vector<Plane::Identifier>RemovedPlanes;
                std::vector<Mesh::Identifier>UpdatedMeshes;
                std::vector<Mesh::Identifier>RemovedMeshes;

                bool IsTracking;

                Frame(System::Session::Impl&);
                ~Frame();

                void GetHitTestResults(std::vector<HitResult>&, Ray, HitTestTrackableType) const;
                Anchor CreateAnchor(Pose, NativeAnchorPtr) const;
                void UpdateAnchor(Anchor&) const;
                void DeleteAnchor(Anchor&) const;
                SceneObject& GetSceneObjectByID(SceneObject::Identifier) const;
                Plane& GetPlaneByID(Plane::Identifier) const;
                Mesh& GetMeshByID(Mesh::Identifier) const;

            private:
                struct Impl;
                std::unique_ptr<Impl> m_impl{};
            };

            static arcana::task<std::shared_ptr<Session>, std::exception_ptr> CreateAsync(System& system, void* graphicsDevice, std::function<void*()> windowProvider);
            ~Session();

            // Do not use, call CreateAsync instead. Kept public to keep compatibility with make_shared.
            // Move to private when changing to unique_ptr.
            Session(System& system, void* graphicsDevice, std::function<void*()> windowProvider);

            std::unique_ptr<Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession, std::function<void(void* texturePointer)> deletedTextureCallback = [](void*){});
            void RequestEndSession();
            Size GetWidthAndHeightForViewIndex(size_t viewIndex) const;
            void SetDepthsNearFar(float depthNear, float depthFar);
            void SetPlaneDetectionEnabled(bool enabled) const;
            bool TrySetFeaturePointCloudEnabled(bool enabled) const;

            bool TrySetPreferredPlaneDetectorOptions(const GeometryDetectorOptions& options);
            bool TrySetMeshDetectorEnabled(const bool enabled);
            bool TrySetPreferredMeshDetectorOptions(const GeometryDetectorOptions& options);

        private:
            std::unique_ptr<Impl> m_impl{};
        };

        System(const char* = "OpenXR Experience");
        ~System();

        bool IsInitialized() const;
        bool TryInitialize();
        static arcana::task<bool, std::exception_ptr> IsSessionSupportedAsync(SessionType);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
