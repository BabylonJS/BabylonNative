#ifndef NOMINMAX
#define NOMINMAX // avoid including macro max definitions in favor of limit max definitions
#endif

#include "SceneUnderstanding.h"
#include "XrRegistry.h"
#include "XrUtility/XrSceneUnderstanding.h"
#include "XrUtility/XrSceneUnderstanding.hpp"

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
    ~Impl()
    {
        Disable();
    }

    void Initialize(const InitOptions& options)
    {
        if (options.Extensions.SceneUnderstandingSupported)
        {
            m_updateInterval = static_cast<XrTime>(NANOSECONDS_IN_SECOND * options.UpdateIntervalInSeconds);

            m_sceneBounds.boxBounds.clear();
            m_sceneBounds.sphereBounds.clear();
            m_sceneBounds.frustumBounds.clear();
            switch (options.DetectionBoundary.Type)
            {
            case DetectionBoundaryType::Box:
            {
                const auto& boxDimensions = std::get<xr::Vector3f>(options.DetectionBoundary.Data);
                m_sceneBounds.boxBounds.resize(1);
                m_sceneBounds.boxBounds[0].pose = XrPosef{ XrQuaternionf{0.f, 0.f, 0.f, 1.f}, XrVector3f{0.f, 0.f, 0.f} };
                m_sceneBounds.boxBounds[0].extents = XrVector3f{ boxDimensions.X, boxDimensions.Y, boxDimensions.Z };
                break;
            }
            case DetectionBoundaryType::Sphere:
            {
                const float radius = std::get<float>(options.DetectionBoundary.Data);
                m_sceneBounds.sphereBounds.resize(1);
                m_sceneBounds.sphereBounds[0].center = XrVector3f{ 0.f, 0.f, 0.f };
                m_sceneBounds.sphereBounds[0].radius = radius;
                break;
            }
            case DetectionBoundaryType::Frustum:
            {
                const auto& frustumData = std::get<xr::Frustum>(options.DetectionBoundary.Data);
                m_sceneBounds.frustumBounds.resize(1);
                m_sceneBounds.frustumBounds[0].farDistance = frustumData.FarDistance;
                m_sceneBounds.frustumBounds[0].pose = XrPosef
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
                m_sceneBounds.frustumBounds[0].fov = XrFovf
                {
                    frustumData.FOV.AngleLeft,
                    frustumData.FOV.AngleRight,
                    frustumData.FOV.AngleUp,
                    frustumData.FOV.AngleDown
                };
                break;
            }
            }

            Enable(options);
        }
    }

    void UpdateFrame(UpdateFrameArgs& args)
    {
        if (!m_sceneObserver)
        {
            return;
        }

        if (m_scanState == ScanState::Idle) {
            if (args.DisplayTime - m_lastUpdateTime > m_updateInterval)
            {
                ResetSceneData();

                m_sceneBounds.space = args.SceneSpace;
                m_sceneBounds.time = args.DisplayTime;
                m_sceneObserver->ComputeNewScene(m_sceneBounds);

                m_lastUpdateTime = args.DisplayTime;
                m_scanState = ScanState::Waiting;
            }
        }
        else if (m_scanState == ScanState::Waiting)
        {
            if (m_sceneObserver->IsSceneComputeCompleted()) {
                m_scanState = ScanState::Processing;
            }
        }
        else if (m_scanState == ScanState::Processing)
        {
            UpdateSceneData(args);
            PopulateUpdateFrameArgs(args);
            m_scanState = ScanState::Idle;
        }
    }

    System::Session::Frame::SceneObject& GetSceneObjectByID(const System::Session::Frame::SceneObject::Identifier id)
    {
        if (m_objects.find(id) == m_objects.end())
        {
            throw std::exception(/*invalid id provided*/);
        }

        return *m_objects.at(id);
    }

    System::Session::Frame::Plane& GetPlaneByID(const System::Session::Frame::Plane::Identifier id)
    {
        if (m_planes.find(id) == m_planes.end())
        {
            throw std::exception(/*invalid id provided*/);
        }

        return *m_planes.at(id);
    }

    System::Session::Frame::Mesh& GetMeshByID(const System::Session::Frame::Mesh::Identifier id)
    {
        if (m_meshes.find(id) == m_meshes.end())
        {
            throw std::exception(/*invalid id provided*/);
        }

        return *m_meshes.at(id);
    }

private:
    enum class ScanState
    {
        Idle,
        Waiting,
        Processing
    };

    struct SceneObject : System::Session::Frame::SceneObject {};
    struct Mesh : System::Session::Frame::Mesh
    {
        uint64_t MeshBufferId;
    };
    struct Plane : System::Session::Frame::Plane
    {
        xr::su::ScenePlane::Extent Size;
    };

    void Enable(const InitOptions& options)
    {
        if (!options.Extensions.SceneUnderstandingSupported)
        {
            return;
        }

        if (m_sceneObserver)
        {
            return;
        }

        m_sceneObserver = std::make_unique<xr::su::SceneObserver>(options.Extensions, options.Session);
        m_scanState = ScanState::Idle;
    }

    void Disable()
    {
        m_sceneObserver = nullptr;
    }

    void ResetSceneData()
    {
        m_updatedObjects.clear();
        m_removedObjects.clear();
        m_updatedMeshes.clear();
        m_removedMeshes.clear();
        m_updatedPlanes.clear();
        m_removedPlanes.clear();
    }

    void UpdateSceneData(UpdateFrameArgs& args)
    {
        m_scene = m_sceneObserver->CreateScene();
        UpdateSceneObjects();
        UpdateMeshes(args);
        UpdatePlanes(args);
    }

    void UpdateSceneObjects()
    {
        const auto objects = m_scene->GetObjects();
        std::unordered_set<xr::su::SceneObject::Id> observedObjectIds{};
        for (const auto& sceneObject : objects)
        {
            observedObjectIds.insert(sceneObject.id);

            if (m_objectIds.find(sceneObject.id) == m_objectIds.end())
            {
                auto object = std::make_unique<SceneObject>();
                const auto objectId = object->ID;
                m_objectIds[sceneObject.id] = objectId;
                m_objects[objectId] = std::move(object);
            }

            const auto objectId = m_objectIds.at(sceneObject.id);
            m_updatedObjects.push_back(objectId);
            auto& object = m_objects.at(objectId);
            object->Type = c_objectTypeMap.at(sceneObject.kind);
        }

        std::unordered_set<xr::su::SceneObject::Id> objectIdsToRemove{};
        for (const auto& [objectId, sceneObjectId] : m_objectIds)
        {
            if (observedObjectIds.find(objectId) == observedObjectIds.end())
            {
                objectIdsToRemove.insert(objectId);
            }
        }

        for (const auto& objectId : objectIdsToRemove)
        {
            const auto sceneObjectID = m_objectIds.at(objectId);
            m_objectIds.erase(objectId);
            m_objects.erase(sceneObjectID);
        }
    }

    void UpdateMeshes(UpdateFrameArgs& args)
    {
        const auto meshes = m_scene->GetMeshes();
        std::unordered_set<xr::su::SceneMesh::Id> observedMeshIds{};
        for (const auto& sceneMesh : meshes)
        {
            observedMeshIds.insert(sceneMesh.id);

            if (m_meshIds.find(sceneMesh.id) == m_meshIds.end())
            {
                auto mesh = std::make_unique<Mesh>();
                const auto meshId = mesh->ID;
                m_meshIds[sceneMesh.id] = meshId;
                m_meshes[meshId] = std::move(mesh);
            }

            const auto meshId = m_meshIds.at(sceneMesh.id);
            m_updatedMeshes.push_back(meshId);
            auto& mesh = m_meshes.at(meshId);
            mesh->ParentSceneObjectID = m_objectIds.at(sceneMesh.parentObjectId);
            mesh->MeshBufferId = sceneMesh.meshBufferId;
        }

        std::unordered_set<xr::su::SceneMesh::Id> meshIdsToRemove{};
        for (const auto& [meshId, sceneMeshId] : m_meshIds)
        {
            if (observedMeshIds.find(meshId) == observedMeshIds.end())
            {
                meshIdsToRemove.insert(meshId);
            }
        }

        for (const auto& meshId : meshIdsToRemove)
        {
            const auto sceneMeshId = m_meshIds.at(meshId);
            m_meshIds.erase(meshId);
            m_meshes.erase(sceneMeshId);
        }

        std::vector<XrUuidMSFT> meshIds{};
        for (const auto& mesh : meshes)
        {
            meshIds.push_back(static_cast<XrUuidMSFT>(mesh.id));
        }
        std::vector<XrSceneComponentLocationMSFT> meshLocations{};
        xr::su::LocateObjects(m_scene->Handle(), args.Extensions, args.SceneSpace, args.DisplayTime, meshIds, meshLocations);
        for (size_t i = 0; i < meshLocations.size() && i < meshIds.size(); i++)
        {
            const auto& id = static_cast<xr::su::SceneMesh::Id>(meshIds.at(i));
            const auto& sceneMeshId = m_meshIds.at(id);
            auto& mesh = m_meshes.at(sceneMeshId);
            const auto& location = meshLocations.at(i);

            // TODO: in preview 3 of the OpenXR scene understanding extension, plane's will also have associated meshes
            xr::SceneMeshBuffers meshBuffers;
            xr::ReadMeshBuffers(m_scene->Handle(), args.Extensions, mesh->MeshBufferId, meshBuffers);

            assert(sizeof(XrVector3f) == sizeof(xr::Vector3f));
            mesh->Positions.resize(meshBuffers.vertexBuffer.size());
            for (size_t n = 0; n < meshBuffers.vertexBuffer.size(); n++)
            {
                const auto position = TransformPoint(meshBuffers.vertexBuffer.at(n), location.pose);
                mesh->Positions[n].X = position.x;
                mesh->Positions[n].Y = position.y;
                mesh->Positions[n].Z = position.z;
            }

            assert(sizeof(Mesh::IndexType) == sizeof(uint32_t));
            mesh->Indices.resize(meshBuffers.indexBuffer.size());
            memcpy(mesh->Indices.data(), meshBuffers.indexBuffer.data(), meshBuffers.indexBuffer.size() * sizeof(uint32_t));

            // TODO: decide whether we want to calculate normals
            mesh->HasNormals = false;
        }
    }

    void UpdatePlanes(UpdateFrameArgs& args)
    {
        const auto planes = m_scene->GetPlanes();
        std::unordered_set<xr::su::ScenePlane::Id> observedPlaneIds{};
        for (const auto& scenePlane : planes)
        {
            observedPlaneIds.insert(scenePlane.id);

            if (m_planeIds.find(scenePlane.id) == m_planeIds.end())
            {
                auto plane = std::make_unique<Plane>();
                const auto planeId = plane->ID;
                m_planeIds[scenePlane.id] = planeId;
                m_planes[planeId] = std::move(plane);
            }

            const auto planeId = m_planeIds.at(scenePlane.id);
            m_updatedPlanes.push_back(planeId);
            auto& plane = m_planes.at(planeId);
            plane->ParentSceneObjectID = m_objectIds.at(scenePlane.parentObjectId);
            plane->Size = scenePlane.size;
        }

        std::unordered_set<xr::su::ScenePlane::Id> planeIdsToRemove{};
        for (const auto& [planeId, scenePlaneId] : m_planeIds)
        {
            if (observedPlaneIds.find(planeId) == observedPlaneIds.end())
            {
                planeIdsToRemove.insert(planeId);
            }
        }

        for (const auto& planeId : planeIdsToRemove)
        {
            const auto scenePlaneId = m_planeIds.at(planeId);
            m_planeIds.erase(planeId);
            m_planes.erase(scenePlaneId);
        }

        std::vector<XrUuidMSFT> planeIds{};
        for (const auto& plane : planes)
        {
            planeIds.push_back(static_cast<XrUuidMSFT>(plane.id));
        }

        std::vector<XrSceneComponentLocationMSFT> planeLocations{};
        xr::su::LocateObjects(m_scene->Handle(), args.Extensions, args.SceneSpace, args.DisplayTime, planeIds, planeLocations);
        for (size_t n = 0; n < planeLocations.size() && n < planeIds.size(); n++)
        {
            const auto& id = static_cast<xr::su::ScenePlane::Id>(planeIds.at(n));
            const auto& planeObjectID = m_planeIds.at(id);
            auto& plane = m_planes.at(planeObjectID);
            const auto& location = planeLocations.at(n);
            plane->Center = XrPoseToBabylonPose(location.pose);
            plane->PolygonFormat = xr::PolygonFormat::XYZ;

            // TODO figure out what to do with alignment
            // TODO update babylonjs object kinds

            // Note: Without a normal its unclear how to define the front/back of the plane
            // This is currently an arbitrary winding order due to this lack of information
            constexpr uint8_t VALUES_IN_POINT = 3;
            constexpr uint8_t VALUES_IN_XYZ_QUAD = 12;
            plane->Polygon.resize(VALUES_IN_XYZ_QUAD);
            plane->Polygon[0] = -1.f * plane->Size.width / 2.f;
            plane->Polygon[1] = -1.f * plane->Size.height / 2.f;
            plane->Polygon[2] = 0;
            plane->Polygon[3] = 1.f * plane->Size.width / 2.f;
            plane->Polygon[4] = -1.f * plane->Size.height / 2.f;
            plane->Polygon[5] = 0;
            plane->Polygon[6] = 1.f * plane->Size.width / 2.f;
            plane->Polygon[7] = 1.f * plane->Size.height / 2.f;
            plane->Polygon[8] = 0;
            plane->Polygon[9] = -1.f * plane->Size.width / 2.f;
            plane->Polygon[10] = 1.f * plane->Size.height / 2.f;
            plane->Polygon[11] = 0;
            plane->PolygonSize = plane->Polygon.size() / VALUES_IN_POINT;
        }
    }

    void PopulateUpdateFrameArgs(UpdateFrameArgs& args)
    {
        args.UpdatedSceneObjects.clear();
        for (const auto& objectId : m_updatedObjects)
        {
            args.UpdatedSceneObjects.push_back(objectId);
        }
        m_updatedObjects.clear();

        args.RemovedSceneObjects.clear();
        for (const auto& objectId : m_removedObjects)
        {
            args.RemovedSceneObjects.push_back(objectId);
        }
        m_removedObjects.clear();

        args.UpdatedMeshes.clear();
        for (const auto& meshId : m_updatedMeshes)
        {
            args.UpdatedMeshes.push_back(meshId);
        }
        m_updatedMeshes.clear();

        args.RemovedMeshes.clear();
        for (const auto& meshId : m_removedMeshes)
        {
            args.RemovedMeshes.push_back(meshId);
        }
        m_removedMeshes.clear();

        args.UpdatedPlanes.clear();
        for (const auto& planeId : m_updatedPlanes)
        {
            args.UpdatedPlanes.push_back(planeId);
        }
        m_updatedPlanes.clear();

        args.RemovedPlanes.clear();
        for (const auto& planeId : m_removedPlanes)
        {
            args.RemovedPlanes.push_back(planeId);
        }
        m_removedPlanes.clear();
    }

    std::unique_ptr<xr::su::SceneObserver> m_sceneObserver;
    std::unique_ptr<xr::su::Scene> m_scene;
    xr::SceneBounds m_sceneBounds{};
    ScanState m_scanState{ ScanState::Idle };
    XrTime m_updateInterval{ 0 };
    XrTime m_lastUpdateTime{ 0 };

    std::unordered_map<xr::su::SceneObject::Id, SceneObject::Identifier> m_objectIds{};
    std::unordered_map<SceneObject::Identifier, std::unique_ptr<SceneObject>> m_objects{};
    std::unordered_map<xr::su::SceneMesh::Id, Mesh::Identifier> m_meshIds{};
    std::unordered_map<Mesh::Identifier, std::unique_ptr<Mesh>> m_meshes{};
    std::unordered_map<xr::su::ScenePlane::Id, Plane::Identifier> m_planeIds{};
    std::unordered_map<Plane::Identifier, std::unique_ptr<Plane>> m_planes{};
    std::vector<SceneObject::Identifier> m_updatedObjects{};
    std::vector<SceneObject::Identifier> m_removedObjects{};
    std::vector<Mesh::Identifier> m_updatedMeshes{};
    std::vector<Mesh::Identifier> m_removedMeshes{};
    std::vector<Plane::Identifier> m_updatedPlanes{};
    std::vector<Plane::Identifier> m_removedPlanes{};

    const std::unordered_map<xr::su::SceneObject::Kind, xr::SceneObjectType> c_objectTypeMap
    {
        { XrSceneObjectKindMSFT::XR_SCENE_OBJECT_KIND_UNCATEGORIZED_MSFT, xr::SceneObjectType::Unknown },
        { XrSceneObjectKindMSFT::XR_SCENE_OBJECT_KIND_BACKGROUND_MSFT, xr::SceneObjectType::Background },
        { XrSceneObjectKindMSFT::XR_SCENE_OBJECT_KIND_WALL_MSFT, xr::SceneObjectType::Wall },
        { XrSceneObjectKindMSFT::XR_SCENE_OBJECT_KIND_FLOOR_MSFT, xr::SceneObjectType::Floor },
        { XrSceneObjectKindMSFT::XR_SCENE_OBJECT_KIND_CEILING_MSFT, xr::SceneObjectType::Ceiling },
        { XrSceneObjectKindMSFT::XR_SCENE_OBJECT_KIND_PLATFORM_MSFT, xr::SceneObjectType::Platform },
        { XrSceneObjectKindMSFT::XR_SCENE_OBJECT_KIND_INFERRED_MSFT, xr::SceneObjectType::Inferred },
        { XrSceneObjectKindMSFT::XR_SCENE_OBJECT_KIND_WORLD_MSFT, xr::SceneObjectType::World }
    };
};

SceneUnderstanding::SceneUnderstanding() 
    : m_impl(std::make_unique<Impl>())
{
}

SceneUnderstanding::~SceneUnderstanding()
{
}

void SceneUnderstanding::Initialize(const InitOptions& options) const
{
    m_impl->Initialize(options);
}

void SceneUnderstanding::UpdateFrame(UpdateFrameArgs& args) const
{
    m_impl->UpdateFrame(args);
}

System::Session::Frame::SceneObject& SceneUnderstanding::GetSceneObjectByID(const System::Session::Frame::SceneObject::Identifier id) const
{
    return m_impl->GetSceneObjectByID(id);
}

System::Session::Frame::Plane& SceneUnderstanding::GetPlaneByID(const System::Session::Frame::Plane::Identifier id) const
{
    return m_impl->GetPlaneByID(id);
}

System::Session::Frame::Mesh& SceneUnderstanding::GetMeshByID(const System::Session::Frame::Mesh::Identifier id) const
{
    return m_impl->GetMeshByID(id);
}