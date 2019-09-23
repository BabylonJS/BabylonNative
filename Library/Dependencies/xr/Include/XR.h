#pragma once

#include <memory>
#include <vector>

namespace xr
{
    enum TextureFormat
    {
        RGBA8,
        D24S8
    };

    class System
    {
    public:
        class Session
        {
            friend class System;
            struct Impl;

        public:
            struct Frame
            {
                struct View
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
                bool ShouldEndSession{};
                bool ShouldRestartSession{};

                Frame(System::Session::Impl&, bool, bool);
                ~Frame();

            private:
                Session::Impl& m_sessionImpl;
                bool m_shouldRender{};
                int64_t m_displayTime{};
            };

            ~Session();

            Session(Session&) = delete;
            Session& operator=(Session&&) = delete;

            std::unique_ptr<Frame> GetNextFrame();
            void RequestEndSession();

        private:
            std::unique_ptr<Impl> m_impl{};

            Session(System& headMountedDisplay, void* graphicsDevice);
        };

        System();
        ~System();

        System(System&) = delete;
        System& operator=(System&&) = delete;

        bool IsInitialized() const;
        bool TryInitialize();

        std::unique_ptr<Session> CreateSession(void* graphicsDevice);

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}