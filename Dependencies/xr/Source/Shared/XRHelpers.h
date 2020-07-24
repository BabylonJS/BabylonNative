#pragma once
#include <XR.h>

namespace xr
{
	/**
	 * Compare two poses and determine if there is a meaningful difference between them.
	 **/
	constexpr float FLOAT_COMPARISON_THRESHOLD = 0.02f;
	bool PoseWasMeaningfullyUpdated(const Pose& lhs, const Pose& rhs)
	{
		return abs(lhs.Position.X - rhs.Position.X) > FLOAT_COMPARISON_THRESHOLD
			|| abs(lhs.Position.Y - rhs.Position.Y) > FLOAT_COMPARISON_THRESHOLD
			|| abs(lhs.Position.Z - rhs.Position.Z) > FLOAT_COMPARISON_THRESHOLD
			|| abs(lhs.Orientation.X - rhs.Orientation.X) > FLOAT_COMPARISON_THRESHOLD
			|| abs(lhs.Orientation.Y - rhs.Orientation.Y) > FLOAT_COMPARISON_THRESHOLD
			|| abs(lhs.Orientation.Z - rhs.Orientation.Z) > FLOAT_COMPARISON_THRESHOLD
			|| abs(lhs.Orientation.W - rhs.Orientation.W) > FLOAT_COMPARISON_THRESHOLD;
	}
	
	bool CheckIfPlaneWasUpdated(Plane& existingPlane, std::vector<float>& newPolygon, Pose& newCenter)
	{
		// First check if the center has changed, or the polygon size has changed.
		if (PoseWasMeaningfullyUpdated(existingPlane.Center, newCenter)
			|| existingPlane.Polygon.size() != newPolygon.size())
		{
			return true;
		}

		// Next loop over the polygon and check if any points have meaningfully changed.
		for (size_t i = 0; i < newPolygon.size(); i++)
		{
			if (abs(existingPlane.Polygon[i] - newPolygon[i]) > FLOAT_COMPARISON_THRESHOLD)
			{
				return true;
			}
		}

		return false;
	}
}
