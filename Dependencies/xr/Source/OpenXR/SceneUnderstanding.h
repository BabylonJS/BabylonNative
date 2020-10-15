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
			const XrSession& Session;
			const XrSupportedExtensions& Extensions;
			const DetectionBoundaryType DetectionBoundaryType;
			const float SphereRadius;
			const XrVector3f BoxDimensions;
			const XrTime UpdateIntervalInSeconds;

			InitOptions(
				const XrSession& session,
				const XrSupportedExtensions& extensions);
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

		SceneUnderstanding();
		~SceneUnderstanding();
		void Initialize(const InitOptions options) const;
		void Update() const;
		void UpdateFrame(UpdateFrameArgs args) const;
		System::Session::Frame::Plane& TryGetPlaneByID(const System::Session::Frame::Plane::Identifier id) const;

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}