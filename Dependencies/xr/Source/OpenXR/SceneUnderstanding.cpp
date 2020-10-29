#ifndef NOMINMAX
#define NOMINMAX // avoid including macro max definitions in favor of limit max definitions
#endif

#include "SceneUnderstanding.h"
#include "XrRegistry.h"

#pragma region DirectX
#include <winrt/Windows.Foundation.h>
namespace winrt
{
    using namespace winrt::Windows::Foundation;
}

namespace
{
    XrVector3f TransformPoint(const XrVector3f point, const XrPosef transform)
    {
        XrVector3f output{};
        auto transformMatrix = xr::math::LoadXrPose(transform);
        winrt::Numerics::float3 pointData{ point.x, point.y, point.z };
        auto pointVector = DirectX::XMLoadFloat3(&pointData);
        auto outPoint = DirectX::XMVector3Transform(pointVector, transformMatrix);
        return XrVector3f{ DirectX::XMVectorGetX(outPoint), DirectX::XMVectorGetY(outPoint), DirectX::XMVectorGetZ(outPoint) };
    }
}
#pragma endregion DirectX

using namespace xr;

constexpr xr::DetectionBoundaryType DEFAULT_BOUNDARY_TYPE = xr::DetectionBoundaryType::Sphere;
constexpr float DEFAULT_SPHERE_RADIUS = 5.f;
constexpr double DEFAULT_UPDATE_INTERVAL_IN_SECONDS = 10;
constexpr double NANOSECONDS_IN_SECOND = 1000000000;

static inline xr::Pose XrPoseToBabylonPose(XrPosef pose)
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
    , Frustum(xr::Frustum{})
    , BoxDimensions(xr::Vector3f{ 0, 0, 0 })
    , UpdateIntervalInSeconds(DEFAULT_UPDATE_INTERVAL_IN_SECONDS) {}

SceneUnderstanding::SceneUnderstanding::InitOptions::InitOptions(
    const XrSession& session,
    const XrSupportedExtensions& extensions,
    const xr::DetectionBoundaryType detectionBoundaryType,
    const float sphereRadius,
    const xr::Frustum frustum,
    const xr::Vector3f boxDimensions,
    const double updateIntervalInSeconds)
    : Session(session)
    , Extensions(extensions)
    , DetectionBoundaryType(detectionBoundaryType)
    , SphereRadius(sphereRadius)
    , Frustum(frustum)
    , BoxDimensions(boxDimensions)
    , UpdateIntervalInSeconds(updateIntervalInSeconds) {}

SceneUnderstanding::SceneUnderstanding::UpdateFrameArgs::UpdateFrameArgs(
    const XrSpace& sceneSpace,
    const XrSupportedExtensions& extensions,
    const XrTime displayTime,
    std::vector<System::Session::Frame::Plane>& planes,
    std::vector<System::Session::Frame::Plane::Identifier>& updatedPlanes,
    std::vector<System::Session::Frame::Plane::Identifier>& removedPlanes,
    std::vector<System::Session::Frame::Mesh>& meshes,
    std::vector<System::Session::Frame::Mesh::Identifier>& updatedMeshes,
    std::vector<System::Session::Frame::Mesh::Identifier>& removedMeshes)
    : SceneSpace(sceneSpace)
    , Extensions(extensions)
    , DisplayTime(displayTime)
    , Planes(planes)
    , UpdatedPlanes(updatedPlanes)
    , RemovedPlanes(removedPlanes)
    , Meshes(meshes)
    , UpdatedMeshes(updatedMeshes)
    , RemovedMeshes(removedMeshes)
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
        m_frustum = options.Frustum;
        m_boxDimensions = { options.BoxDimensions.X, options.BoxDimensions.Y, options.BoxDimensions.Z };
        m_updateInterval = static_cast<XrTime>(NANOSECONDS_IN_SECOND * options.UpdateIntervalInSeconds);

        if (m_initialized)
        {
            // This function will be repeatedly called when setting new detection boundaries.
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
                HandleReady(time, space, extensions);
                break;
            case State::ComputingScene:
                HandleComputingScene(time, space, extensions);
                break;
            case State::Waiting:
                HandleWaiting(time);
                break;
            }

            m_lastFrameUpdate = time;
        }
    }

    void HandleReady(const XrTime time, const XrSpace& space, const XrSupportedExtensions& extensions)
    {
        XrNewSceneComputeInfoMSFT computeInfo{ XR_TYPE_NEW_SCENE_COMPUTE_INFO_MSFT };
        computeInfo.bounds.space = space;
        computeInfo.bounds.time = time;

        XrSpaceLocation viewInLocal{ XR_TYPE_SPACE_LOCATION };
        CHECK_XRCMD(xrLocateSpace(m_viewSpace.Get(), space, time, &viewInLocal));

        XrSceneSphereBoundMSFT sphere{};
        XrSceneOrientedBoxBoundMSFT box{};
        XrSceneFrustumBoundMSFT frustum{};
        switch (m_boundaryType)
        {
        case DetectionBoundaryType::Sphere:
            if (xr::math::Pose::IsPoseValid(viewInLocal)) {
                sphere.center = viewInLocal.pose.position;
                sphere.radius = m_sphereRadius;
            }

            computeInfo.bounds.sphereCount = 1;
            computeInfo.bounds.spheres = &sphere;
            break;
        case DetectionBoundaryType::Box:
            if (xr::math::Pose::IsPoseValid(viewInLocal)) {
                box.pose = viewInLocal.pose;
                box.extents = m_boxDimensions;
            }

            computeInfo.bounds.boxCount = 1;
            computeInfo.bounds.boxes = &box;
            break;
        case DetectionBoundaryType::Frustum:
            frustum.farDistance = m_frustum.FarDistance;
            frustum.pose = XrPosef
            {
                XrQuaternionf
                {
                    m_frustum.Pose.Orientation.X,
                    m_frustum.Pose.Orientation.Y,
                    m_frustum.Pose.Orientation.Z,
                    m_frustum.Pose.Orientation.W,
                },
                XrVector3f
                {
                    m_frustum.Pose.Position.X,
                    m_frustum.Pose.Position.Y,
                    m_frustum.Pose.Position.Z
                }
            };
            frustum.fov = XrFovf
            {
                m_frustum.FOV.AngleLeft,
                m_frustum.FOV.AngleRight,
                m_frustum.FOV.AngleUp,
                m_frustum.FOV.AngleDown
            };

            computeInfo.bounds.frustumCount = 1;
            computeInfo.bounds.frustums = &frustum;
            break;
        }

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
                sceneObjectKeys[i] = sceneObjects.at(i).sceneObjectKey;
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

            // Get a set of the known objects to understand if any objects were removed
            std::unordered_set<XrSceneObjectKeyMSFT> knownObjects{};
            std::transform(m_sceneObjects.begin(), m_sceneObjects.end(),
                std::inserter(knownObjects, knownObjects.end()),
                [](auto pair) { return pair.first; });

            for (size_t i = 0; i < sceneObjects.size(); i++)
            {
                const auto& location = m_sceneObjectLocations.at(i);
                const auto& sceneObject = sceneObjects.at(i);

                if (m_sceneObjects.count(sceneObject.sceneObjectKey) == 0)
                {
                    m_updatedObjects.push_back(sceneObject.sceneObjectKey);
                    m_sceneObjects[sceneObject.sceneObjectKey] = std::make_shared<SceneObject>();
                }

                knownObjects.erase(sceneObject.sceneObjectKey);
                auto& object = m_sceneObjects[sceneObject.sceneObjectKey];

                // Set scene object pose
                if (xr::math::Pose::IsPoseValid(location.locationFlags)) {
                    object->Pose = location.pose;
                }

                // Set scene object kind
                XrSceneObjectPropertiesMSFT properties{XR_TYPE_SCENE_OBJECT_PROPERTIES_MSFT};
                XrSceneObjectKindMSFT kind{XR_TYPE_SCENE_OBJECT_KIND_MSFT};
                xr::InsertExtensionStruct(properties, kind);
                XrSceneObjectPropertiesGetInfoMSFT getInfo{XR_TYPE_SCENE_OBJECT_PROPERTIES_GET_INFO_MSFT};
                getInfo.sceneObjectKey = sceneObject.sceneObjectKey;
                CHECK_XRCMD(extensions.xrGetSceneObjectPropertiesMSFT(m_sceneHandle.Get(), &getInfo, &properties));
                object->Kind = kind.kind;

                // Get sets of the known meshes and planes for this object to understand if any were removed
                std::unordered_set<XrSceneMeshKeyMSFT> knownMeshes{};
                std::transform(object->Meshes.begin(), object->Meshes.end(),
                    std::inserter(knownMeshes, knownMeshes.end()),
                    [](auto pair) { return pair.first; });

                std::unordered_set<XrScenePlaneKeyMSFT> knownPlanes{};
                std::transform(object->Planes.begin(), object->Planes.end(),
                    std::inserter(knownPlanes, knownPlanes.end()),
                    [](auto pair) { return pair.first; });

                const auto meshKeys = xr::GetMeshKeys(extensions, m_sceneHandle.Get(), sceneObject.sceneObjectKey);
                for (const auto& meshKey : meshKeys)
                {
                    if (object->Meshes.count(meshKey) == 0)
                    {
                        m_updatedMeshes.push_back(meshKey);
                    }

                    knownMeshes.erase(meshKey);
                    auto sceneMesh = xr::GetSceneMesh(extensions, m_sceneHandle.Get(), meshKey);
                    object->Meshes[meshKey] = Mesh{ std::move(sceneMesh), meshKey, sceneObject.sceneObjectKey };
                }

                const auto planeKeys = xr::GetPlaneKeys(extensions, m_sceneHandle.Get(), sceneObject.sceneObjectKey);
                for (const auto& planeKey : planeKeys)
                {
                    if (object->Planes.count(planeKey) == 0)
                    {
                        m_updatedPlanes.push_back(planeKey);
                    }

                    knownPlanes.erase(planeKey);
                    auto scenePlane = xr::GetScenePlane(extensions, m_sceneHandle.Get(), planeKey);
                    object->Planes[planeKey] = Plane{ std::move(scenePlane), planeKey, sceneObject.sceneObjectKey };
                }

                // Clean up removed meshes and planes for this object
                for (const auto& meshKey : knownMeshes)
                {
                    m_removedMeshes.push_back(meshKey);
                    object->Meshes.erase(meshKey);
                }

                for (const auto& planeKey : knownPlanes)
                {
                    m_removedPlanes.push_back(planeKey);
                    object->Planes.erase(planeKey);
                }
            }

            // Clean up removed objects
            for (const auto& objectKey : knownObjects)
            {
                m_removedObjects.push_back(objectKey);
                m_sceneObjects.erase(objectKey);
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
        PopulateFramePlaneArgs(args);
        PopulateFrameMeshArgs(args);
    }

    void PopulateFramePlaneArgs(UpdateFrameArgs& args)
    {
        args.Planes.clear();
        for (const auto& [objectKey, object] : m_sceneObjects)
        {
            for (const auto& [key, xrPlane] : object->Planes)
            {
                if (m_babylonPlanes.count(key) == 0)
                {
                    const auto newPlane = std::make_shared<xr::System::Session::Frame::Plane>();
                    m_babylonPlanes[key] = newPlane;
                    m_babylonPlanesById[newPlane->ID] = newPlane;
                }

                auto& babylonPlane = *m_babylonPlanes[key];
                babylonPlane.Center = XrPoseToBabylonPose(object->Pose);
                babylonPlane.PolygonFormat = xr::PolygonFormat::XYZ;

                // Note: Without a normal its unclear how to define the front/back of the plane
                // This is currently an arbitrary winding order due to this lack of information
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

                babylonPlane.GeometryId = static_cast<xr::GeometryId>(objectKey);

                if (s_sceneObjectTypeMap.count(object->Kind) > 0)
                {
                    babylonPlane.GeometryType = s_sceneObjectTypeMap.at(object->Kind);
                }
                else
                {
                    babylonPlane.GeometryType = s_sceneObjectTypeMap.at(XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_UNKNOWN_MSFT);
                }

                args.Planes.push_back(babylonPlane);
            }
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

    void PopulateFrameMeshArgs(UpdateFrameArgs& args)
    {
        args.Meshes.clear();
        for (const auto& [objectKey, object] : m_sceneObjects)
        {
            for (const auto& [key, xrMesh] : object->Meshes)
            {
                if (m_babylonMeshes.count(key) == 0)
                {
                    const auto newMesh = std::make_shared<xr::System::Session::Frame::Mesh>();
                    m_babylonMeshes[key] = newMesh;
                    m_babylonMeshesById[newMesh->ID] = newMesh;
                }

                auto& babylonMesh = *m_babylonMeshes[key];

                assert(sizeof(XrVector3f) == sizeof(xr::Vector3f));
                constexpr uint8_t NUM_FLOAT_IN_VECTOR3 = 3;
                babylonMesh.Positions.resize(NUM_FLOAT_IN_VECTOR3 * xrMesh.positions.size());
                for (size_t n = 0; n < xrMesh.positions.size(); n++)
                {
                    size_t positionIndex = NUM_FLOAT_IN_VECTOR3 * n;
                    const auto position = TransformPoint(xrMesh.positions.at(n), object->Pose);
                    babylonMesh.Positions[positionIndex] = position.x;
                    babylonMesh.Positions[positionIndex + 1] = position.y;
                    babylonMesh.Positions[positionIndex + 2] = position.z;
                }

                babylonMesh.Indices.resize(xrMesh.indices.size());
                memcpy(babylonMesh.Indices.data(), xrMesh.indices.data(), xrMesh.indices.size() * sizeof(xr::System::Session::Frame::Mesh::IndexType));

                babylonMesh.HasNormals = false;
                babylonMesh.Normals.resize(0);

                babylonMesh.GeometryId = static_cast<xr::GeometryId>(objectKey);
                if (s_sceneObjectTypeMap.count(object->Kind) > 0)
                {
                    babylonMesh.GeometryType = s_sceneObjectTypeMap.at(object->Kind);
                }
                else
                {
                    babylonMesh.GeometryType = s_sceneObjectTypeMap.at(XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_UNKNOWN_MSFT);
                }

                args.Meshes.push_back(babylonMesh);
            }
        }

        args.UpdatedMeshes.clear();
        for (const auto xrKey : m_updatedMeshes)
        {
            if (m_babylonMeshes.count(xrKey) > 0)
            {
                const auto babylonID = m_babylonMeshes.at(xrKey)->ID;
                args.UpdatedMeshes.push_back(babylonID);
            }
        }

        args.RemovedMeshes.clear();
        for (const auto xrKey : m_removedMeshes)
        {
            if (m_babylonMeshes.count(xrKey) > 0)
            {
                const auto babylonMesh = m_babylonMeshes.at(xrKey);
                args.RemovedMeshes.push_back(babylonMesh->ID);
                m_babylonMeshes.erase(xrKey);
                m_babylonMeshesById.erase(babylonMesh->ID);
            }
        }
    }

    void PopulateSceneObjectsArgs(UpdateSceneObjectsArgs& args)
    {
        args.SceneObjects.clear();
        for (const auto& [key, object] : m_sceneObjects)
        {
            args.SceneObjects[key] = object;
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

    System::Session::Frame::Mesh& TryGetMeshByID(const System::Session::Frame::Mesh::Identifier id)
    {
        if (m_babylonMeshesById.count(id) > 0)
        {
            return *m_babylonMeshesById[id];
        }

        throw std::exception(/*unknown mesh id*/);
    }

private:
    const std::map<XrSceneObjectKindTypeMSFT, std::string> s_sceneObjectTypeMap
    {
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_BACKGROUND_MSFT, "background"},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_CEILING_MSFT, "ceiling"},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_FLOOR_MSFT, "floor"},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_PLATFORM_MSFT, "platform"},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_UNKNOWN_MSFT, "unknown"},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_WALL_MSFT, "wall"}
    };

    enum class State
    {
        Ready,
        ComputingScene,
        Waiting
    };

    bool m_initialized{ false };
    xr::SpaceHandle m_viewSpace{};
    xr::DetectionBoundaryType m_boundaryType{};
    float m_sphereRadius;
    xr::Frustum m_frustum;
    XrVector3f m_boxDimensions{};
    XrTime m_updateInterval;
    xr::SceneObserverHandle m_sceneObserverHandle{};
    XrTime m_lastSceneUpdate{};
    State m_state{ State::Ready };
    xr::SceneHandle m_sceneHandle{};
    XrTime m_lastFrameUpdate{};
    XrTime m_lastSceneObjectUpdate{};
    std::vector<XrSceneObjectLocationMSFT> m_sceneObjectLocations;

    std::map<XrSceneObjectKeyMSFT, std::shared_ptr<SceneObject>> m_sceneObjects{};
    std::vector<XrSceneObjectKeyMSFT> m_updatedObjects{};
    std::vector<XrSceneMeshKeyMSFT> m_updatedMeshes{};
    std::vector<XrScenePlaneKeyMSFT> m_updatedPlanes{};
    std::vector<XrSceneObjectKeyMSFT> m_removedObjects{};
    std::vector<XrSceneMeshKeyMSFT> m_removedMeshes{};
    std::vector<XrScenePlaneKeyMSFT> m_removedPlanes{};

    std::map<XrScenePlaneKeyMSFT, std::shared_ptr<xr::System::Session::Frame::Plane>> m_babylonPlanes;
    std::map<xr::System::Session::Frame::Plane::Identifier, std::shared_ptr<xr::System::Session::Frame::Plane>> m_babylonPlanesById;
    std::map<XrSceneMeshKeyMSFT, std::shared_ptr<xr::System::Session::Frame::Mesh>> m_babylonMeshes;
    std::map<xr::System::Session::Frame::Mesh::Identifier, std::shared_ptr<xr::System::Session::Frame::Mesh>> m_babylonMeshesById;
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

System::Session::Frame::Mesh& SceneUnderstanding::TryGetMeshByID(const System::Session::Frame::Mesh::Identifier id) const
{
    return m_impl->TryGetMeshByID(id);
}