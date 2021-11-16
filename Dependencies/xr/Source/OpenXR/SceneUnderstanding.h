#pragma once

#include "XrSupportedExtensions.h"
#include "XrUtility/XrSceneUnderstanding.h"

namespace xr
{
    struct SceneUnderstanding
    {
    public:
        struct InitOptions
        {
            const XrSession& Session;
            const XrSupportedExtensions& Extensions;
            const xr::DetectionBoundary DetectionBoundary;
            const double UpdateIntervalInSeconds;

            InitOptions(
                const XrSession& session,
                const XrSupportedExtensions& extensions);

            InitOptions(
                const XrSession& session,
                const XrSupportedExtensions& extensions,
                const xr::DetectionBoundary detectionBoundary,
                const double updateIntervalInSeconds);
        };

        struct UpdateFrameArgs
        {
            const XrSpace& SceneSpace;
            const XrSupportedExtensions& Extensions;
            const XrTime DisplayTime;
            std::vector<System::Session::Frame::SceneObject::Identifier>& UpdatedSceneObjects;
            std::vector<System::Session::Frame::SceneObject::Identifier>& RemovedSceneObjects;
            std::vector<System::Session::Frame::Plane::Identifier>& UpdatedPlanes;
            std::vector<System::Session::Frame::Plane::Identifier>& RemovedPlanes;
            std::vector<System::Session::Frame::Mesh::Identifier>& UpdatedMeshes;
            std::vector<System::Session::Frame::Mesh::Identifier>& RemovedMeshes;
            std::vector<System::Session::Frame::ImageTrackingResult::Identifier>UpdatedImageTrackingResults;
            std::vector<System::Session::Frame::ImageTrackingResult::Identifier>RemovedImageTrackingResults;
        };

        SceneUnderstanding();
        ~SceneUnderstanding();
        void Initialize(const InitOptions& options) const;
        void UpdateFrame(UpdateFrameArgs& args) const;
        System::Session::Frame::SceneObject& GetSceneObjectByID(const System::Session::Frame::SceneObject::Identifier id) const;
        System::Session::Frame::Plane& GetPlaneByID(const System::Session::Frame::Plane::Identifier id) const;
        System::Session::Frame::Mesh& GetMeshByID(const System::Session::Frame::Mesh::Identifier id) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
}