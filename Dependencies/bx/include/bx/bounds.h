/*
 * Copyright 2011-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#ifndef BX_BOUNDS_H_HEADER_GUARD
#define BX_BOUNDS_H_HEADER_GUARD

#include <bx/math.h>

namespace bx
{
	/// Infinite line defined by a point and direction.
	///
	struct Line
	{
		Vec3 pos = InitNone;
		Vec3 dir = InitNone;
	};

	/// Line segment defined by start and end points.
	///
	struct LineSegment
	{
		Vec3 pos = InitNone;
		Vec3 end = InitNone;
	};

	/// Axis-aligned bounding box.
	///
	struct Aabb
	{
		Vec3 min = InitNone;
		Vec3 max = InitNone;
	};

	/// Capsule defined by a line segment and radius.
	///
	struct Capsule
	{
		Vec3  pos = InitNone;
		Vec3  end = InitNone;
		float radius;
	};

	/// Cone defined by apex, base center, and base radius.
	///
	struct Cone
	{
		Vec3  pos = InitNone;
		Vec3  end = InitNone;
		float radius;
	};

	/// Cylinder defined by two end-cap centers and radius.
	///
	struct Cylinder
	{
		Vec3  pos = InitNone;
		Vec3  end = InitNone;
		float radius;
	};

	/// Disk defined by center, normal, and radius.
	///
	struct Disk
	{
		Vec3  center = InitNone;
		Vec3  normal = InitNone;
		float radius;
	};

	/// Oriented bounding box stored as a 4x4 matrix (rotation + scale + translation).
	///
	struct Obb
	{
		float mtx[16];
	};

	/// Sphere defined by center and radius.
	///
	struct Sphere
	{
		Vec3  center = InitNone;
		float radius;
	};

	/// Triangle defined by three vertices.
	///
	struct Triangle
	{
		Vec3 v0 = InitNone;
		Vec3 v1 = InitNone;
		Vec3 v2 = InitNone;
	};

	/// Ray defined by origin and direction.
	///
	struct Ray
	{
		Vec3 pos = InitNone;
		Vec3 dir = InitNone;
	};

	/// Ray-cast hit result containing position and surface plane.
	///
	struct Hit
	{
		Vec3  pos   = InitNone;
		Plane plane = InitNone;
	};

	/// 1D interval [min, max].
	///
	struct Interval
	{
		/// Construct interval with min = max = _val.
		/// @param _val Value to set both min and max to.
		Interval(float _val);

		/// Construct interval with explicit min and max.
		/// @param _min Minimum bound.
		/// @param _max Maximum bound.
		Interval(float _min, float _max);

		/// Set min = max = _val.
		/// @param _val Value to assign.
		void set(float _val);

		/// Shift interval so that its center is _val.
		/// @param _val New center value.
		void setCenter(float _val);

		/// Expand interval by _val in both directions.
		/// @param _val Expansion amount.
		void expand(float _val);

		float min;
		float max;
	};

	/// Returns point along a ray at parametric distance _t.
	/// @param _ray Source ray.
	/// @param _t Parametric distance along the ray.
	/// @return Point at position pos + dir * _t.
	Vec3 getPointAt(const Ray& _ray, float _t);

	/// Returns point along an infinite line at parametric distance _t.
	/// @param _line Source line.
	/// @param _t Parametric distance along the line.
	/// @return Point at position pos + dir * _t.
	Vec3 getPointAt(const Line& _line, float _t);

	/// Returns point along a line segment at parametric position _t in [0,1].
	/// @param _line Source line segment.
	/// @param _t Parametric position, 0 = start, 1 = end.
	/// @return Interpolated point on the segment.
	Vec3 getPointAt(const LineSegment& _line, float _t);

	/// Returns the center of an AABB.
	/// @param _aabb Axis-aligned bounding box.
	/// @return Center point (min + max) / 2.
	Vec3 getCenter(const Aabb& _aabb);

	/// Returns the half-extents of an AABB.
	/// @param _aabb Axis-aligned bounding box.
	/// @return Half-extents (max - min) / 2.
	Vec3 getExtents(const Aabb& _aabb);

	/// Returns the centroid of a triangle.
	/// @param _triangle Source triangle.
	/// @return Centroid (v0 + v1 + v2) / 3.
	Vec3 getCenter(const Triangle& _triangle);

	/// Convert half-extents to an AABB centered at the origin.
	/// @param[out] _outAabb Resulting axis-aligned bounding box.
	/// @param _extents Half-extents of the box.
	void toAabb(Aabb& _outAabb, const Vec3& _extents);

	/// Convert center and half-extents to an AABB.
	/// @param[out] _outAabb Resulting axis-aligned bounding box.
	/// @param _center Center of the box.
	/// @param _extents Half-extents of the box.
	void toAabb(Aabb& _outAabb, const Vec3& _center, const Vec3& _extents);

	/// Convert cylinder to axis aligned bounding box.
	/// @param[out] _outAabb Resulting axis-aligned bounding box.
	/// @param _cylinder Source cylinder.
	void toAabb(Aabb& _outAabb, const Cylinder& _cylinder);

	/// Convert disk to axis aligned bounding box.
	/// @param[out] _outAabb Resulting axis-aligned bounding box.
	/// @param _disk Source disk.
	void toAabb(Aabb& _outAabb, const Disk& _disk);

	/// Convert oriented bounding box to axis aligned bounding box.
	/// @param[out] _outAabb Resulting axis-aligned bounding box.
	/// @param _obb Source oriented bounding box.
	void toAabb(Aabb& _outAabb, const Obb& _obb);

	/// Convert sphere to axis aligned bounding box.
	/// @param[out] _outAabb Resulting axis-aligned bounding box.
	/// @param _sphere Source sphere.
	void toAabb(Aabb& _outAabb, const Sphere& _sphere);

	/// Convert triangle to axis aligned bounding box.
	/// @param[out] _outAabb Resulting axis-aligned bounding box.
	/// @param _triangle Source triangle.
	void toAabb(Aabb& _outAabb, const Triangle& _triangle);

	/// Calculate axis aligned bounding box.
	/// @param[out] _outAabb Resulting axis-aligned bounding box.
	/// @param _vertices Pointer to vertex data.
	/// @param _numVertices Number of vertices.
	/// @param _stride Byte stride between vertices.
	void toAabb(Aabb& _outAabb, const void* _vertices, uint32_t _numVertices, uint32_t _stride);

	/// Transform vertices and calculate axis aligned bounding box.
	/// @param[out] _outAabb Resulting axis-aligned bounding box.
	/// @param _mtx 4x4 transformation matrix.
	/// @param _vertices Pointer to vertex data.
	/// @param _numVertices Number of vertices.
	/// @param _stride Byte stride between vertices.
	void toAabb(Aabb& _outAabb, const float* _mtx, const void* _vertices, uint32_t _numVertices, uint32_t _stride);

	/// Expand AABB.
	/// @param _outAabb Axis-aligned bounding box to expand.
	/// @param _factor Expansion factor.
	void aabbExpand(Aabb& _outAabb, float _factor);

	/// Expand AABB with xyz.
	/// @param _outAabb Axis-aligned bounding box to expand.
	/// @param _pos Point to include in the bounding box.
	void aabbExpand(Aabb& _outAabb, const Vec3& _pos);

	/// Calculate surface area of axis aligned bounding box.
	/// @param _aabb Source axis-aligned bounding box.
	/// @return Surface area of the AABB.
	float calcAreaAabb(const Aabb& _aabb);

	/// Convert axis aligned bounding box to oriented bounding box.
	/// @param[out] _outObb Resulting oriented bounding box.
	/// @param _aabb Source axis-aligned bounding box.
	void toObb(Obb& _outObb, const Aabb& _aabb);

	/// Calculate oriented bounding box.
	/// @param[out] _outObb Resulting oriented bounding box.
	/// @param _vertices Pointer to vertex data.
	/// @param _numVertices Number of vertices.
	/// @param _stride Byte stride between vertices.
	/// @param _steps Number of optimization steps.
	void calcObb(Obb& _outObb, const void* _vertices, uint32_t _numVertices, uint32_t _stride, uint32_t _steps = 17);

	/// Calculate maximum bounding sphere.
	/// @param[out] _outSphere Resulting bounding sphere.
	/// @param _vertices Pointer to vertex data.
	/// @param _numVertices Number of vertices.
	/// @param _stride Byte stride between vertices.
	void calcMaxBoundingSphere(Sphere& _outSphere, const void* _vertices, uint32_t _numVertices, uint32_t _stride);

	/// Calculate minimum bounding sphere.
	/// @param[out] _outSphere Resulting bounding sphere.
	/// @param _vertices Pointer to vertex data.
	/// @param _numVertices Number of vertices.
	/// @param _stride Byte stride between vertices.
	/// @param _step Refinement step size.
	void calcMinBoundingSphere(Sphere& _outSphere, const void* _vertices, uint32_t _numVertices, uint32_t _stride, float _step = 0.01f);

	/// Returns 6 (near, far, left, right, top, bottom) planes representing frustum planes.
	/// @param[out] _outPlanes Array of 6 frustum planes.
	/// @param _viewProj 4x4 view-projection matrix.
	/// @param[in] _homogeneousNdc True for [-1,1] NDC (OpenGL), false for [0,1] (D3D).
	void buildFrustumPlanes(Plane* _outPlanes, const float* _viewProj, bool _homogeneousNdc);

	/// Returns point from 3 intersecting planes.
	/// @param _pa First plane.
	/// @param _pb Second plane.
	/// @param _pc Third plane.
	/// @return Intersection point.
	Vec3 intersectPlanes(const Plane& _pa, const Plane& _pb, const Plane& _pc);

	/// Make screen space ray from x, y coordinate and inverse view-projection matrix.
	/// @param _x Screen-space x coordinate.
	/// @param _y Screen-space y coordinate.
	/// @param _invVp 4x4 inverse view-projection matrix.
	/// @return Ray in world space.
	Ray makeRay(float _x, float _y, const float* _invVp);

	/// Intersect ray / AABB.
	/// @param _ray Source ray.
	/// @param _aabb Axis-aligned bounding box.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the AABB.
	bool intersect(const Ray& _ray, const Aabb& _aabb, Hit* _hit = NULL);

	/// Intersect ray / OBB.
	/// @param _ray Source ray.
	/// @param _obb Oriented bounding box.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the OBB.
	bool intersect(const Ray& _ray, const Obb& _obb, Hit* _hit = NULL);

	/// Intersect ray / cylinder.
	/// @param _ray Source ray.
	/// @param _cylinder Source cylinder.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the cylinder.
	bool intersect(const Ray& _ray, const Cylinder& _cylinder, Hit* _hit = NULL);

	/// Intersect ray / capsule.
	/// @param _ray Source ray.
	/// @param _capsule Source capsule.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the capsule.
	bool intersect(const Ray& _ray, const Capsule& _capsule, Hit* _hit = NULL);

	/// Intersect ray / cone.
	/// @param _ray Source ray.
	/// @param _cone Source cone.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the cone.
	bool intersect(const Ray& _ray, const Cone& _cone, Hit* _hit = NULL);

	/// Intersect ray / disk.
	/// @param _ray Source ray.
	/// @param _disk Source disk.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the disk.
	bool intersect(const Ray& _ray, const Disk& _disk, Hit* _hit = NULL);

	/// Intersect ray / plane.
	/// @param _ray Source ray.
	/// @param _plane Source plane.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the plane.
	bool intersect(const Ray& _ray, const Plane& _plane, Hit* _hit = NULL);

	/// Intersect ray / plane.
	/// @param _ray Source ray.
	/// @param _plane Source plane.
	/// @param _doublesided If true, intersect with both sides of the plane.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the plane.
	bool intersect(const Ray& _ray, const Plane& _plane, bool _doublesided, Hit* _hit = NULL);

	/// Intersect ray / sphere.
	/// @param _ray Source ray.
	/// @param _sphere Source sphere.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the sphere.
	bool intersect(const Ray& _ray, const Sphere& _sphere, Hit* _hit = NULL);

	/// Intersect ray / triangle.
	/// @param _ray Source ray.
	/// @param _triangle Source triangle.
	/// @param[out] _hit Optional hit result.
	/// @return True if the ray intersects the triangle.
	bool intersect(const Ray& _ray, const Triangle& _triangle, Hit* _hit = NULL);

	/// Returns the closest point on an infinite line to a given point.
	/// @param _line Source line.
	/// @param _point Query point.
	/// @return Closest point on the line.
	Vec3 closestPoint(const Line& _line, const Vec3& _point);

	/// Returns the closest point on a line segment to a given point.
	/// @param _line Source line segment.
	/// @param _point Query point.
	/// @return Closest point on the segment.
	Vec3 closestPoint(const LineSegment& _line, const Vec3& _point);

	/// Finds the closest pair of points between two line segments.
	/// @param _a First line segment.
	/// @param _b Second line segment.
	/// @param[out] _outS Parametric position [0,1] on segment _a.
	/// @param[out] _outT Parametric position [0,1] on segment _b.
	void closestPoint(const LineSegment& _a, const LineSegment& _b, float& _outS, float& _outT);

	/// Returns the closest point on a plane to a given point.
	/// @param _plane Source plane.
	/// @param _point Query point.
	/// @return Closest point on the plane.
	Vec3 closestPoint(const Plane& _plane, const Vec3& _point);

	/// Returns the closest point on an AABB to a given point.
	/// @param _aabb Source axis-aligned bounding box.
	/// @param _point Query point.
	/// @return Closest point on the AABB.
	Vec3 closestPoint(const Aabb& _aabb, const Vec3& _point);

	/// Returns the closest point on an OBB to a given point.
	/// @param _obb Source oriented bounding box.
	/// @param _point Query point.
	/// @return Closest point on the OBB.
	Vec3 closestPoint(const Obb& _obb, const Vec3& _point);

	/// Returns the closest point on a decomposed OBB (center, half-extents, rotation) to a given point.
	/// @param _center OBB center.
	/// @param _halfExtents OBB half-extents along each local axis.
	/// @param _rotation OBB orientation.
	/// @param _point Query point.
	/// @return Closest point on the OBB.
	Vec3 closestPoint(const Vec3& _center, const Vec3& _halfExtents, const Quaternion& _rotation, const Vec3& _point);

	/// Returns the closest point on a triangle to a given point.
	/// @param _triangle Source triangle.
	/// @param _point Query point.
	/// @return Closest point on the triangle.
	Vec3 closestPoint(const Triangle& _triangle, const Vec3& _point);

	/// Test overlap between an interval and a point.
	/// @param _interval Source interval.
	/// @param _pos Point position.
	/// @return True if the point is within the interval.
	bool overlap(const Interval& _interval, float _pos);

	/// Test overlap between two intervals.
	/// @param _intervalA First interval.
	/// @param _intervalB Second interval.
	/// @return True if the intervals overlap.
	bool overlap(const Interval& _intervalA, const Interval& _intervalB);

	/// Test overlap between an AABB and a point.
	/// @param _aabb Axis-aligned bounding box.
	/// @param _pos Point position.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabb, const Vec3& _pos);

	/// Test overlap between an AABB and a sphere.
	/// @param _aabb Axis-aligned bounding box.
	/// @param _sphere Source sphere.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabb, const Sphere& _sphere);

	/// Test overlap between two AABBs.
	/// @param _aabbA First axis-aligned bounding box.
	/// @param _aabbB Second axis-aligned bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabbA, const Aabb& _aabbB);

	/// Test overlap between an AABB and a plane.
	/// @param _aabb Axis-aligned bounding box.
	/// @param _plane Source plane.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabb, const Plane& _plane);

	/// Test overlap between an AABB and a triangle.
	/// @param _aabb Axis-aligned bounding box.
	/// @param _triangle Source triangle.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabb, const Triangle& _triangle);

	/// Test overlap between an AABB and a cylinder.
	/// @param _aabb Axis-aligned bounding box.
	/// @param _cylinder Source cylinder.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabb, const Cylinder& _cylinder);

	/// Test overlap between an AABB and a capsule.
	/// @param _aabb Axis-aligned bounding box.
	/// @param _capsule Source capsule.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabb, const Capsule& _capsule);

	/// Test overlap between an AABB and a cone.
	/// @param _aabb Axis-aligned bounding box.
	/// @param _cone Source cone.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabb, const Cone& _cone);

	/// Test overlap between an AABB and a disk.
	/// @param _aabb Axis-aligned bounding box.
	/// @param _disk Source disk.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabb, const Disk& _disk);

	/// Test overlap between an AABB and an OBB.
	/// @param _aabb Axis-aligned bounding box.
	/// @param _obb Oriented bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Aabb& _aabb, const Obb& _obb);

	/// Test overlap between a capsule and a point.
	/// @param _capsule Source capsule.
	/// @param _pos Point position.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsule, const Vec3& _pos);

	/// Test overlap between a capsule and a sphere.
	/// @param _capsule Source capsule.
	/// @param _sphere Source sphere.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsule, const Sphere& _sphere);

	/// Test overlap between a capsule and an AABB.
	/// @param _capsule Source capsule.
	/// @param _aabb Axis-aligned bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsule, const Aabb& _aabb);

	/// Test overlap between a capsule and a plane.
	/// @param _capsule Source capsule.
	/// @param _plane Source plane.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsule, const Plane& _plane);

	/// Test overlap between a capsule and a triangle.
	/// @param _capsule Source capsule.
	/// @param _triangle Source triangle.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsule, const Triangle& _triangle);

	/// Test overlap between a capsule and a cylinder.
	/// @param _capsule Source capsule.
	/// @param _cylinder Source cylinder.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsule, const Cylinder& _cylinder);

	/// Test overlap between two capsules.
	/// @param _capsuleA First capsule.
	/// @param _capsuleB Second capsule.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsuleA, const Capsule& _capsuleB);

	/// Test overlap between a capsule and a cone.
	/// @param _capsule Source capsule.
	/// @param _cone Source cone.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsule, const Cone& _cone);

	/// Test overlap between a capsule and a disk.
	/// @param _capsule Source capsule.
	/// @param _disk Source disk.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsule, const Disk& _disk);

	/// Test overlap between a capsule and an OBB.
	/// @param _capsule Source capsule.
	/// @param _obb Oriented bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Capsule& _capsule, const Obb& _obb);

	/// Test overlap between a cone and a point.
	/// @param _cone Source cone.
	/// @param _pos Point position.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _cone, const Vec3& _pos);

	/// Test overlap between a cone and a sphere.
	/// @param _cone Source cone.
	/// @param _sphere Source sphere.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _cone, const Sphere& _sphere);

	/// Test overlap between a cone and an AABB.
	/// @param _cone Source cone.
	/// @param _aabb Axis-aligned bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _cone, const Aabb& _aabb);

	/// Test overlap between a cone and a plane.
	/// @param _cone Source cone.
	/// @param _plane Source plane.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _cone, const Plane& _plane);

	/// Test overlap between a cone and a triangle.
	/// @param _cone Source cone.
	/// @param _triangle Source triangle.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _cone, const Triangle& _triangle);

	/// Test overlap between a cone and a cylinder.
	/// @param _cone Source cone.
	/// @param _cylinder Source cylinder.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _cone, const Cylinder& _cylinder);

	/// Test overlap between a cone and a capsule.
	/// @param _cone Source cone.
	/// @param _capsule Source capsule.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _cone, const Capsule& _capsule);

	/// Test overlap between two cones.
	/// @param _coneA First cone.
	/// @param _coneB Second cone.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _coneA, const Cone& _coneB);

	/// Test overlap between a cone and a disk.
	/// @param _cone Source cone.
	/// @param _disk Source disk.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _cone, const Disk& _disk);

	/// Test overlap between a cone and an OBB.
	/// @param _cone Source cone.
	/// @param _obb Oriented bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Cone& _cone, const Obb& _obb);

	/// Test overlap between a cylinder and a point.
	/// @param _cylinder Source cylinder.
	/// @param _pos Point position.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinder, const Vec3& _pos);

	/// Test overlap between a cylinder and a sphere.
	/// @param _cylinder Source cylinder.
	/// @param _sphere Source sphere.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinder, const Sphere& _sphere);

	/// Test overlap between a cylinder and an AABB.
	/// @param _cylinder Source cylinder.
	/// @param _aabb Axis-aligned bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinder, const Aabb& _aabb);

	/// Test overlap between a cylinder and a plane.
	/// @param _cylinder Source cylinder.
	/// @param _plane Source plane.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinder, const Plane& _plane);

	/// Test overlap between a cylinder and a triangle.
	/// @param _cylinder Source cylinder.
	/// @param _triangle Source triangle.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinder, const Triangle& _triangle);

	/// Test overlap between two cylinders.
	/// @param _cylinderA First cylinder.
	/// @param _cylinderB Second cylinder.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinderA, const Cylinder& _cylinderB);

	/// Test overlap between a cylinder and a capsule.
	/// @param _cylinder Source cylinder.
	/// @param _capsule Source capsule.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinder, const Capsule& _capsule);

	/// Test overlap between a cylinder and a cone.
	/// @param _cylinder Source cylinder.
	/// @param _cone Source cone.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinder, const Cone& _cone);

	/// Test overlap between a cylinder and a disk.
	/// @param _cylinder Source cylinder.
	/// @param _disk Source disk.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinder, const Disk& _disk);

	/// Test overlap between a cylinder and an OBB.
	/// @param _cylinder Source cylinder.
	/// @param _obb Oriented bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Cylinder& _cylinder, const Obb& _obb);

	/// Test overlap between a disk and a point.
	/// @param _disk Source disk.
	/// @param _pos Point position.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _disk, const Vec3& _pos);

	/// Test overlap between a disk and a sphere.
	/// @param _disk Source disk.
	/// @param _sphere Source sphere.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _disk, const Sphere& _sphere);

	/// Test overlap between a disk and an AABB.
	/// @param _disk Source disk.
	/// @param _aabb Axis-aligned bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _disk, const Aabb& _aabb);

	/// Test overlap between a disk and a plane.
	/// @param _disk Source disk.
	/// @param _plane Source plane.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _disk, const Plane& _plane);

	/// Test overlap between a disk and a triangle.
	/// @param _disk Source disk.
	/// @param _triangle Source triangle.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _disk, const Triangle& _triangle);

	/// Test overlap between a disk and a cylinder.
	/// @param _disk Source disk.
	/// @param _cylinder Source cylinder.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _disk, const Cylinder& _cylinder);

	/// Test overlap between a disk and a capsule.
	/// @param _disk Source disk.
	/// @param _capsule Source capsule.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _disk, const Capsule& _capsule);

	/// Test overlap between a disk and a cone.
	/// @param _disk Source disk.
	/// @param _cone Source cone.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _disk, const Cone& _cone);

	/// Test overlap between two disks.
	/// @param _diskA First disk.
	/// @param _diskB Second disk.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _diskA, const Disk& _diskB);

	/// Test overlap between a disk and an OBB.
	/// @param _disk Source disk.
	/// @param _obb Oriented bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Disk& _disk, const Obb& _obb);

	/// Test overlap between an OBB and a point.
	/// @param _obb Oriented bounding box.
	/// @param _pos Point position.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obb, const Vec3& _pos);

	/// Test overlap between an OBB and a sphere.
	/// @param _obb Oriented bounding box.
	/// @param _sphere Source sphere.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obb, const Sphere& _sphere);

	/// Test overlap between an OBB and an AABB.
	/// @param _obb Oriented bounding box.
	/// @param _aabb Axis-aligned bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obb, const Aabb& _aabb);

	/// Test overlap between an OBB and a plane.
	/// @param _obb Oriented bounding box.
	/// @param _plane Source plane.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obb, const Plane& _plane);

	/// Test overlap between an OBB and a triangle.
	/// @param _obb Oriented bounding box.
	/// @param _triangle Source triangle.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obb, const Triangle& _triangle);

	/// Test overlap between an OBB and a cylinder.
	/// @param _obb Oriented bounding box.
	/// @param _cylinder Source cylinder.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obb, const Cylinder& _cylinder);

	/// Test overlap between an OBB and a capsule.
	/// @param _obb Oriented bounding box.
	/// @param _capsule Source capsule.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obb, const Capsule& _capsule);

	/// Test overlap between an OBB and a cone.
	/// @param _obb Oriented bounding box.
	/// @param _cone Source cone.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obb, const Cone& _cone);

	/// Test overlap between an OBB and a disk.
	/// @param _obb Oriented bounding box.
	/// @param _disk Source disk.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obb, const Disk& _disk);

	/// Test overlap between two OBBs.
	/// @param _obbA First oriented bounding box.
	/// @param _obbB Second oriented bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Obb& _obbA, const Obb& _obbB);

	/// Test overlap between a plane and a point.
	/// @param _plane Source plane.
	/// @param _pos Point position.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _plane, const Vec3& _pos);

	/// Test overlap between a plane and a sphere.
	/// @param _plane Source plane.
	/// @param _sphere Source sphere.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _plane, const Sphere& _sphere);

	/// Test overlap between a plane and an AABB.
	/// @param _plane Source plane.
	/// @param _aabb Axis-aligned bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _plane, const Aabb& _aabb);

	/// Test overlap between two planes.
	/// @param _planeA First plane.
	/// @param _planeB Second plane.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _planeA, const Plane& _planeB);

	/// Test overlap between a plane and a triangle.
	/// @param _plane Source plane.
	/// @param _triangle Source triangle.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _plane, const Triangle& _triangle);

	/// Test overlap between a plane and a cylinder.
	/// @param _plane Source plane.
	/// @param _cylinder Source cylinder.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _plane, const Cylinder& _cylinder);

	/// Test overlap between a plane and a capsule.
	/// @param _plane Source plane.
	/// @param _capsule Source capsule.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _plane, const Capsule& _capsule);

	/// Test overlap between a plane and a cone.
	/// @param _plane Source plane.
	/// @param _cone Source cone.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _plane, const Cone& _cone);

	/// Test overlap between a plane and a disk.
	/// @param _plane Source plane.
	/// @param _disk Source disk.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _plane, const Disk& _disk);

	/// Test overlap between a plane and an OBB.
	/// @param _plane Source plane.
	/// @param _obb Oriented bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Plane& _plane, const Obb& _obb);

	/// Test overlap between a sphere and a point.
	/// @param _sphere Source sphere.
	/// @param _pos Point position.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphere, const Vec3& _pos);

	/// Test overlap between two spheres.
	/// @param _sphereA First sphere.
	/// @param _sphereB Second sphere.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphereA, const Sphere& _sphereB);

	/// Test overlap between a sphere and an AABB.
	/// @param _sphere Source sphere.
	/// @param _aabb Axis-aligned bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphere, const Aabb& _aabb);

	/// Test overlap between a sphere and a plane.
	/// @param _sphere Source sphere.
	/// @param _plane Source plane.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphere, const Plane& _plane);

	/// Test overlap between a sphere and a triangle.
	/// @param _sphere Source sphere.
	/// @param _triangle Source triangle.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphere, const Triangle& _triangle);

	/// Test overlap between a sphere and a cylinder.
	/// @param _sphere Source sphere.
	/// @param _cylinder Source cylinder.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphere, const Cylinder& _cylinder);

	/// Test overlap between a sphere and a capsule.
	/// @param _sphere Source sphere.
	/// @param _capsule Source capsule.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphere, const Capsule& _capsule);

	/// Test overlap between a sphere and a cone.
	/// @param _sphere Source sphere.
	/// @param _cone Source cone.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphere, const Cone& _cone);

	/// Test overlap between a sphere and a disk.
	/// @param _sphere Source sphere.
	/// @param _disk Source disk.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphere, const Disk& _disk);

	/// Test overlap between a sphere and an OBB.
	/// @param _sphere Source sphere.
	/// @param _obb Oriented bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Sphere& _sphere, const Obb& _obb);

	/// Test overlap between a triangle and a point.
	/// @param _triangle Source triangle.
	/// @param _pos Point position.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangle, const Vec3& _pos);

	/// Test overlap between a triangle and a sphere.
	/// @param _triangle Source triangle.
	/// @param _sphere Source sphere.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangle, const Sphere& _sphere);

	/// Test overlap between a triangle and an AABB.
	/// @param _triangle Source triangle.
	/// @param _aabb Axis-aligned bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangle, const Aabb& _aabb);

	/// Test overlap between a triangle and a plane.
	/// @param _triangle Source triangle.
	/// @param _plane Source plane.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangle, const Plane& _plane);

	/// Test overlap between two triangles.
	/// @param _triangleA First triangle.
	/// @param _triangleB Second triangle.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangleA, const Triangle& _triangleB);

	/// Test overlap between a triangle and a cylinder.
	/// @param _triangle Source triangle.
	/// @param _cylinder Source cylinder.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangle, const Cylinder& _cylinder);

	/// Test overlap between a triangle and a capsule.
	/// @param _triangle Source triangle.
	/// @param _capsule Source capsule.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangle, const Capsule& _capsule);

	/// Test overlap between a triangle and a cone.
	/// @param _triangle Source triangle.
	/// @param _cone Source cone.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangle, const Cone& _cone);

	/// Test overlap between a triangle and a disk.
	/// @param _triangle Source triangle.
	/// @param _disk Source disk.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangle, const Disk& _disk);

	/// Test overlap between a triangle and an OBB.
	/// @param _triangle Source triangle.
	/// @param _obb Oriented bounding box.
	/// @return True if the primitives overlap.
	bool overlap(const Triangle& _triangle, const Obb& _obb);

} // namespace bx

#include "inline/bounds.inl"

#endif // BX_BOUNDS_H_HEADER_GUARD
