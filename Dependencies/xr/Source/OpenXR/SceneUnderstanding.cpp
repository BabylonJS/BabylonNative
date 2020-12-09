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
    , DetectionBoundary{ DEFAULT_BOUNDARY_TYPE, DEFAULT_SPHERE_RADIUS }
    , UpdateIntervalInSeconds(DEFAULT_UPDATE_INTERVAL_IN_SECONDS) {}

SceneUnderstanding::SceneUnderstanding::InitOptions::InitOptions(
    const XrSession& session,
    const XrSupportedExtensions& extensions,
    const xr::DetectionBoundary detectionBoundary,
    const double updateIntervalInSeconds)
    : Session(session)
    , Extensions(extensions)
    , DetectionBoundary(detectionBoundary)
    , UpdateIntervalInSeconds(updateIntervalInSeconds) {}

struct SceneUnderstanding::SceneUnderstanding::Impl
{
public:
    void Initialize(InitOptions options)
    {
        const auto& session = options.Session;
        const auto& extensions = options.Extensions;
        if (!extensions.SceneUnderstandingSupported)
        {
            return;
        }

        // Default to most recently provided detection boundary information
        m_detectionBoundary = options.DetectionBoundary;
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
                HandleReady(time, extensions);
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

    void HandleReady(const XrTime time, const XrSupportedExtensions& extensions)
    {
        XrNewSceneComputeInfoMSFT computeInfo{ XR_TYPE_NEW_SCENE_COMPUTE_INFO_MSFT };
        computeInfo.bounds.space = m_viewSpace.Get();
        computeInfo.bounds.time = time;

        XrSceneSphereBoundMSFT sphere{};
        XrSceneOrientedBoxBoundMSFT box{};
        XrSceneFrustumBoundMSFT frustum{};
        switch (m_detectionBoundary.Type)
        {
        case DetectionBoundaryType::Sphere:
        {
            sphere.center = XrVector3f{ 0.f, 0.f, 0.f };
            sphere.radius = std::get<float>(m_detectionBoundary.Data);

            computeInfo.bounds.sphereCount = 1;
            computeInfo.bounds.spheres = &sphere;
            break;
        }
        case DetectionBoundaryType::Box:
        {
            box.pose = XrPosef{ XrQuaternionf{0.f, 0.f, 0.f, 1.f}, XrVector3f{0.f, 0.f, 0.f} };
            const auto& boxDimensions = std::get<xr::Vector3f>(m_detectionBoundary.Data);
            box.extents = XrVector3f{ boxDimensions.X, boxDimensions.Y, boxDimensions.Z };

            computeInfo.bounds.boxCount = 1;
            computeInfo.bounds.boxes = &box;
            break;
        }
        case DetectionBoundaryType::Frustum:
        {
            const auto& frustumData = std::get<xr::Frustum>(m_detectionBoundary.Data);
            frustum.farDistance = frustumData.FarDistance;
            frustum.pose = XrPosef
            {
                XrQuaternionf
                {
                    frustumData.Pose.Orientation.X,
                    frustumData.Pose.Orientation.Y,
                    frustumData.Pose.Orientation.Z,
                    frustumData.Pose.Orientation.W,
                },
                XrVector3f
                {
                    frustumData.Pose.Position.X,
                    frustumData.Pose.Position.Y,
                    frustumData.Pose.Position.Z
                }
            };
            frustum.fov = XrFovf
            {
                frustumData.FOV.AngleLeft,
                frustumData.FOV.AngleRight,
                frustumData.FOV.AngleUp,
                frustumData.FOV.AngleDown
            };

            computeInfo.bounds.frustumCount = 1;
            computeInfo.bounds.frustums = &frustum;
            break;
        }
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
            std::unordered_set<SceneObject::Identifier> knownObjects{};
            std::transform(m_sceneObjects.begin(), m_sceneObjects.end(),
                std::inserter(knownObjects, knownObjects.end()),
                [](const auto& pair) { return pair.first; });

            for (size_t i = 0; i < sceneObjects.size(); i++)
            {
                const auto& location = m_sceneObjectLocations.at(i);
                const auto& sceneObject = sceneObjects.at(i);
                const auto sceneObjectId = static_cast<SceneObject::Identifier>(sceneObject.sceneObjectKey);

                if (m_sceneObjects.count(sceneObjectId) == 0)
                {
                    m_sceneObjects[sceneObjectId] = std::make_shared<SceneObject>();
                    m_sceneObjects[sceneObjectId]->ID = sceneObjectId;
                    m_updatedObjects.push_back(sceneObjectId);
                }

                knownObjects.erase(sceneObjectId);
                auto& object = m_sceneObjects[sceneObjectId];

                // Set scene object pose
                if (xr::math::Pose::IsPoseValid(location.locationFlags)) {
                    object->Pose = location.pose;
                }

                // Set scene object kind
                XrSceneObjectPropertiesMSFT properties{ XR_TYPE_SCENE_OBJECT_PROPERTIES_MSFT };
                XrSceneObjectKindMSFT kind{ XR_TYPE_SCENE_OBJECT_KIND_MSFT };
                xr::InsertExtensionStruct(properties, kind);
                XrSceneObjectPropertiesGetInfoMSFT getInfo{ XR_TYPE_SCENE_OBJECT_PROPERTIES_GET_INFO_MSFT };
                getInfo.sceneObjectKey = sceneObject.sceneObjectKey;
                CHECK_XRCMD(extensions.xrGetSceneObjectPropertiesMSFT(m_sceneHandle.Get(), &getInfo, &properties));

                if (s_sceneObjectTypeMap.count(kind.kind) > 0)
                {
                    object->Type = s_sceneObjectTypeMap.at(kind.kind);
                }
                else
                {
                    object->Type = xr::SceneObjectType::Unknown;
                }

                // Get sets of the known meshes and planes for this object to understand if any were removed
                std::unordered_set<XrSceneMeshKeyMSFT> knownMeshes{};
                std::transform(object->Meshes.begin(), object->Meshes.end(),
                    std::inserter(knownMeshes, knownMeshes.end()),
                    [](const auto& pair) { return pair.first; });

                std::unordered_set<XrScenePlaneKeyMSFT> knownPlanes{};
                std::transform(object->Planes.begin(), object->Planes.end(),
                    std::inserter(knownPlanes, knownPlanes.end()),
                    [](const auto& pair) { return pair.first; });

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
        PopulateFrameSceneObjectArgs(args);
        PopulateFramePlaneArgs(args);
        PopulateFrameMeshArgs(args);
    }

    void PopulateFrameSceneObjectArgs(UpdateFrameArgs& args)
    {
        args.UpdatedSceneObjects = m_updatedObjects;
        args.RemovedSceneObjects = m_removedObjects;
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

                babylonPlane.ParentSceneObjectID = objectKey;

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
                babylonMesh.Positions.resize(xrMesh.positions.size());
                for (size_t n = 0; n < xrMesh.positions.size(); n++)
                {
                    const auto position = TransformPoint(xrMesh.positions.at(n), object->Pose);
                    babylonMesh.Positions[n].X = position.x;
                    babylonMesh.Positions[n].Y = position.y;
                    babylonMesh.Positions[n].Z = position.z;
                }

                babylonMesh.Indices.resize(xrMesh.indices.size());
                memcpy(babylonMesh.Indices.data(), xrMesh.indices.data(), xrMesh.indices.size() * sizeof(xr::System::Session::Frame::Mesh::IndexType));

                // OpenXR uses counter clockwise winding order
                babylonMesh.IsClockwiseWindingOrder = false;

                babylonMesh.HasNormals = false;
                babylonMesh.Normals.resize(0);

                babylonMesh.ParentSceneObjectID = objectKey;

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

    System::Session::Frame::SceneObject& TryGetSceneObjectByID(const System::Session::Frame::SceneObject::Identifier id)
    {
        if (m_sceneObjects.count(id) > 0)
        {
            return *m_sceneObjects.at(id);
        }

        throw std::exception(/*unknown scene object id*/);
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
    const std::map<XrSceneObjectKindTypeMSFT, xr::SceneObjectType> s_sceneObjectTypeMap
    {
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_BACKGROUND_MSFT, xr::SceneObjectType::Background},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_CEILING_MSFT, xr::SceneObjectType::Ceiling},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_FLOOR_MSFT, xr::SceneObjectType::Floor},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_PLATFORM_MSFT, xr::SceneObjectType::Platform},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_UNKNOWN_MSFT, xr::SceneObjectType::Unknown},
        {XrSceneObjectKindTypeMSFT::XR_SCENE_OBJECT_KIND_TYPE_WALL_MSFT, xr::SceneObjectType::Wall}
    };

    enum class State
    {
        Ready,
        ComputingScene,
        Waiting
    };

    bool m_initialized{ false };
    xr::SpaceHandle m_viewSpace{};
    xr::DetectionBoundary m_detectionBoundary;
    XrTime m_updateInterval;
    xr::SceneObserverHandle m_sceneObserverHandle{};
    XrTime m_lastSceneUpdate{};
    State m_state{ State::Ready };
    xr::SceneHandle m_sceneHandle{};
    XrTime m_lastFrameUpdate{};
    XrTime m_lastSceneObjectUpdate{};
    std::vector<XrSceneObjectLocationMSFT> m_sceneObjectLocations;

    std::vector<SceneObject::Identifier> m_updatedObjects{};
    std::vector<XrSceneMeshKeyMSFT> m_updatedMeshes{};
    std::vector<XrScenePlaneKeyMSFT> m_updatedPlanes{};
    std::vector<SceneObject::Identifier> m_removedObjects{};
    std::vector<XrSceneMeshKeyMSFT> m_removedMeshes{};
    std::vector<XrScenePlaneKeyMSFT> m_removedPlanes{};

    std::map<SceneObject::Identifier, std::shared_ptr<SceneObject>> m_sceneObjects{};
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

void SceneUnderstanding::Initialize(InitOptions options) const
{
    m_impl->Initialize(options);
}

void SceneUnderstanding::UpdateFrame(UpdateFrameArgs args) const
{
    m_impl->UpdateFrame(args);
}

System::Session::Frame::SceneObject& SceneUnderstanding::TryGetSceneObjectByID(const System::Session::Frame::SceneObject::Identifier id) const
{
    return m_impl->TryGetSceneObjectByID(id);
}

System::Session::Frame::Plane& SceneUnderstanding::TryGetPlaneByID(const System::Session::Frame::Plane::Identifier id) const
{
    return m_impl->TryGetPlaneByID(id);
}

System::Session::Frame::Mesh& SceneUnderstanding::TryGetMeshByID(const System::Session::Frame::Mesh::Identifier id) const
{
    return m_impl->TryGetMeshByID(id);
}