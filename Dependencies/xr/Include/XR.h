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

    struct Size
    {
        size_t Width{};
        size_t Height{};
    };

    struct Pose
    {
        struct
        {
            float X{};
            float Y{};
            float Z{};
        } Position;

        struct
        {
            float X{};
            float Y{};
            float Z{};
            float W{};
        } Orientation;
    };

    using NativeTrackablePtr = void*;
    struct HitResult
    {
        Pose Pose{};
        NativeTrackablePtr NativeTrackable{};
    };

    struct Ray
    {
        struct
        {
            float X{};
            float Y{};
            float Z{};
        } Origin;

        struct
        {
            float X{};
            float Y{};
            float Z{};
        } Direction;
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

    class System
    {
    public:
        static constexpr float DEFAULT_DEPTH_NEAR_Z{ 0.5f };
        static constexpr float DEFAULT_DEPTH_FAR_Z{ 1000.f };

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

                struct View
                {
                    Space Space{};

                    struct
                    {
                        float AngleLeft{};
                        float AngleRight{};
                        float AngleUp{};
                        float AngleDown{};
                    } FieldOfView;

                    TextureFormat ColorTextureFormat{};
                    void* ColorTexturePointer{};
                    Size ColorTextureSize;

                    TextureFormat DepthTextureFormat{};
                    void* DepthTexturePointer{};
                    Size DepthTextureSize;

                    float DepthNearZ{};
                    float DepthFarZ{};
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
                    Space GripSpace{};
                    Space AimSpace{};
                    HandednessEnum Handedness{};

                private:
                    static inline Identifier NEXT_ID{ 0 };
                };

                struct Plane
                {
                    using Identifier = size_t;
                    const Identifier ID{ NEXT_ID++ };
                    Pose Center{};
                    std::vector<float> Polygon{};
                    size_t PolygonSize{0};
                    PolygonFormat PolygonFormat{};
                
                private:
                    static inline Identifier NEXT_ID{0};
                };

                std::vector<View>& Views;
                std::vector<InputSource>& InputSources;
                std::vector<Plane>& Planes;
                std::vector<FeaturePoint>& FeaturePointCloud;
                
                std::vector<Plane::Identifier>UpdatedPlanes;
                std::vector<Plane::Identifier>RemovedPlanes;

                Frame(System::Session::Impl&);
                ~Frame();

                void GetHitTestResults(std::vector<HitResult>&, Ray) const;
                Anchor CreateAnchor(Pose, NativeAnchorPtr) const;
                void UpdateAnchor(Anchor&) const;
                void DeleteAnchor(Anchor&) const;
                Plane& GetPlaneByID(Plane::Identifier) const;

            private:
                struct Impl;
                std::unique_ptr<Impl> m_impl{};
            };

            static arcana::task<std::shared_ptr<Session>, std::exception_ptr> CreateAsync(System& system, void* graphicsDevice, void* window);
            ~Session();

            // Do not use, call CreateAsync instead. Kept public to keep compatibility with make_shared.
            // Move to private when changing to unique_ptr.
            Session(System& system, void* graphicsDevice, void* window);

            std::unique_ptr<Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession, std::function<void(void* texturePointer)> deletedTextureCallback = [](void*){});
            void RequestEndSession();
            Size GetWidthAndHeightForViewIndex(size_t viewIndex) const;
            void SetDepthsNearFar(float depthNear, float depthFar);
            void SetPlaneDetectionEnabled(bool enabled) const;
            bool TrySetFeaturePointCloudEnabled(bool enabled) const;

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
