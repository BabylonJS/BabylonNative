#pragma once

#include <memory>
#include <string>
#include <vector>

namespace xr
{
    enum class TextureFormat
    {
        RGBA8_SRGB,
        BGRA8_SRGB,
        D24S8
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

    class System
    {
    public:
        static constexpr float DEFAULT_DEPTH_NEAR_Z{ 0.5f };
        static constexpr float DEFAULT_DEPTH_FAR_Z{ 1000.f };

        class Session
        {
            friend class System;
            class Impl;

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

                std::vector<View>& Views;
                std::vector<InputSource>& InputSources;

                Frame(System::Session::Impl&);
                ~Frame();

                void GetHitTestResults(std::vector<Pose>&, Ray) const;

            private:
                class Impl;
                std::unique_ptr<Impl> m_impl{};
            };

            Session(System& system, void* graphicsDevice);
            ~Session();

            Session(Session&) = delete;
            Session& operator=(Session&&) = delete;

            std::unique_ptr<Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession);
            void RequestEndSession();
            Size GetWidthAndHeightForViewIndex(size_t viewIndex) const;
            void SetDepthsNearFar(float depthNear, float depthFar);

        private:
            std::unique_ptr<Impl> m_impl{};
        };

        System(const char* = "OpenXR Experience");
        ~System();

        System(System&) = delete;
        System& operator=(System&&) = delete;

        bool IsInitialized() const;
        bool TryInitialize();

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
