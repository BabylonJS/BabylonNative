#pragma once

#include <gsl/gsl>

#include <functional>
#include <memory>
#include <vector>

namespace babylon
{
    // Attempts to store the instance, system, and related non-session state for an HMD.
    class HeadMountedDisplay
    {
    public:
        class Session
        {
            friend class HeadMountedDisplay;
            struct Impl;

        public:
            struct XrFrame
            {
                struct View
                {
                    enum TextureFormat
                    {
                        RGBA8,
                        D24S8
                    };

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

                    struct
                    {
                        float AngleLeft{};
                        float AngleRight{};
                        float AngleUp{};
                        float AngleDown{};
                    } FieldOfView;

                    TextureFormat ColorTextureFormat{};
                    void* ColorTexturePointer{};
                    struct
                    {
                        size_t Width{};
                        size_t Height{};
                    } ColorTextureSize;

                    TextureFormat DepthTextureFormat{};
                    void* DepthTexturePointer{};
                    struct
                    {
                        size_t Width{};
                        size_t Height{};
                    } DepthTextureSize;
                };

                std::vector<View> Views{};

                XrFrame(HeadMountedDisplay::Session::Impl&);
                ~XrFrame();

            private:
                Session::Impl& m_sessionImpl;
                bool m_shouldRender{};
                int64_t m_displayTime{};
            };

            ~Session();

            Session(Session&) = delete;
            Session& operator=(Session&&) = delete;

            std::unique_ptr<XrFrame> GetNextFrame();
            void RequestEndSession();

        private:
            std::unique_ptr<Impl> m_impl{};

            Session(HeadMountedDisplay& headMountedDisplay, void* graphicsDevice);
        };

        HeadMountedDisplay();
        ~HeadMountedDisplay();

        HeadMountedDisplay(HeadMountedDisplay&) = delete;
        HeadMountedDisplay& operator=(HeadMountedDisplay&&) = delete;

        bool IsInitialized() const;
        bool TryInitialize();

        std::unique_ptr<Session> CreateSession(void* graphicsDevice);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}