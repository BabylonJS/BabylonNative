#define NOMINMAX // avoid including macro max definitions in favor of limit max definitions

#include "SceneUnderstanding.h"
#include "XrRegistry.h"

using namespace xr;

constexpr SceneUnderstanding::DetectionBoundaryType DEFAULT_BOUNDARY_TYPE = SceneUnderstanding::DetectionBoundaryType::Sphere;
constexpr float DEFAULT_SPHERE_RADIUS = 5.f;
constexpr double DEFAULT_UPDATE_INTERVAL_IN_SECONDS = 2;
constexpr double NANOSECONDS_IN_SECOND = 1000000000;

inline xr::Pose XrPoseToBabylonPose(XrPosef pose)
{
    // Right handed to left handed conversions are handled in the Babylon.js layer
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

SceneUnderstanding::SceneUnderstanding::InitOptions::InitOptions(
    const XrSession& session,
    const XrSupportedExtensions& extensions,
    const SceneUnderstanding::DetectionBoundaryType detectionBoundaryType,
    const float sphereRadius,
    const XrVector3f boxDimensions,
    const double updateIntervalInSeconds)
    : Session(session)
    , Extensions(extensions)
    , DetectionBoundaryType(detectionBoundaryType)
    , SphereRadius(sphereRadius)
    , BoxDimensions(boxDimensions)
    , UpdateIntervalInSeconds(updateIntervalInSeconds) {}

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

SceneUnderstanding::SceneUnderstanding::UpdateSceneObjectsArgs::UpdateSceneObjectsArgs(
    const XrSpace& sceneSpace,
    const XrSupportedExtensions& extensions,
    const XrTime displayTime,
    std::map<XrSceneObjectKeyMSFT, std::shared_ptr<SceneObject>>& sceneObjects,
    std::vector<XrSceneObjectKeyMSFT>& updatedObjects,
    std::vector<XrSceneObjectKeyMSFT>& removedObjects)
    : SceneSpace(sceneSpace)
    , Extensions(extensions)
    , DisplayTime(displayTime)
    , SceneObjects(sceneObjects)
    , UpdatedObjects(updatedObjects)
    , RemovedObjects(removedObjects)
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
        m_updateInterval = static_cast<XrTime>(NANOSECONDS_IN_SECOND * options.UpdateIntervalInSeconds);

        if (m_initialized)
        {
            // Unclear whether recreating the XrSceneObserverMSFT is needed
            return;
        }

        XrReferenceSpaceCreateInfo spaceCreateInfo{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
        spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        spaceCreateInfo.poseInReferenceSpace = xr::math::Pose::Identity();
        CHECK_XRCMD(xrCreateReferenceSpace(session, &spaceCreateInfo, m_viewSpace.Put()));

        m_sceneObserverHandle.Reset();
        m_sceneObserverHandle = xr::CreateSceneObserver(extensions, session);

        m_initialized = true;
    }

    void UpdateFrame(UpdateFrameArgs& args)
    {
        Update(args.DisplayTime, args.SceneSpace, args.Extensions);
        PopulateFrameArguments(args);
    }

    void UpdateSceneObjects(UpdateSceneObjectsArgs& args)
    {
        Update(args.DisplayTime, args.SceneSpace, args.Extensions);
        PopulateSceneObjectsArgs(args);
    }

    void Update(const XrTime time, const XrSpace& space, const XrSupportedExtensions& extensions)
    {
        if (!m_initialized)
        {
            return;
        }

        if (m_lastFrameUpdate < time)
        {
            m_updatedObjects.clear();
            m_updatedMeshes.clear();
            m_updatedPlanes.clear();
            m_removedObjects.clear();
            m_removedMeshes.clear();
            m_removedPlanes.clear();

            switch (m_state)
            {
            case State::Ready:
            {
                HandleReady(time, space, extensions);
            }
            break;
            case State::ComputingScene:
            {
                HandleComputingScene(time, space, extensions);
            }
            break;
            case State::Waiting:
            {
                HandleWaiting(time);
            }
            break;
            }

            m_lastFrameUpdate = time;
        }
    }

    void HandleReady(const XrTime time, const XrSpace& space, const XrSupportedExtensions& extensions)
    {
        // TODO: support other boundary types
        XrSceneSphereBoundMSFT sphere{};
        XrSpaceLocation viewInLocal{ XR_TYPE_SPACE_LOCATION };
        CHECK_XRCMD(xrLocateSpace(m_viewSpace.Get(), space, time, &viewInLocal));
        if (xr::math::Pose::IsPoseValid(viewInLocal)) {
            sphere.center = viewInLocal.pose.position;
            sphere.radius = m_sphereRadius;
        }

        XrNewSceneComputeInfoMSFT computeInfo{ XR_TYPE_NEW_SCENE_COMPUTE_INFO_MSFT };
        computeInfo.bounds.space = space;
        computeInfo.bounds.time = time;
        computeInfo.bounds.sphereCount = 1;
        computeInfo.bounds.spheres = &sphere;

        CHECK_XRCMD(extensions.xrComputeNewSceneMSFT(m_sceneObserverHandle.Get(), &computeInfo));

        m_lastSceneUpdate = time;
        m_state = State::ComputingScene;
    }

    void HandleComputingScene(const XrTime time, const XrSpace& space, const XrSupportedExtensions& extensions)
    {
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

            XrSceneObjectsLocateInfoMSFT locateInfo{ XR_TYPE_SCENE_OBJECTS_LOCATE_INFO_MSFT };
            locateInfo.baseSpace = space;
            locateInfo.time = time;
            locateInfo.sceneObjectCount = static_cast<uint32_t>(sceneObjects.size());
            locateInfo.sceneObjectKeys = sceneObjectKeys.data();

            m_sceneObjectLocations.resize(sceneObjects.size());
            XrSceneObjectLocationsMSFT locations{ XR_TYPE_SCENE_OBJECT_LOCATIONS_MSFT };
            locations.sceneObjectCount = static_cast<uint32_t>(m_sceneObjectLocations.size());
            locations.sceneObjectLocations = m_sceneObjectLocations.data();

            CHECK_XRCMD(extensions.xrLocateSceneObjectsMSFT(m_sceneHandle.Get(), &locateInfo, &locations));

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

                // TODO: see if there's a better locatino to call this
                XrSceneObjectPropertiesMSFT properties{XR_TYPE_SCENE_OBJECT_PROPERTIES_MSFT};
                XrSceneObjectKindMSFT kind{XR_TYPE_SCENE_OBJECT_KIND_MSFT};
                xr::InsertExtensionStruct(properties, kind);
                XrSceneObjectPropertiesGetInfoMSFT getInfo{XR_TYPE_SCENE_OBJECT_PROPERTIES_GET_INFO_MSFT};
                getInfo.sceneObjectKey = sceneObject.sceneObjectKey;
                CHECK_XRCMD(extensions.xrGetSceneObjectPropertiesMSFT(m_sceneHandle.Get(), &getInfo, &properties));
                sceneData.Kind = kind.kind;

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

    void HandleWaiting(const XrTime time)
    {
        if (time - m_lastSceneUpdate > m_updateInterval)
        {
            m_state = State::Ready;
        }
    }

    void PopulateFrameArguments(UpdateFrameArgs& args)
    {
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
            babylonPlane.PolygonFormat = xr::PolygonFormat::XYZ;

            constexpr uint8_t VALUES_IN_POINT = 3;
            constexpr uint8_t VALUES_IN_XYZ_QUAD = 12;
            babylonPlane.Polygon.resize(VALUES_IN_XYZ_QUAD);
            babylonPlane.Polygon[0] = -1.f * xrPlane.extent.width / 2.f;
            babylonPlane.Polygon[1] = -1.f * xrPlane.extent.height / 2.f;
            babylonPlane.Polygon[2] = 0;
            babylonPlane.Polygon[3] = 1.f * xrPlane.extent.width / 2.f;
            babylonPlane.Polygon[4] = -1.f * xrPlane.extent.height / 2.f;
            babylonPlane.Polygon[5] = 0;
            babylonPlane.Polygon[6] = 1.f * xrPlane.extent.width / 2.f;
            babylonPlane.Polygon[7] = 1.f * xrPlane.extent.height / 2.f;
            babylonPlane.Polygon[8] = 0;
            babylonPlane.Polygon[9] = -1.f * xrPlane.extent.width / 2.f;
            babylonPlane.Polygon[10] = 1.f * xrPlane.extent.height / 2.f;
            babylonPlane.Polygon[11] = 0;
            babylonPlane.PolygonSize = babylonPlane.Polygon.size() / VALUES_IN_POINT; // Number of points

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

    void PopulateSceneObjectsArgs(UpdateSceneObjectsArgs& args)
    {
        if (args.DisplayTime != m_lastSceneObjectUpdate)
        {
            // TODO: reduce amount of stored data
            m_babylonObjects.clear();
            for (const auto& [key, objectData] : m_objects)
            {
                auto object = std::make_shared<SceneObject>();
                object->Key = key;
                object->Kind = objectData.Kind;

                object->Meshes.resize(objectData.MeshKeys.size());
                for (size_t i = 0; i < objectData.MeshKeys.size(); i++)
                {
                    const auto meshKey = objectData.MeshKeys.at(i);
                    object->Meshes.at(i) = m_meshes.at(meshKey);
                }

                object->Planes.resize(objectData.PlaneKeys.size());
                for (size_t i = 0; i < objectData.PlaneKeys.size(); i++)
                {
                    const auto planeKey = objectData.PlaneKeys.at(i);
                    object->Planes.at(i) = m_planes.at(planeKey);
                }

                m_babylonObjects[key] = object;
            }
        }

        args.SceneObjects.clear();
        for (const auto& [key, babylonObject] : m_babylonObjects)
        {
            args.SceneObjects[key] = babylonObject;
        }

        args.UpdatedObjects = m_updatedObjects;
        args.RemovedObjects = m_removedObjects;
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
        XrSceneObjectKindTypeMSFT Kind;
        std::vector<XrSceneMeshKeyMSFT> MeshKeys;
        std::vector<XrScenePlaneKeyMSFT> PlaneKeys;
        XrPosef Pose;
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
    XrTime m_lastSceneObjectUpdate{};
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
    std::map<XrSceneObjectKeyMSFT, std::shared_ptr<SceneObject>> m_babylonObjects{};
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

void SceneUnderstanding::UpdateSceneObjects(UpdateSceneObjectsArgs args) const
{
    m_impl->UpdateSceneObjects(args);
}

System::Session::Frame::Plane& SceneUnderstanding::TryGetPlaneByID(const System::Session::Frame::Plane::Identifier id) const
{
    return m_impl->TryGetPlaneByID(id);
}