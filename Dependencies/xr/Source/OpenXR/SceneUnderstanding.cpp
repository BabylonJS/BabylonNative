#define NOMINMAX // avoid including macro max definitions in favor of limit max definitions

#include "SceneUnderstanding.h"
#include "XrRegistry.h"
#include "XrSceneUnderstanding.h"

using namespace xr;

constexpr SceneUnderstanding::DetectionBoundaryType DEFAULT_BOUNDARY_TYPE = SceneUnderstanding::DetectionBoundaryType::Sphere;
constexpr float DEFAULT_SPHERE_RADIUS = 5.f;
constexpr XrTime DEFAULT_UPDATE_INTERVAL_IN_SECONDS = 2;
constexpr XrTime NANOSECONDS_IN_SECOND = 1000000000;

inline xr::Pose XrPoseToBabylonPose(XrPosef pose)
{
    // TODO: Fix coordinate systems?
    return xr::Pose{
        {
            pose.position.x,
            pose.position.y,
            pose.position.z,
        },
        {
            pose.orientation.x,
            pose.orientation.y,
            pose.orientation.z,
            pose.orientation.w
        }};
}

SceneUnderstanding::SceneUnderstanding::InitOptions::InitOptions(
    const XrSession& session,
    const XrSupportedExtensions& extensions)
    : Session(session)
    , Extensions(extensions)
    , DetectionBoundaryType(DEFAULT_BOUNDARY_TYPE)
    , SphereRadius(DEFAULT_SPHERE_RADIUS)
    , BoxDimensions(XrVector3f{ 0, 0, 0 })
    , UpdateIntervalInSeconds(DEFAULT_UPDATE_INTERVAL_IN_SECONDS) {}

SceneUnderstanding::SceneUnderstanding::UpdateFrameArgs::UpdateFrameArgs(
    const XrSpace& sceneSpace,
    const XrSupportedExtensions& extensions,
    const XrTime displayTime,
    std::vector<System::Session::Frame::Plane>& planes,
    std::vector<System::Session::Frame::Plane::Identifier>& updatedPlanes,
    std::vector<System::Session::Frame::Plane::Identifier>& removedPlanes)
    : SceneSpace(sceneSpace)
    , Extensions(extensions)
    , DisplayTime(displayTime)
    , Planes(planes)
    , UpdatedPlanes(updatedPlanes)
    , RemovedPlanes(removedPlanes)
{
}

struct SceneUnderstanding::SceneUnderstanding::Impl
{
public:
    Impl() {}
    void Initialize(const InitOptions& options)
    {
        const auto& session = options.Session;
        const auto& extensions = options.Extensions;
        if (!extensions.SceneUnderstandingSupported)
        {
            return;
        }

        // Default to most recently provided detection boundary information
        m_boundaryType = options.DetectionBoundaryType;
        m_sphereRadius = options.SphereRadius;
        m_boxDimensions = options.BoxDimensions;
        m_updateInterval = NANOSECONDS_IN_SECOND * options.UpdateIntervalInSeconds;

        if (m_initialized)
        {
            // Unclear whether recreating the XrSceneObserverMSFT is needed
            return;
        }

        XrReferenceSpaceCreateInfo spaceCreateInfo{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        spaceCreateInfo.poseInReferenceSpace = xr::math::Pose::Identity();
        CHECK_XRCMD(xrCreateReferenceSpace(session, &spaceCreateInfo, m_viewSpace.Put()));

        m_sceneObserverHandle.Reset();
        m_sceneObserverHandle = xr::CreateSceneObserver(extensions, session);

        m_initialized = true;
    }

    void UpdateFrame(UpdateFrameArgs& args)
    {
        const auto time = args.DisplayTime;

        if (!m_initialized)
        {
            return;
        }

        if (m_lastFrameUpdate < time)
        {
            // Update every frame
            m_updatedObjects.clear();
            m_updatedMeshes.clear();
            m_updatedPlanes.clear();
            m_removedObjects.clear();
            m_removedMeshes.clear();
            m_removedPlanes.clear();
        }

        switch (m_state)
        {
        case State::Ready:
        {
            HandleReady(args);
        }
        break;
        case State::ComputingScene:
        {
            HandleComputingScene(args);
        }
        break;
        case State::Waiting:
        {
            HandleWaiting(args);
        }
        break;
        }

        PopulateFrameArguments(args);
        m_lastFrameUpdate = time;
    }

    void HandleReady(UpdateFrameArgs& args)
    {
        const auto& extensions = args.Extensions;
        const auto time = args.DisplayTime;
        const auto space = args.SceneSpace;

        // TODO: support other boundary types
        XrSceneSphereBoundMSFT sphere{};
        XrSpaceLocation viewInLocal{ XR_TYPE_SPACE_LOCATION };
        CHECK_XRCMD(xrLocateSpace(m_viewSpace.Get(), space, time, &viewInLocal));
        if (xr::math::Pose::IsPoseValid(viewInLocal)) {
            sphere.center = viewInLocal.pose.position;
            sphere.radius = m_sphereRadius;
        }

        XrNewSceneComputeInfoMSFT computeInfo{XR_TYPE_NEW_SCENE_COMPUTE_INFO_MSFT};
        computeInfo.bounds.space = space;
        computeInfo.bounds.time = time;
        computeInfo.bounds.sphereCount = 1;
        computeInfo.bounds.spheres = &sphere;

        CHECK_XRCMD(extensions.xrComputeNewSceneMSFT(m_sceneObserverHandle.Get(), &computeInfo));

        m_lastSceneUpdate = time;
        m_state = State::ComputingScene;
    }

    void HandleComputingScene(UpdateFrameArgs& args)
    {
        const auto& extensions = args.Extensions;

        XrSceneComputeStateMSFT state{};
        CHECK_XRCMD(extensions.xrGetSceneComputeStateMSFT(m_sceneObserverHandle.Get(), &state));
        if (state == XR_SCENE_COMPUTE_STATE_COMPLETED_MSFT) {
            m_sceneHandle = xr::CreateScene(extensions, m_sceneObserverHandle.Get());

            const auto sceneObjects = xr::GetSceneObjects(extensions, m_sceneHandle.Get());

            std::vector<XrSceneObjectKeyMSFT> sceneObjectKeys;
            sceneObjectKeys.resize(sceneObjects.size());
            for (size_t i = 0; i < sceneObjectKeys.size(); i++)
            {
                sceneObjectKeys.at(i) = sceneObjects.at(i).sceneObjectKey;
            }

            XrSceneObjectsLocateInfoMSFT locateInfo{XR_TYPE_SCENE_OBJECTS_LOCATE_INFO_MSFT};
            locateInfo.baseSpace = args.SceneSpace;
            locateInfo.time = args.DisplayTime;
            locateInfo.sceneObjectCount = static_cast<uint32_t>(sceneObjects.size());
            locateInfo.sceneObjectKeys = sceneObjectKeys.data();

            m_sceneObjectLocations.resize(sceneObjects.size());
            XrSceneObjectLocationsMSFT locations{XR_TYPE_SCENE_OBJECT_LOCATIONS_MSFT};
            locations.sceneObjectCount = static_cast<uint32_t>(m_sceneObjectLocations.size());
            locations.sceneObjectLocations = m_sceneObjectLocations.data();

            CHECK_XRCMD(args.Extensions.xrLocateSceneObjectsMSFT(m_sceneHandle.Get(), &locateInfo, &locations));

            for (size_t i = 0; i < sceneObjects.size(); i++)
            {
                const auto& location = m_sceneObjectLocations.at(i);
                const auto& sceneObject = sceneObjects.at(i);

                if (xr::math::Pose::IsPoseValid(location.locationFlags)) {
                    if (m_objects.count(sceneObject.sceneObjectKey) == 0)
                    {
                        m_updatedObjects.push_back(sceneObject.sceneObjectKey);
                        m_objects[sceneObject.sceneObjectKey] = SceneObjectData{};
                    }

                    m_objects[sceneObject.sceneObjectKey].Pose = location.pose;
                }
            }

            std::set<XrSceneObjectKeyMSFT> knownObjects{};
            std::transform(m_objects.begin(), m_objects.end(),
                std::inserter(knownObjects, knownObjects.end()),
                [](auto pair) { return pair.first; });

            std::set<XrSceneMeshKeyMSFT> knownMeshes{};
            std::transform(m_meshes.begin(), m_meshes.end(),
                std::inserter(knownMeshes, knownMeshes.end()),
                [](auto pair) { return pair.first; });

            std::set<XrScenePlaneKeyMSFT> knownPlanes{};
            std::transform(m_planes.begin(), m_planes.end(),
                std::inserter(knownPlanes, knownPlanes.end()),
                [](auto pair) { return pair.first; });

            // m_updated vectors cleared every frame
            for (const auto& sceneObject : sceneObjects)
            {
                if (m_objects.count(sceneObject.sceneObjectKey) == 0)
                {
                    m_updatedObjects.push_back(sceneObject.sceneObjectKey);
                    m_objects[sceneObject.sceneObjectKey] = SceneObjectData{};
                }

                knownObjects.erase(sceneObject.sceneObjectKey);
                auto& sceneData = m_objects[sceneObject.sceneObjectKey];

                sceneData.MeshKeys = xr::GetMeshKeys(extensions, m_sceneHandle.Get(), sceneObject.sceneObjectKey);
                for (const auto& meshKey : sceneData.MeshKeys)
                {
                    if (m_meshes.count(meshKey) == 0)
                    {
                        m_updatedMeshes.push_back(meshKey);
                    }

                    knownMeshes.erase(meshKey);
                    auto sceneMesh = xr::GetSceneMesh(extensions, m_sceneHandle.Get(), meshKey);
                    m_meshes[meshKey] = Mesh{ std::move(sceneMesh), sceneObject.sceneObjectKey };
                }

                sceneData.PlaneKeys = xr::GetPlaneKeys(extensions, m_sceneHandle.Get(), sceneObject.sceneObjectKey);
                for (const auto& planeKey : sceneData.PlaneKeys)
                {
                    if (m_planes.count(planeKey) == 0)
                    {
                        m_updatedPlanes.push_back(planeKey);
                    }

                    knownPlanes.erase(planeKey);
                    auto scenePlane = xr::GetScenePlane(extensions, m_sceneHandle.Get(), planeKey);
                    m_planes[planeKey] = Plane{ std::move(scenePlane), sceneObject.sceneObjectKey };
                }
            }

            // m_removed vectors cleared every frame
            for (const auto& objectKey : knownObjects)
            {
                m_removedObjects.push_back(objectKey);
                m_objects.erase(objectKey);
            }

            for (const auto& meshKey : knownMeshes)
            {
                m_removedMeshes.push_back(meshKey);
                m_meshes.erase(meshKey);
            }

            for (const auto& planeKey : knownPlanes)
            {
                m_removedPlanes.push_back(planeKey);
                m_planes.erase(planeKey);
            }

            m_state = State::Waiting;
        }
    }

    void HandleWaiting(UpdateFrameArgs& args)
    {
        if (args.DisplayTime - m_lastSceneUpdate > m_updateInterval)
        {
            m_state = State::Ready;
        }
    }

    void PopulateFrameArguments(UpdateFrameArgs& args)
    {
        // TODO: avoid recalculating
        args.Planes.clear();
        for (const auto& [key, xrPlane] : m_planes)
        {
            if (m_babylonPlanes.count(key) == 0)
            {
                const auto newPlane = std::make_shared<xr::System::Session::Frame::Plane>();
                m_babylonPlanes[key] = newPlane;
                m_babylonPlanesById[newPlane->ID] = newPlane;
            }

            auto& babylonPlane = *m_babylonPlanes[key];
            babylonPlane.Center = XrPoseToBabylonPose(m_objects.at(xrPlane.parentObjectKey).Pose);
            babylonPlane.PolygonFormat = xr::PolygonFormat::XZ;

            constexpr uint8_t POINTS_IN_XZ_QUAD = 8;
            babylonPlane.Polygon.resize(POINTS_IN_XZ_QUAD);
            babylonPlane.Polygon[0] = -1.f * xrPlane.extent.width / 2.f;
            babylonPlane.Polygon[1] = -1.f * xrPlane.extent.height / 2.f;
            babylonPlane.Polygon[2] = 1.f * xrPlane.extent.width / 2.f;
            babylonPlane.Polygon[3] = -1.f * xrPlane.extent.height / 2.f;
            babylonPlane.Polygon[4] = 1.f * xrPlane.extent.width / 2.f;
            babylonPlane.Polygon[5] = 1.f * xrPlane.extent.height / 2.f;
            babylonPlane.Polygon[6] = -1.f * xrPlane.extent.width / 2.f;
            babylonPlane.Polygon[7] = 1.f * xrPlane.extent.height / 2.f;
            babylonPlane.PolygonSize = babylonPlane.Polygon.size();

            args.Planes.push_back(babylonPlane);
        }

        args.UpdatedPlanes.clear();
        for (const auto xrKey : m_updatedPlanes)
        {
            if (m_babylonPlanes.count(xrKey) > 0)
            {
                const auto babylonID = m_babylonPlanes.at(xrKey)->ID;
                args.UpdatedPlanes.push_back(babylonID);
            }
        }

        args.RemovedPlanes.clear();
        for (const auto xrKey : m_removedPlanes)
        {
            if (m_babylonPlanes.count(xrKey) > 0)
            {
                const auto babylonPlane = m_babylonPlanes.at(xrKey);
                args.RemovedPlanes.push_back(babylonPlane->ID);
                m_babylonPlanes.erase(xrKey);
                m_babylonPlanesById.erase(babylonPlane->ID);
            }
        }
    }

    System::Session::Frame::Plane& TryGetPlaneByID(const System::Session::Frame::Plane::Identifier id)
    {
        if (m_babylonPlanesById.count(id) > 0)
        {
            return *m_babylonPlanesById[id];
        }

        throw std::exception(/*unknown plane id*/);
    }

private:
    enum State : uint8_t
    {
        Ready,
        ComputingScene,
        Waiting
    };

    struct SceneObjectData
    {
        std::vector<XrSceneMeshKeyMSFT> MeshKeys;
        std::vector<XrScenePlaneKeyMSFT> PlaneKeys;
        XrPosef Pose;
    };

    struct Mesh : xr::SceneMesh
    {
        XrSceneObjectKeyMSFT parentObjectKey;
    };

    struct Plane : xr::ScenePlane
    {
        XrSceneObjectKeyMSFT parentObjectKey;
    };

    bool m_initialized{ false };
    xr::SpaceHandle m_viewSpace{};
    SceneUnderstanding::DetectionBoundaryType m_boundaryType{};
    float m_sphereRadius;
    XrVector3f m_boxDimensions{};
    XrTime m_updateInterval;
    xr::SceneObserverHandle m_sceneObserverHandle{};
    XrTime m_lastSceneUpdate{};
    State m_state{ State::Ready };
    xr::SceneHandle m_sceneHandle{};
    XrTime m_lastFrameUpdate{};
    std::vector<XrSceneObjectLocationMSFT> m_sceneObjectLocations;

    std::map<XrSceneObjectKeyMSFT, SceneObjectData> m_objects{};
    std::map<XrSceneMeshKeyMSFT, Mesh> m_meshes{};
    std::map<XrScenePlaneKeyMSFT, Plane> m_planes{};
    std::vector<XrSceneObjectKeyMSFT> m_updatedObjects{};
    std::vector<XrSceneMeshKeyMSFT> m_updatedMeshes{};
    std::vector<XrScenePlaneKeyMSFT> m_updatedPlanes{};
    std::vector<XrSceneObjectKeyMSFT> m_removedObjects{};
    std::vector<XrSceneMeshKeyMSFT> m_removedMeshes{};
    std::vector<XrScenePlaneKeyMSFT> m_removedPlanes{};

    std::map<XrScenePlaneKeyMSFT, std::shared_ptr<xr::System::Session::Frame::Plane>> m_babylonPlanes;
    std::map<xr::System::Session::Frame::Plane::Identifier, std::shared_ptr<xr::System::Session::Frame::Plane>> m_babylonPlanesById;
};

SceneUnderstanding::SceneUnderstanding() 
    : m_impl(std::make_unique<Impl>())
{
}

SceneUnderstanding::~SceneUnderstanding()
{
}

void SceneUnderstanding::Initialize(const InitOptions options) const
{
    m_impl->Initialize(options);
}

void SceneUnderstanding::UpdateFrame(UpdateFrameArgs args) const
{
    m_impl->UpdateFrame(args);
}

System::Session::Frame::Plane& SceneUnderstanding::TryGetPlaneByID(const System::Session::Frame::Plane::Identifier id) const
{
    return m_impl->TryGetPlaneByID(id);
}