#pragma once

#include "XrSupportedExtensions.h"
#include "XrSceneUnderstanding.h"

namespace xr
{
    struct SceneUnderstanding
    {
    public:
        enum DetectionBoundaryType : uint8_t
        {
            Sphere,
            Box
        };

        struct InitOptions
        {
            const XrSession& Session;
            const XrSupportedExtensions& Extensions;
            const DetectionBoundaryType DetectionBoundaryType;
            const float SphereRadius;
            const XrVector3f BoxDimensions;
            const double UpdateIntervalInSeconds;

            InitOptions(
                const XrSession& session,
                const XrSupportedExtensions& extensions);

            InitOptions(
                const XrSession& session,
                const XrSupportedExtensions& extensions,
                const SceneUnderstanding::DetectionBoundaryType detectionBoundaryType,
                const float sphereRadius,
                const XrVector3f boxDimensions,
                const double updateIntervalInSeconds);
        };

        struct UpdateFrameArgs
        {
            const XrSpace& SceneSpace;
            const XrSupportedExtensions& Extensions;
            const XrTime DisplayTime;
            std::vector<System::Session::Frame::Plane>& Planes;
            std::vector<System::Session::Frame::Plane::Identifier>& UpdatedPlanes;
            std::vector<System::Session::Frame::Plane::Identifier>& RemovedPlanes;

            UpdateFrameArgs(
                const XrSpace& sceneSpace,
                const XrSupportedExtensions& extensions,
                const XrTime displayTime,
                std::vector<System::Session::Frame::Plane>& planes,
                std::vector<System::Session::Frame::Plane::Identifier>& updatedPlanes,
                std::vector<System::Session::Frame::Plane::Identifier>& removedPlanes);
        };

        struct Mesh : SceneMesh
        {
            XrSceneMeshKeyMSFT key;
            XrSceneObjectKeyMSFT parentObjectKey;
        };

        struct Plane : ScenePlane
        {
            XrScenePlaneKeyMSFT key;
            XrSceneObjectKeyMSFT parentObjectKey;
        };

        struct SceneObject
        {
            XrSceneObjectKeyMSFT Key;
            XrSceneObjectKindTypeMSFT Kind;
            std::vector<Mesh> Meshes;
            std::vector<Plane> Planes;
            XrPosef Pose;
        };

        struct UpdateSceneObjectsArgs
        {
            const XrSpace& SceneSpace;
            const XrSupportedExtensions& Extensions;
            const XrTime DisplayTime;
            std::map<XrSceneObjectKeyMSFT, std::shared_ptr<SceneObject>>& SceneObjects;
            std::vector<XrSceneObjectKeyMSFT>& UpdatedObjects;
            std::vector<XrSceneObjectKeyMSFT>& RemovedObjects;

            UpdateSceneObjectsArgs(
                const XrSpace& sceneSpace,
                const XrSupportedExtensions& extensions,
                const XrTime displayTime,
                std::map<XrSceneObjectKeyMSFT, std::shared_ptr<SceneObject>>& sceneObjects,
                std::vector<XrSceneObjectKeyMSFT>& updatedObjects,
                std::vector<XrSceneObjectKeyMSFT>& removedObjects);
        };

        SceneUnderstanding();
        ~SceneUnderstanding();
        void Initialize(const InitOptions options) const;
        void UpdateFrame(UpdateFrameArgs args) const;
        void UpdateSceneObjects(UpdateSceneObjectsArgs args) const;
        System::Session::Frame::Plane& TryGetPlaneByID(const System::Session::Frame::Plane::Identifier id) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
}