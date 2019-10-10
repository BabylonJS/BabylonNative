#pragma once

#include <memory>
#include <string>
#include <vector>

namespace xr
{
    class Exception final : public std::exception
    {
    public:
        Exception::Exception(const char* message);
        const char* Exception::what() const noexcept;

    private:
        std::string m_message{};
    };

    enum TextureFormat
    {
        RGBA8,
        D24S8
    };

    class System
    {
    public:
        // These values are taken from Microsoft's OpenXR sample and are used in conjunction with the depth extension. Values are reversed, as in example.
        // https://github.com/microsoft/OpenXR-SDK-VisualStudio/blob/42172676a9388f02ce5a776c89a62cecf95429bd/samples/BasicXrApp/OpenXrProgram.cpp#L226
        constexpr static float DEPTH_NEAR_Z{ 0.5f };
        constexpr static float DEPTH_FAR_Z{ 1000.f };

        class Session
        {
            friend class System;
            class Impl;

        public:
            class Frame
            {
            public:
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

                std::vector<View>& Views;

                Frame(System::Session::Impl&);
                ~Frame();

            private:
                Session::Impl& m_sessionImpl;
                bool m_shouldRender{};
                int64_t m_displayTime{};
            };

            Session(System& headMountedDisplay, void* graphicsDevice);
            ~Session();

            Session(Session&) = delete;
            Session& operator=(Session&&) = delete;

            std::unique_ptr<Frame> GetNextFrame(bool& shouldEndSession, bool& shouldRestartSession);
            void RequestEndSession();
            std::pair<size_t, size_t> GetWidthAndHeightForViewIndex(size_t viewIndex) const;

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
