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
                std::vector<XrSceneComputeFeatureMSFT> computeOptions {
                    XrSceneComputeFeatureMSFT::XR_SCENE_COMPUTE_FEATURE_PLANE_MSFT,
                    XrSceneComputeFeatureMSFT::XR_SCENE_COMPUTE_FEATURE_PLANE_MESH_MSFT,
                    XrSceneComputeFeatureMSFT::XR_SCENE_COMPUTE_FEATURE_VISUAL_MESH_MSFT
                };
                m_sceneObserver->ComputeNewScene(computeOptions, m_sceneBounds);

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
        return m_objects.at(id);
    }

    System::Session::Frame::Plane& GetPlaneByID(const System::Session::Frame::Plane::Identifier id)
    {
        return m_planes.at(id);
    }

    System::Session::Frame::Mesh& GetMeshByID(const System::Session::Frame::Mesh::Identifier id)
    {
        return m_meshes.at(id);
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
        const auto xrObjects = m_scene->GetObjects();
        std::unordered_set<xr::su::SceneObject::Id> observedXRObjectIds{};
        for (const auto& xrObject : xrObjects)
        {
            observedXRObjectIds.insert(xrObject.id);

            if (m_objectIds.find(xrObject.id) == m_objectIds.end())
            {
                SceneObject object;
                const auto objectId = object.ID;
                m_objects.emplace(objectId, object);
                m_objectIds[xrObject.id] = objectId;
            }

            const auto objectId = m_objectIds.at(xrObject.id);
            m_updatedObjects.push_back(objectId);
            auto& object = m_objects.at(objectId);
            object.Type = c_objectTypeMap.at(xrObject.type);
        }

        std::unordered_set<xr::su::SceneObject::Id> xrObjectIdsToRemove{};
        for (const auto& [xrObjectId, objectId] : m_objectIds)
        {
            if (observedXRObjectIds.find(xrObjectId) == observedXRObjectIds.end())
            {
                xrObjectIdsToRemove.insert(xrObjectId);
            }
        }

        for (const auto& xrObjectId : xrObjectIdsToRemove)
        {
            const auto objectId = m_objectIds.at(xrObjectId);
            m_objectIds.erase(xrObjectId);
            m_objects.erase(objectId);
        }
    }

    void UpdateMeshes(UpdateFrameArgs& args)
    {
        const auto xrMeshes = m_scene->GetVisualMeshes();
        std::unordered_set<xr::su::SceneMesh::Id> observedXRMeshIds{};
        for (const auto& xrMesh : xrMeshes)
        {
            observedXRMeshIds.insert(xrMesh.id);

            if (m_meshIds.find(xrMesh.id) == m_meshIds.end())
            {
                Mesh mesh;
                const auto meshId = mesh.ID;
                m_meshes.emplace(meshId, mesh);
                m_meshIds[xrMesh.id] = meshId;
            }

            const auto meshId = m_meshIds.at(xrMesh.id);
            m_updatedMeshes.push_back(meshId);
            auto& mesh = m_meshes.at(meshId);
            mesh.ParentSceneObjectID = m_objectIds.at(xrMesh.parentObjectId);
            mesh.MeshBufferId = xrMesh.meshBufferId;
        }

        std::unordered_set<xr::su::SceneMesh::Id> xrMeshIdsToRemove{};
        for (const auto& [xrMeshId, meshId] : m_meshIds)
        {
            if (observedXRMeshIds.find(xrMeshId) == observedXRMeshIds.end())
            {
                xrMeshIdsToRemove.insert(xrMeshId);
            }
        }

        for (const auto& xrMeshId : xrMeshIdsToRemove)
        {
            const auto meshId = m_meshIds.at(xrMeshId);
            m_meshIds.erase(xrMeshId);
            m_meshes.erase(meshId);
        }

        std::vector<XrUuidMSFT> xrMeshIds{};
        for (const auto& xrMesh : xrMeshes)
        {
            xrMeshIds.push_back(static_cast<XrUuidMSFT>(xrMesh.id));
        }

        std::vector<XrSceneComponentLocationMSFT> xrMeshLocations{};
        xr::su::LocateObjects(m_scene->Handle(), args.Extensions, args.SceneSpace, args.DisplayTime, xrMeshIds, xrMeshLocations);
        for (size_t i = 0; i < xrMeshLocations.size() && i < xrMeshIds.size(); i++)
        {
            const auto& xrMeshId = static_cast<xr::su::SceneMesh::Id>(xrMeshIds.at(i));
            const auto& meshId = m_meshIds.at(xrMeshId);
            auto& mesh = m_meshes.at(meshId);
            const auto& location = xrMeshLocations.at(i);

            // TODO: in preview 3 of the OpenXR scene understanding extension, plane's will also have associated meshes
            std::vector<XrVector3f> vertexBuffer{};
            std::vector<uint32_t> indexBuffer{};
            xr::ReadMeshBuffers(m_scene->Handle(), args.Extensions, mesh.MeshBufferId, vertexBuffer, indexBuffer);

            static_assert(sizeof(XrVector3f) == sizeof(xr::Vector3f));
            mesh.Positions.resize(vertexBuffer.size());
            for (size_t n = 0; n < vertexBuffer.size(); n++)
            {
                const auto position = TransformPoint(vertexBuffer.at(n), location.pose);
                mesh.Positions[n].X = position.x;
                mesh.Positions[n].Y = position.y;
                mesh.Positions[n].Z = position.z;
            }

            static_assert(sizeof(Mesh::IndexType) == sizeof(uint32_t));
            mesh.Indices.resize(indexBuffer.size());
            memcpy(mesh.Indices.data(), indexBuffer.data(), indexBuffer.size() * sizeof(uint32_t));

            mesh.HasNormals = false;
        }
    }

    void UpdatePlanes(UpdateFrameArgs& args)
    {
        const auto xrPlanes = m_scene->GetPlanes();
        std::unordered_set<xr::su::ScenePlane::Id> observedXRPlaneIds{};
        for (const auto& xrPlane : xrPlanes)
        {
            observedXRPlaneIds.insert(xrPlane.id);

            if (m_planeIds.find(xrPlane.id) == m_planeIds.end())
            {
                Plane plane;
                const auto planeId = plane.ID;
                m_planes.emplace(planeId, plane);
                m_planeIds[xrPlane.id] = planeId;
            }

            const auto planeId = m_planeIds.at(xrPlane.id);
            m_updatedPlanes.push_back(planeId);
            auto& plane = m_planes.at(planeId);
            plane.ParentSceneObjectID = m_objectIds.at(xrPlane.parentObjectId);
            plane.Size = xrPlane.size;
        }

        std::unordered_set<xr::su::ScenePlane::Id> xrPlaneIdsToRemove{};
        for (const auto& [xrPlaneId, planeId] : m_planeIds)
        {
            if (observedXRPlaneIds.find(xrPlaneId) == observedXRPlaneIds.end())
            {
                xrPlaneIdsToRemove.insert(xrPlaneId);
            }
        }

        for (const auto& xrPlaneId : xrPlaneIdsToRemove)
        {
            const auto planeId = m_planeIds.at(xrPlaneId);
            m_planeIds.erase(xrPlaneId);
            m_planes.erase(planeId);
        }

        std::vector<XrUuidMSFT> xrPlaneIds{};
        for (const auto& xrPlane : xrPlanes)
        {
            xrPlaneIds.push_back(static_cast<XrUuidMSFT>(xrPlane.id));
        }

        std::vector<XrSceneComponentLocationMSFT> xrPlaneLocations{};
        xr::su::LocateObjects(m_scene->Handle(), args.Extensions, args.SceneSpace, args.DisplayTime, xrPlaneIds, xrPlaneLocations);
        for (size_t n = 0; n < xrPlaneLocations.size() && n < xrPlaneIds.size(); n++)
        {
            const auto& id = static_cast<xr::su::ScenePlane::Id>(xrPlaneIds.at(n));
            const auto& planeObjectID = m_planeIds.at(id);
            auto& plane = m_planes.at(planeObjectID);
            const auto& location = xrPlaneLocations.at(n);
            plane.Center = XrPoseToBabylonPose(location.pose);
            plane.PolygonFormat = xr::PolygonFormat::XYZ;

            // Note: Without a normal its unclear how to define the front/back of the plane
            // This is currently an arbitrary winding order due to this lack of information
            constexpr uint8_t VALUES_IN_POINT = 3;
            constexpr uint8_t VALUES_IN_XYZ_QUAD = 12;
            plane.Polygon.resize(VALUES_IN_XYZ_QUAD);
            plane.Polygon[0] = -1.f * plane.Size.width / 2.f;
            plane.Polygon[1] = -1.f * plane.Size.height / 2.f;
            plane.Polygon[2] = 0;
            plane.Polygon[3] = 1.f * plane.Size.width / 2.f;
            plane.Polygon[4] = -1.f * plane.Size.height / 2.f;
            plane.Polygon[5] = 0;
            plane.Polygon[6] = 1.f * plane.Size.width / 2.f;
            plane.Polygon[7] = 1.f * plane.Size.height / 2.f;
            plane.Polygon[8] = 0;
            plane.Polygon[9] = -1.f * plane.Size.width / 2.f;
            plane.Polygon[10] = 1.f * plane.Size.height / 2.f;
            plane.Polygon[11] = 0;
            plane.PolygonSize = plane.Polygon.size() / VALUES_IN_POINT;
        }
    }

    void PopulateUpdateFrameArgs(UpdateFrameArgs& args)
    {
        args.UpdatedSceneObjects.clear();
        m_updatedObjects.swap(args.UpdatedSceneObjects);

        args.RemovedSceneObjects.clear();
        m_removedObjects.swap(args.RemovedSceneObjects);

        args.UpdatedMeshes.clear();
        m_updatedMeshes.swap(args.UpdatedMeshes);

        args.RemovedMeshes.clear();
        m_removedMeshes.swap(args.RemovedMeshes);

        args.UpdatedPlanes.clear();
        m_updatedPlanes.swap(args.UpdatedPlanes);

        args.RemovedPlanes.clear();
        m_removedPlanes.swap(args.RemovedPlanes);
    }

    std::unique_ptr<xr::su::SceneObserver> m_sceneObserver;
    std::unique_ptr<xr::su::Scene> m_scene;
    xr::SceneBounds m_sceneBounds{};
    ScanState m_scanState{ ScanState::Idle };
    XrTime m_updateInterval{ 0 };
    XrTime m_lastUpdateTime{ 0 };

    std::unordered_map<xr::su::SceneObject::Id, SceneObject::Identifier> m_objectIds{};
    std::unordered_map<SceneObject::Identifier, SceneObject> m_objects{};
    std::unordered_map<xr::su::SceneMesh::Id, Mesh::Identifier> m_meshIds{};
    std::unordered_map<Mesh::Identifier, Mesh> m_meshes{};
    std::unordered_map<xr::su::ScenePlane::Id, Plane::Identifier> m_planeIds{};
    std::unordered_map<Plane::Identifier, Plane> m_planes{};
    std::vector<SceneObject::Identifier> m_updatedObjects{};
    std::vector<SceneObject::Identifier> m_removedObjects{};
    std::vector<Mesh::Identifier> m_updatedMeshes{};
    std::vector<Mesh::Identifier> m_removedMeshes{};
    std::vector<Plane::Identifier> m_updatedPlanes{};
    std::vector<Plane::Identifier> m_removedPlanes{};

    const std::unordered_map<xr::su::SceneObject::Type, xr::SceneObjectType> c_objectTypeMap
    {
        { XrSceneObjectTypeMSFT::XR_SCENE_OBJECT_TYPE_UNCATEGORIZED_MSFT, xr::SceneObjectType::Unknown },
        { XrSceneObjectTypeMSFT::XR_SCENE_OBJECT_TYPE_BACKGROUND_MSFT, xr::SceneObjectType::Background },
        { XrSceneObjectTypeMSFT::XR_SCENE_OBJECT_TYPE_WALL_MSFT, xr::SceneObjectType::Wall },
        { XrSceneObjectTypeMSFT::XR_SCENE_OBJECT_TYPE_FLOOR_MSFT, xr::SceneObjectType::Floor },
        { XrSceneObjectTypeMSFT::XR_SCENE_OBJECT_TYPE_CEILING_MSFT, xr::SceneObjectType::Ceiling },
        { XrSceneObjectTypeMSFT::XR_SCENE_OBJECT_TYPE_PLATFORM_MSFT, xr::SceneObjectType::Platform },
        { XrSceneObjectTypeMSFT::XR_SCENE_OBJECT_TYPE_INFERRED_MSFT, xr::SceneObjectType::Inferred },
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