#pragma once

#include "XrSupportedExtensions.h"
#include "XrRegistry.h"

namespace xr 
{
    struct XrInput
    {
    public:
        struct InitOptions
        {
            const XrInstance Instance;
            const XrSession Session;
            const XrSupportedExtensions& Extensions;
            XrSystemHandTrackingPropertiesEXT& HandTrackingInteractionProps;
            XrSystemEyeGazeInteractionPropertiesEXT& EyeGazeInteractionProps;
        };

        struct UpdateFrameArgs
        {
            const XrInstance Instance;
            const XrSession Session;
            const XrSpace SceneSpace;
            const XrTime DisplayTime;
            const XrSupportedExtensions& Extensions;
            std::vector<System::Session::Frame::InputSource>& activeInputSources;
            std::optional<Space>& eyeTrackerSpace;
        };

        XrInput();
        ~XrInput();
        void Initialize(const InitOptions& args) const;
        void UpdateFrame(const UpdateFrameArgs& args) const;
        void RefreshInputSources(
            const XrInstance instance,
            const XrSession session,
            std::vector<System::Session::Frame::InputSource>& activeInputSources) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
}