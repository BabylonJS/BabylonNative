#pragma once

#include "XrSupportedExtensions.h"

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
			DetectionBoundaryType DetectionBoundaryType;
			float SphereRadius;
			XrVector3f BoxDimensions;

			InitOptions();
		};

		struct UpdateFrameArgs
		{
			std::vector<System::Session::Frame::Plane>& Planes;
			std::vector<System::Session::Frame::Plane::Identifier>& UpdatedPlanes;
			std::vector<System::Session::Frame::Plane::Identifier>& RemovedPlanes;

			UpdateFrameArgs(
				std::vector<System::Session::Frame::Plane>& planes,
				std::vector<System::Session::Frame::Plane::Identifier>& updatedPlanes,
				std::vector<System::Session::Frame::Plane::Identifier>& removedPlanes);
		};

		SceneUnderstanding();
		~SceneUnderstanding();
		void Initialize(const InitOptions options) const;
		void UpdateFrame(UpdateFrameArgs args) const;
		System::Session::Frame::Plane& TryGetPlaneByID(const System::Session::Frame::Plane::Identifier id) const;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}