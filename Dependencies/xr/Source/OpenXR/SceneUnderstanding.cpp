#include "SceneUnderstanding.h"
#include "XrRegistry.h"

using namespace xr;

constexpr SceneUnderstanding::DetectionBoundaryType DEFAULT_BOUNDARY_TYPE = SceneUnderstanding::DetectionBoundaryType::Sphere;
constexpr float DEFAULT_SPHERE_RADIUS = 5.f;

SceneUnderstanding::SceneUnderstanding::InitOptions::InitOptions()
	: DetectionBoundaryType(DEFAULT_BOUNDARY_TYPE)
	, SphereRadius(DEFAULT_SPHERE_RADIUS)
	, BoxDimensions(XrVector3f{ 0, 0, 0 }) {}

SceneUnderstanding::SceneUnderstanding::UpdateFrameArgs::UpdateFrameArgs(
	std::vector<System::Session::Frame::Plane>& planes,
	std::vector<System::Session::Frame::Plane::Identifier>& updatedPlanes,
	std::vector<System::Session::Frame::Plane::Identifier>& removedPlanes)
	: Planes(planes)
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
		const auto& extensions = XrRegistry::Context().Extensions();
		if (!extensions->SceneUnderstandingSupported)
		{
			return;
		}

		// TODO
		//* Create an XrSceneObserverMSFT handle to mange the system resource of the scene understanding compute.
		//* Start the scene compute by calling xrComputeNewSceneMSFT with XrSceneBoundsMSFT to limit the scan range.
		//* Inspect the completion of computation polling xrGetSceneComputeStateMSFT.
		//* Once compute is completed, create a handle to the result by calling xrCreateSceneMSFT.
		//* Get a list of objects in the scene using xrGetSceneObjectsMSFT.
		//* Get the properties of each scene object, such as object kind, using xrGetSceneObjectPropertiesMSFT.
		//* Locate scene objects using xrLocateSceneObjectsMSFT.
		//* Each object contains multiple planes and multiple meshes.
		//* Each object, plane or mesh is identified by atoms (uint64_t) that’s unique in the XrSceneMSFT.
		//* Get the properties of each plane, such as plane equation, alignment and bounds, using xrGetScenePlanePropertiesMSFT.
		//* Get the properties of each mesh, such as vertices and indices buffer, using xrGetSceneMeshMSFT.

			m_initialized = true;
	}

	void UpdateFrame(UpdateFrameArgs& args)
	{
		if (!m_initialized)
		{
			return;
		}
	}

	System::Session::Frame::Plane& TryGetPlaneByID(const System::Session::Frame::Plane::Identifier id)
	{
		throw std::exception(/*TODO*/);
	}

private:
	bool m_initialized{ false };
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