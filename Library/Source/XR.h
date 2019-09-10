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
                struct Position
                {
                    float X{};
                    float Y{};
                    float Z{};
                };

                struct Orientation
                {
                    float X{};
                    float Y{};
                    float Z{};
                    float W{};
                };

                struct FieldOfView
                {
                    float AngleLeft{};
                    float AngleRight{};
                    float AngleUp{};
                    float AngleDown{};
                };

                struct View
                {
                    std::vector<Position> Positions{};
                    std::vector<Orientation> Orientations{};
                    std::vector<FieldOfView> FieldsOfView{};

                    uint64_t ColorTextureFormat{};
                    void* ColorTexturePointer{};

                    uint64_t DepthTextureFormat{};
                    void* DepthTexturePointer{};
                };

                XrFrame(HeadMountedDisplay::Session::Impl&);
                ~XrFrame();

                std::vector<View> Views{};

            private:
                Session::Impl& m_sessionImpl;
                bool m_shouldRender{};
                int64_t m_displayTime{};
            };

            ~Session();

            std::unique_ptr<XrFrame> GetNextFrame();

        private:
            std::unique_ptr<Impl> m_impl{};

            Session(HeadMountedDisplay& headMountedDisplay, void* graphicsDevice);
        };

        HeadMountedDisplay();
        ~HeadMountedDisplay();

        bool TryInitialize();

        std::unique_ptr<Session> CreateSession(void* graphicsDevice);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}