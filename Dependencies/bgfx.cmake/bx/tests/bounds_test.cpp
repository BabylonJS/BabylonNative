/*
 * Copyright 2010-2026 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bx/blob/master/LICENSE
 */

#include "test.h"
#include <bx/bounds.h>
#include <bx/math.h>

static bx::Cylinder makeCylinder(const bx::Vec3& _center, float _halfHeight, float _radius, const bx::Vec3& _axis = {0.0f, 1.0f, 0.0f})
{
	const bx::Vec3 half = bx::mul(_axis, _halfHeight);
	return { bx::sub(_center, half), bx::add(_center, half), _radius };
}

static bx::Capsule makeCapsule(const bx::Vec3& _center, float _halfHeight, float _radius, const bx::Vec3& _axis = {0.0f, 1.0f, 0.0f})
{
	const bx::Vec3 half = bx::mul(_axis, _halfHeight);
	return { bx::sub(_center, half), bx::add(_center, half), _radius };
}

static bx::Cone makeCone(const bx::Vec3& _base, float _height, float _radius, const bx::Vec3& _axis = {0.0f, 1.0f, 0.0f})
{
	return { _base, bx::add(_base, bx::mul(_axis, _height) ), _radius };
}

static bx::Plane makePlaneY(float _y)
{
	return { {0.0f, 1.0f, 0.0f}, -_y };
}

static bx::Obb makeObb(const bx::Vec3& _center, const bx::Vec3& _halfExtents)
{
	bx::Aabb aabb;
	bx::toAabb(aabb, _center, _halfExtents);
	bx::Obb obb;
	bx::toObb(obb, aabb);
	return obb;
}

TEST_CASE("toAabb from sphere", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, bx::Sphere{ {1.0f, 2.0f, 3.0f}, 0.5f });

	REQUIRE(bx::isEqual(aabb.min, { 0.5f, 1.5f, 2.5f}, 0.0001f) );
	REQUIRE(bx::isEqual(aabb.max, { 1.5f, 2.5f, 3.5f}, 0.0001f) );
}

TEST_CASE("toAabb from cylinder", "[bounds]")
{
	const bx::Vec3 axis = bx::normalize(bx::Vec3{1.0f, 1.0f, 0.0f});
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 1.0f, 0.5f, axis);
	bx::Aabb aabb;
	bx::toAabb(aabb, cyl);

	REQUIRE(bx::isFinite(aabb.min.x) );
	REQUIRE(bx::isFinite(aabb.max.y) );
	REQUIRE(aabb.min.y <= -0.5f);
	REQUIRE(aabb.max.y >=  0.5f);
}

TEST_CASE("toAabb from triangle", "[bounds]")
{
	bx::Aabb aabb;
	const bx::Triangle tri = { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} };
	bx::toAabb(aabb, tri);
	REQUIRE(bx::isEqual(aabb.min, {0.0f, 0.0f, 0.0f}, 0.0001f) );
	REQUIRE(bx::isEqual(aabb.max, {1.0f, 1.0f, 0.0f}, 0.0001f) );
}

TEST_CASE("closestPoint on line", "[bounds]")
{
	const bx::Line line = { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	const bx::Vec3 cp = bx::closestPoint(line, {5.0f, 3.0f, 0.0f});
	REQUIRE(bx::isEqual(cp, {5.0f, 0.0f, 0.0f}, 0.0001f) );
}

TEST_CASE("closestPoint on line segment", "[bounds]")
{
	const bx::LineSegment seg = { {0.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f} };

	REQUIRE(bx::isEqual(bx::closestPoint(seg, { 5.0f, 3.0f, 0.0f}), { 5.0f, 0.0f, 0.0f}, 0.0001f) );
	REQUIRE(bx::isEqual(bx::closestPoint(seg, {-5.0f, 0.0f, 0.0f}), { 0.0f, 0.0f, 0.0f}, 0.0001f) );
	REQUIRE(bx::isEqual(bx::closestPoint(seg, {15.0f, 0.0f, 0.0f}), {10.0f, 0.0f, 0.0f}, 0.0001f) );
}

TEST_CASE("closestPoint on plane", "[bounds]")
{
	const bx::Plane plane = makePlaneY(0.0f);
	const bx::Vec3 cp = bx::closestPoint(plane, {3.0f, 5.0f, -2.0f});
	REQUIRE(bx::isEqual(cp, {3.0f, 0.0f, -2.0f}, 0.0001f) );
}

TEST_CASE("closestPoint on bx::Aabb", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	REQUIRE(bx::isEqual(bx::closestPoint(aabb, {0.5f, 0.5f, 0.5f}), {0.5f, 0.5f, 0.5f}, 0.0001f) );
	REQUIRE(bx::isEqual(bx::closestPoint(aabb, {2.0f, 0.0f, 0.0f}), {1.0f, 0.0f, 0.0f}, 0.0001f) );
}

TEST_CASE("overlap bx::Aabb/bx::Vec3", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE( bx::overlap(aabb, bx::Vec3{0.0f, 0.0f, 0.0f}) );
	REQUIRE(!bx::overlap(aabb, bx::Vec3{2.0f, 0.0f, 0.0f}) );
}

TEST_CASE("overlap bx::Aabb/bx::Aabb", "[bounds]")
{
	bx::Aabb a, b;
	bx::toAabb(a, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	bx::toAabb(b, {0.5f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(bx::overlap(a, b) );

	bx::toAabb(b, {3.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(!bx::overlap(a, b) );
}

TEST_CASE("overlap bx::Aabb/bx::Sphere", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE( bx::overlap(aabb, bx::Sphere{ {0.0f, 0.0f, 0.0f}, 0.5f }) );
	REQUIRE( bx::overlap(aabb, bx::Sphere{ {1.5f, 0.0f, 0.0f}, 1.0f }) );
	REQUIRE(!bx::overlap(aabb, bx::Sphere{ {3.0f, 0.0f, 0.0f}, 0.5f }) );
}

TEST_CASE("overlap bx::Aabb/bx::Plane", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE( bx::overlap(aabb, makePlaneY(0.0f) ) );
	REQUIRE(!bx::overlap(aabb, makePlaneY(5.0f) ) );
}

TEST_CASE("overlap bx::Sphere/bx::Sphere", "[bounds]")
{
	REQUIRE( bx::overlap(bx::Sphere{ {0.0f, 0.0f, 0.0f}, 1.0f }, bx::Sphere{ {1.0f, 0.0f, 0.0f}, 1.0f }) );
	REQUIRE(!bx::overlap(bx::Sphere{ {0.0f, 0.0f, 0.0f}, 0.5f }, bx::Sphere{ {3.0f, 0.0f, 0.0f}, 0.5f }) );
}

TEST_CASE("overlap bx::Sphere/bx::Plane", "[bounds]")
{
	REQUIRE( bx::overlap(bx::Sphere{ {0.0f, 0.0f, 0.0f}, 1.0f }, makePlaneY(0.0f) ) );
	REQUIRE(!bx::overlap(bx::Sphere{ {0.0f, 5.0f, 0.0f}, 1.0f }, makePlaneY(0.0f) ) );
}

TEST_CASE("overlap bx::Plane/bx::Plane", "[bounds]")
{
	const bx::Plane pY = makePlaneY(0.0f);
	const bx::Plane pX = { {1.0f, 0.0f, 0.0f}, 0.0f };
	REQUIRE(bx::overlap(pY, pX) );

	REQUIRE(!bx::overlap(pY, makePlaneY(5.0f) ) );
}

TEST_CASE("overlap bx::Capsule/bx::Sphere", "[bounds]")
{
	const bx::Capsule cap = makeCapsule({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f);
	REQUIRE( bx::overlap(cap, bx::Sphere{ {0.0f, 0.0f, 0.0f}, 0.1f }) );
	REQUIRE(!bx::overlap(cap, bx::Sphere{ {5.0f, 0.0f, 0.0f}, 0.1f }) );
}

TEST_CASE("overlap bx::Capsule/bx::Capsule", "[bounds]")
{
	const bx::Capsule a = makeCapsule({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f);
	const bx::Capsule b = makeCapsule({0.8f, 0.0f, 0.0f}, 2.0f, 0.5f);
	REQUIRE(bx::overlap(a, b) );

	const bx::Capsule c = makeCapsule({5.0f, 0.0f, 0.0f}, 2.0f, 0.5f);
	REQUIRE(!bx::overlap(a, c) );
}

TEST_CASE("overlap bx::Obb/bx::Vec3", "[bounds]")
{
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE( bx::overlap(obb, bx::Vec3{0.0f, 0.0f, 0.0f}) );
	REQUIRE(!bx::overlap(obb, bx::Vec3{2.0f, 0.0f, 0.0f}) );
}

TEST_CASE("overlap bx::Obb/bx::Sphere", "[bounds]")
{
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE( bx::overlap(obb, bx::Sphere{ {0.0f, 0.0f, 0.0f}, 0.5f }) );
	REQUIRE(!bx::overlap(obb, bx::Sphere{ {5.0f, 0.0f, 0.0f}, 0.5f }) );
}

TEST_CASE("overlap bx::Obb/bx::Plane", "[bounds]")
{
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE( bx::overlap(obb, makePlaneY(0.0f) ) );
	REQUIRE(!bx::overlap(obb, makePlaneY(5.0f) ) );
}

TEST_CASE("overlap bx::Obb/bx::Obb", "[bounds]")
{
	const bx::Obb a = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	const bx::Obb c = makeObb({10.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	REQUIRE(!bx::overlap(a, c) );
}

TEST_CASE("overlap bx::Cylinder/bx::Vec3", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 1.0f, 0.5f);
	REQUIRE( bx::overlap(cyl, bx::Vec3{0.0f, 0.0f, 0.0f}) );
	REQUIRE(!bx::overlap(cyl, bx::Vec3{5.0f, 0.0f, 0.0f}) );
}

TEST_CASE("overlap bx::Cylinder/bx::Sphere", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 1.0f, 0.5f);
	REQUIRE( bx::overlap(cyl, bx::Sphere{ {0.0f, 0.0f, 0.0f}, 0.1f }) );
	REQUIRE(!bx::overlap(cyl, bx::Sphere{ {5.0f, 0.0f, 0.0f}, 0.1f }) );
}

TEST_CASE("overlap bx::Cylinder/bx::Plane", "[bounds]")
{
	const bx::Cylinder upright = makeCylinder({0.0f, 1.5f, 0.0f}, 1.0f, 0.5f);
	const bx::Plane ground = makePlaneY(0.0f);
	REQUIRE(!bx::overlap(upright, ground) ); // 0.5 above plane.

	const bx::Cylinder touching = makeCylinder({0.0f, 1.0f, 0.0f}, 1.0f, 0.5f);
	REQUIRE(bx::overlap(touching, ground) );

	const bx::Cylinder high = makeCylinder({0.0f, 10.0f, 0.0f}, 1.0f, 0.5f);
	REQUIRE(!bx::overlap(high, ground) );

	const bx::Cylinder sideways = makeCylinder({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f, {1.0f, 0.0f, 0.0f});
	REQUIRE(bx::overlap(sideways, ground) ); // bx::Disk rim at y=+/-0.5.

	const bx::Cylinder nearAbove = makeCylinder({0.0f, 0.3f, 0.0f}, 2.0f, 0.5f, {1.0f, 0.0f, 0.0f});
	REQUIRE(bx::overlap(nearAbove, ground) );

	const bx::Cylinder farAbove = makeCylinder({0.0f, 1.0f, 0.0f}, 2.0f, 0.5f, {1.0f, 0.0f, 0.0f});
	REQUIRE(!bx::overlap(farAbove, ground) );

	const bx::Vec3 axis45 = bx::normalize(bx::Vec3{1.0f, 1.0f, 0.0f});
	const bx::Cylinder tilted = makeCylinder({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f, axis45);
	REQUIRE(bx::overlap(tilted, ground) );

	const bx::Cylinder straddling = makeCylinder({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f);
	REQUIRE(bx::overlap(straddling, ground) );
}

TEST_CASE("overlap bx::Cylinder/bx::Cylinder", "[bounds]")
{
	const bx::Cylinder a = makeCylinder({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f);
	const bx::Cylinder b = makeCylinder({0.8f, 0.0f, 0.0f}, 2.0f, 0.5f);
	REQUIRE(bx::overlap(a, b) );

	const bx::Cylinder c = makeCylinder({5.0f, 0.0f, 0.0f}, 2.0f, 0.5f);
	REQUIRE(!bx::overlap(a, c) );

	const bx::Cylinder d = makeCylinder({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f, {1.0f, 0.0f, 0.0f});
	REQUIRE(bx::overlap(a, d) );

	const bx::Cylinder e = makeCylinder({0.9f, 0.0f, 0.0f}, 2.0f, 0.5f, {0.0f, 0.0f, 1.0f});
	REQUIRE(bx::overlap(a, e) );

	const bx::Cylinder f = makeCylinder({1.5f, 0.0f, 0.0f}, 2.0f, 0.5f, {0.0f, 0.0f, 1.0f});
	REQUIRE(!bx::overlap(a, f) );
}

TEST_CASE("overlap bx::Cylinder/bx::Capsule", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f);

	const bx::Capsule cap = makeCapsule({0.8f, 0.0f, 0.0f}, 2.0f, 0.5f);
	REQUIRE(bx::overlap(cyl, cap) );

	const bx::Capsule far = makeCapsule({5.0f, 0.0f, 0.0f}, 2.0f, 0.5f);
	REQUIRE(!bx::overlap(cyl, far) );

	const bx::Capsule perp = makeCapsule({0.0f, 0.0f, 0.0f}, 2.0f, 0.3f, {1.0f, 0.0f, 0.0f});
	REQUIRE(bx::overlap(cyl, perp) );
}

TEST_CASE("overlap bx::Cylinder/bx::Disk", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f);

	const bx::Disk crossing = { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 0.8f };
	REQUIRE(bx::overlap(cyl, crossing) );

	const bx::Disk far = { {5.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 0.3f };
	REQUIRE(!bx::overlap(cyl, far) );
}

TEST_CASE("overlap bx::Cylinder/bx::Obb", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 1.0f, 0.5f);
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(bx::overlap(cyl, obb) );

	const bx::Obb far = makeObb({5.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(!bx::overlap(cyl, far) );
}

TEST_CASE("overlap bx::Cone/bx::Vec3", "[bounds]")
{
	const bx::Cone cone = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);

	REQUIRE( bx::overlap(cone, bx::Vec3{0.0f, 0.0f, 0.0f}) );
	REQUIRE(!bx::overlap(cone, bx::Vec3{5.0f, 0.0f, 0.0f}) );
}

TEST_CASE("overlap bx::Cone/bx::Cylinder", "[bounds]")
{
	const bx::Cone cone = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);

	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 1.0f, 0.5f);
	REQUIRE(bx::overlap(cone, cyl) );

	const bx::Cylinder far = makeCylinder({5.0f, 0.0f, 0.0f}, 1.0f, 0.5f);
	REQUIRE(!bx::overlap(cone, far) );
}

TEST_CASE("overlap bx::Cone/bx::Capsule", "[bounds]")
{
	const bx::Cone cone = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);

	const bx::Capsule cap = makeCapsule({0.0f, 0.0f, 0.0f}, 1.0f, 0.3f);
	REQUIRE(bx::overlap(cone, cap) );

	const bx::Capsule far = makeCapsule({5.0f, 0.0f, 0.0f}, 1.0f, 0.3f);
	REQUIRE(!bx::overlap(cone, far) );
}

TEST_CASE("overlap bx::Cone/bx::Cone", "[bounds]")
{
	const bx::Cone a = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);
	const bx::Cone b = makeCone({0.0f, 0.5f, 0.0f}, 2.0f, 1.0f);
	REQUIRE(bx::overlap(a, b) );

	const bx::Cone c = makeCone({5.0f, 0.0f, 0.0f}, 2.0f, 1.0f);
	REQUIRE(!bx::overlap(a, c) );

	const bx::Cone angleA = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);
	const bx::Cone angleB = makeCone({0.5f, 0.5f, 0.0f}, 2.0f, 1.0f, {1.0f, 0.0f, 0.0f});
	REQUIRE(bx::overlap(angleA, angleB) );
}

TEST_CASE("overlap bx::Cone/bx::Disk", "[bounds]")
{
	const bx::Cone cone = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);

	const bx::Disk disk = { {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 0.5f };
	REQUIRE(bx::overlap(cone, disk) );

	const bx::Disk far = { {5.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 0.5f };
	REQUIRE(!bx::overlap(cone, far) );
}

TEST_CASE("overlap bx::Cone/bx::Obb", "[bounds]")
{
	const bx::Cone cone = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(bx::overlap(cone, obb) );

	const bx::Obb far = makeObb({5.0f, 5.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(!bx::overlap(cone, far) );
}

TEST_CASE("overlap bx::Disk/bx::Vec3", "[bounds]")
{
	const bx::Disk disk = { {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 1.0f };

	REQUIRE( bx::overlap(disk, bx::Vec3{0.0f, 0.0f, 0.0f}) );
	REQUIRE( bx::overlap(disk, bx::Vec3{0.5f, 0.0f, 0.0f}) );
	REQUIRE(!bx::overlap(disk, bx::Vec3{0.0f, 1.0f, 0.0f}) );
	REQUIRE(!bx::overlap(disk, bx::Vec3{2.0f, 0.0f, 0.0f}) );
}

TEST_CASE("overlap bx::Disk/bx::Plane", "[bounds]")
{
	const bx::Disk disk = { {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 1.0f };

	const bx::Plane pX = { {1.0f, 0.0f, 0.0f}, 0.0f };
	REQUIRE(bx::overlap(disk, pX) );
}

TEST_CASE("overlap bx::Disk/bx::Disk", "[bounds]")
{
	const bx::Disk a = { {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 1.0f };
	const bx::Disk b = { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 1.0f };
	REQUIRE(bx::overlap(a, b) );
}

TEST_CASE("overlap bx::Disk/bx::Obb", "[bounds]")
{
	const bx::Disk disk = { {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 1.0f };
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(bx::overlap(disk, obb) );

	const bx::Obb far = makeObb({5.0f, 5.0f, 5.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(!bx::overlap(disk, far) );
}

TEST_CASE("overlap bx::Triangle/bx::Vec3", "[bounds]")
{
	const bx::Triangle tri = { {0.0f, 0.0f, 0.0f}, {2.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 0.0f} };

	REQUIRE( bx::overlap(tri, bx::Vec3{0.5f, 0.5f, 0.0f}) );
	REQUIRE(!bx::overlap(tri, bx::Vec3{5.0f, 5.0f, 0.0f}) );
}

TEST_CASE("overlap bx::Triangle/bx::Sphere", "[bounds]")
{
	const bx::Triangle tri = { {0.0f, 0.0f, 0.0f}, {2.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 0.0f} };

	REQUIRE( bx::overlap(tri, bx::Sphere{ {0.5f, 0.5f, 0.0f}, 0.1f }) );
	REQUIRE(!bx::overlap(tri, bx::Sphere{ {5.0f, 5.0f, 0.0f}, 0.1f }) );
}

TEST_CASE("overlap bx::Triangle/bx::Aabb", "[bounds]")
{
	const bx::Triangle tri = { {0.0f, 0.0f, 0.0f}, {5.0f, 0.0f, 0.0f}, {0.0f, 5.0f, 0.0f} };
	bx::Aabb aabb;
	bx::toAabb(aabb, {20.0f, 20.0f, 20.0f}, {0.1f, 0.1f, 0.1f});
	REQUIRE(!bx::overlap(aabb, tri) );
}

TEST_CASE("intersect bx::Ray/bx::Sphere", "[bounds]")
{
	const bx::Sphere sphere = { {0.0f, 0.0f, 0.0f}, 1.0f };

	const bx::Ray hit = { {-5.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	bx::Hit hitResult;
	REQUIRE(bx::intersect(hit, sphere, &hitResult) );
	REQUIRE(bx::isEqual(hitResult.pos, {-1.0f, 0.0f, 0.0f}, 0.001f) );

	const bx::Ray miss = { {-5.0f, 5.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(!bx::intersect(miss, sphere) );
}

TEST_CASE("intersect bx::Ray/bx::Aabb", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	const bx::Ray hit = { {-5.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(bx::intersect(hit, aabb) );

	const bx::Ray miss = { {-5.0f, 5.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(!bx::intersect(miss, aabb) );
}

TEST_CASE("intersect bx::Ray/bx::Plane", "[bounds]")
{
	const bx::Plane plane = makePlaneY(0.0f);

	const bx::Ray down = { {0.0f, 5.0f, 0.0f}, {0.0f, -1.0f, 0.0f} };
	bx::Hit hitResult;
	REQUIRE(bx::intersect(down, plane, &hitResult) );
	REQUIRE(bx::isEqual(hitResult.pos, {0.0f, 0.0f, 0.0f}, 0.001f) );

	const bx::Ray away = { {0.0f, -5.0f, 0.0f}, {0.0f, -1.0f, 0.0f} };
	REQUIRE(!bx::intersect(away, plane) );
}

TEST_CASE("intersect bx::Ray/bx::Cylinder", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);

	const bx::Ray hit = { {-5.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(bx::intersect(hit, cyl) );

	const bx::Ray miss = { {-5.0f, 10.0f, 10.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(!bx::intersect(miss, cyl) );
}

TEST_CASE("intersect bx::Ray/bx::Capsule", "[bounds]")
{
	const bx::Capsule cap = makeCapsule({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);

	const bx::Ray hit = { {-5.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(bx::intersect(hit, cap) );

	const bx::Ray miss = { {-5.0f, 5.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(!bx::intersect(miss, cap) );
}

TEST_CASE("intersect bx::Ray/bx::Cone", "[bounds]")
{
	const bx::Cone cone = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);

	const bx::Ray hit = { {-5.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(bx::intersect(hit, cone) );

	const bx::Ray miss = { {-5.0f, 5.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(!bx::intersect(miss, cone) );
}

TEST_CASE("intersect bx::Ray/bx::Triangle", "[bounds]")
{
	const bx::Triangle tri = { {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f} };

	const bx::Ray hit = { {0.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 1.0f} };
	bx::Hit hitResult;
	REQUIRE(bx::intersect(hit, tri, &hitResult) );
	REQUIRE(bx::isEqual(hitResult.pos, {0.0f, 0.0f, 0.0f}, 0.001f) );

	const bx::Ray miss = { {5.0f, 5.0f, -5.0f}, {0.0f, 0.0f, 1.0f} };
	REQUIRE(!bx::intersect(miss, tri) );
}

TEST_CASE("intersect bx::Ray/bx::Obb", "[bounds]")
{
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	const bx::Ray hit = { {-5.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(bx::intersect(hit, obb) );

	const bx::Ray miss = { {-5.0f, 5.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(!bx::intersect(miss, obb) );
}

TEST_CASE("intersect bx::Ray/bx::Disk", "[bounds]")
{
	const bx::Disk disk = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, 1.0f };

	const bx::Ray hit = { {0.0f, 0.0f, -5.0f}, {0.0f, 0.0f, 1.0f} };
	bx::Hit hitResult;
	REQUIRE(bx::intersect(hit, disk, &hitResult) );
	REQUIRE(bx::isEqual(hitResult.pos, {0.0f, 0.0f, 0.0f}, 0.001f) );

	const bx::Ray miss = { {5.0f, 5.0f, -5.0f}, {0.0f, 0.0f, 1.0f} };
	REQUIRE(!bx::intersect(miss, disk) );
}

TEST_CASE("getCenter bx::Aabb", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {1.0f, 2.0f, 3.0f}, {0.5f, 0.5f, 0.5f});
	REQUIRE(bx::isEqual(bx::getCenter(aabb), {1.0f, 2.0f, 3.0f}, 0.0001f) );
}

TEST_CASE("getExtents bx::Aabb", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {2.0f, 3.0f, 4.0f});
	REQUIRE(bx::isEqual(bx::getExtents(aabb), {2.0f, 3.0f, 4.0f}, 0.0001f) );
}

TEST_CASE("calcAreaAabb", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	REQUIRE(bx::isEqual(bx::calcAreaAabb(aabb), 24.0f, 0.0001f) );
}

TEST_CASE("aabbExpand by factor", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	aabbExpand(aabb, 1.0f);
	REQUIRE(bx::isEqual(aabb.min, {-2.0f, -2.0f, -2.0f}, 0.0001f) );
	REQUIRE(bx::isEqual(aabb.max, { 2.0f,  2.0f,  2.0f}, 0.0001f) );
}

TEST_CASE("aabbExpand by point", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	aabbExpand(aabb, bx::Vec3{5.0f, 0.0f, 0.0f});
	REQUIRE(aabb.max.x >= 5.0f - 0.0001f);
}

TEST_CASE("getPointAt bx::Ray", "[bounds]")
{
	const bx::Ray ray = { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	REQUIRE(bx::isEqual(bx::getPointAt(ray, 3.0f), {3.0f, 0.0f, 0.0f}, 0.0001f) );
}

TEST_CASE("getPointAt bx::LineSegment", "[bounds]")
{
	const bx::LineSegment seg = { {0.0f, 0.0f, 0.0f}, {10.0f, 0.0f, 0.0f} };
	REQUIRE(bx::isEqual(bx::getPointAt(seg, 0.5f), {5.0f, 0.0f, 0.0f}, 0.0001f) );
}

TEST_CASE("overlap symmetry bx::Cylinder/bx::Capsule", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 2.0f, 0.5f);
	const bx::Capsule  cap = makeCapsule({0.8f, 0.0f, 0.0f}, 2.0f, 0.4f);
	REQUIRE(bx::overlap(cyl, cap) == bx::overlap(cap, cyl) );
}

TEST_CASE("overlap symmetry bx::Cone/bx::Cylinder", "[bounds]")
{
	const bx::Cone cone = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);
	const bx::Cylinder cyl = makeCylinder({0.5f, 0.0f, 0.0f}, 1.0f, 0.5f);
	REQUIRE(bx::overlap(cone, cyl) == bx::overlap(cyl, cone) );
}

TEST_CASE("overlap symmetry bx::Cylinder/bx::Obb", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 1.0f, 0.5f);
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(bx::overlap(cyl, obb) == bx::overlap(obb, cyl) );
}

TEST_CASE("overlap symmetry bx::Cone/bx::Capsule", "[bounds]")
{
	const bx::Cone cone = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);
	const bx::Capsule cap = makeCapsule({0.5f, 0.0f, 0.0f}, 1.0f, 0.3f);
	REQUIRE(bx::overlap(cone, cap) == bx::overlap(cap, cone) );
}

TEST_CASE("overlap symmetry bx::Cone/bx::Obb", "[bounds]")
{
	const bx::Cone cone = makeCone({0.0f, 0.0f, 0.0f}, 2.0f, 1.0f);
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});
	REQUIRE(bx::overlap(cone, obb) == bx::overlap(obb, cone) );
}

TEST_CASE("overlap symmetry bx::Cylinder/bx::Plane", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.5f, 0.0f}, 1.0f, 0.5f);
	const bx::Plane plane = makePlaneY(0.0f);
	REQUIRE(bx::overlap(cyl, plane) == bx::overlap(plane, cyl) );
}

TEST_CASE("closestPoint bx::Triangle interior", "[bounds]")
{
	const bx::Triangle tri = { {100.0f, 1.0f, 0.0f}, {100.0f, 1.0f, 1.0f}, {-100.0f, 1.0f, 0.0f} };
	const bx::Vec3 cp = bx::closestPoint(tri, {0.0f, 0.0f, 0.1f});
	REQUIRE(bx::isEqual(cp, {0.0f, 1.0f, 0.1f}, 2.0e-5f) );
}

TEST_CASE("closestPoint bx::Triangle edge AB", "[bounds]")
{
	const bx::Triangle tri = { {0.0f, 0.0f, 0.0f}, {2.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 0.0f} };
	const bx::Vec3 cp = bx::closestPoint(tri, {1.0f, -1.0f, 0.0f});
	REQUIRE(bx::isEqual(cp.y, 0.0f, 0.01f) );
	REQUIRE(cp.x >= -0.01f);
	REQUIRE(cp.x <=  2.01f);
}

TEST_CASE("closestPoint bx::Triangle edge AC", "[bounds]")
{
	const bx::Triangle tri = { {0.0f, 0.0f, 0.0f}, {2.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 0.0f} };
	const bx::Vec3 cp = bx::closestPoint(tri, {-1.0f, 1.0f, 0.0f});
	REQUIRE(bx::isEqual(cp.x, 0.0f, 0.01f) );
	REQUIRE(cp.y >= -0.01f);
	REQUIRE(cp.y <=  2.01f);
}

TEST_CASE("closestPoint bx::Triangle vertex A", "[bounds]")
{
	const bx::Triangle tri = { {100.0f, 1.0f, 0.0f}, {100.0f, 1.0f, 1.0f}, {-100.0f, 1.0f, 0.0f} };
	const bx::Vec3 cp = bx::closestPoint(tri, {101.0f, 0.0f, -1.0f});
	REQUIRE(bx::isEqual(cp, {100.0f, 1.0f, 0.0f}, 2.0e-5f) );
}

TEST_CASE("closestPoint bx::Triangle vertex B", "[bounds]")
{
	const bx::Triangle tri = { {100.0f, 1.0f, 0.0f}, {100.0f, 1.0f, 1.0f}, {-100.0f, 1.0f, 0.0f} };
	const bx::Vec3 cp = bx::closestPoint(tri, {101.0f, 0.0f, 2.0f});
	REQUIRE(bx::isEqual(cp, {100.0f, 1.0f, 1.0f}, 2.0e-5f) );
}

TEST_CASE("closestPoint bx::Triangle vertex C", "[bounds]")
{
	const bx::Triangle tri = { {100.0f, 1.0f, 0.0f}, {100.0f, 1.0f, 1.0f}, {-100.0f, 1.0f, 0.0f} };
	const bx::Vec3 cp = bx::closestPoint(tri, {-101.0f, 0.0f, 0.0f});
	REQUIRE(bx::isEqual(cp, {-100.0f, 1.0f, 0.0f}, 2.0e-5f) );
}

TEST_CASE("closestPoint bx::Triangle small near origin", "[bounds]")
{
	const bx::Triangle tri = {
		{-0.132395342f, -0.294095188f, -0.164812326f},
		{-0.126054004f, -0.283950001f, -0.159065604f},
		{-0.154956535f, -0.284792334f, -0.160523415f},
	};
	const bx::Vec3 cp = bx::closestPoint(tri, {0.0f, 0.0f, 0.0f});
	REQUIRE(bx::length(cp) < 1.0f);
}

TEST_CASE("closestPoint bx::Obb", "[bounds]")
{
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	const bx::Vec3 inside = bx::closestPoint(obb, {0.0f, 0.0f, 0.0f});
	REQUIRE(bx::isEqual(inside, {0.0f, 0.0f, 0.0f}, 0.001f) );

	const bx::Vec3 outside = bx::closestPoint(obb, {3.0f, 0.0f, 0.0f});
	REQUIRE(bx::isEqual(outside, {1.0f, 0.0f, 0.0f}, 0.001f) );

	const bx::Vec3 corner = bx::closestPoint(obb, {3.0f, 3.0f, 3.0f});
	REQUIRE(bx::isEqual(corner, {1.0f, 1.0f, 1.0f}, 0.001f) );
}

TEST_CASE("distance bx::Plane/bx::Vec3 signed", "[bounds]")
{
	const bx::Plane plane = makePlaneY(2.0f);

	REQUIRE(bx::isEqual(bx::distance(plane, {5.0f,  7.0f, 0.0f}),  5.0f, 0.0001f) );
	REQUIRE(bx::isEqual(bx::distance(plane, {5.0f, -3.0f, 0.0f}), -5.0f, 0.0001f) );
	REQUIRE(bx::isEqual(bx::distance(plane, {0.0f,  2.0f, 0.0f}),  0.0f, 0.0001f) );
}

TEST_CASE("intersectPlanes 3 orthogonal planes", "[bounds]")
{
	const bx::Plane pY = makePlaneY(2.0f);
	const bx::Plane pX = { {1.0f, 0.0f, 0.0f}, -3.0f };
	const bx::Plane pZ = { {0.0f, 0.0f, 1.0f}, -4.0f };

	const bx::Vec3 pt = intersectPlanes(pX, pY, pZ);
	REQUIRE(bx::isEqual(pt, {3.0f, 2.0f, 4.0f}, 0.001f) );
}

TEST_CASE("calcPlane from three vertices", "[bounds]")
{
	bx::Plane plane(bx::InitNone);
	bx::calcPlane(plane, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

	REQUIRE(bx::isEqual(abs(plane.normal.z), 1.0f, 0.001f) );
	REQUIRE(bx::isEqual(abs(plane.dist), 0.0f, 0.001f) );
}

TEST_CASE("calcPlane from normal and point", "[bounds]")
{
	bx::Plane plane(bx::InitNone);
	bx::calcPlane(plane, {0.0f, 1.0f, 0.0f}, {0.0f, 5.0f, 0.0f});

	REQUIRE(bx::isEqual(plane.normal, {0.0f, 1.0f, 0.0f}, 0.001f) );
	REQUIRE(bx::isEqual(bx::distance(plane, {0.0f, 5.0f, 0.0f}), 0.0f, 0.001f) );
}

TEST_CASE("calcPlane from triangle", "[bounds]")
{
	const bx::Triangle tri = { {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f} };
	bx::Plane plane(bx::InitNone);
	bx::calcPlane(plane, tri.v0, tri.v1, tri.v2);

	REQUIRE(bx::isEqual(abs(plane.normal.y), 1.0f, 0.001f) );
}

TEST_CASE("getCenter bx::Triangle", "[bounds]")
{
	const bx::Triangle tri = { {0.0f, 0.0f, 0.0f}, {3.0f, 0.0f, 0.0f}, {0.0f, 3.0f, 0.0f} };
	const bx::Vec3 c = bx::getCenter(tri);
	REQUIRE(bx::isEqual(c, {1.0f, 1.0f, 0.0f}, 0.001f) );
}

TEST_CASE("overlap bx::Aabb/bx::Vec3 boundary", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {0.1f, 0.2f, 0.3f});

	const bx::Vec3 probes[] = {
		{-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
		{0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f},
	};
	const bx::Vec3 halfSize = {0.1f, 0.2f, 0.3f};

	for (int32_t ii = 0; ii < 6; ++ii)
	{
		const bx::Vec3 inside = bx::mul(bx::mul(halfSize, probes[ii]), 0.99f);
		REQUIRE(bx::overlap(aabb, inside) );

		const bx::Vec3 outside = bx::mul(bx::mul(halfSize, probes[ii]), 1.01f);
		REQUIRE(!bx::overlap(aabb, outside) );
	}
}

TEST_CASE("overlap bx::Sphere/bx::Vec3 boundary", "[bounds]")
{
	const bx::Sphere sphere = { {0.0f, 0.0f, 0.0f}, 0.5f };

	REQUIRE( bx::overlap(sphere, bx::Vec3{0.495f, 0.0f, 0.0f}) );
	REQUIRE(!bx::overlap(sphere, bx::Vec3{0.505f, 0.0f, 0.0f}) );
}

TEST_CASE("overlap bx::Capsule/bx::Vec3 boundary", "[bounds]")
{
	const bx::Capsule cap = makeCapsule({0.0f, 0.0f, 0.0f}, 0.2f, 0.1f);

	REQUIRE( bx::overlap(cap, bx::Vec3{  0.0f,  0.0f, 0.0f}) );
	REQUIRE( bx::overlap(cap, bx::Vec3{0.099f,  0.2f, 0.0f}) );
	REQUIRE( bx::overlap(cap, bx::Vec3{0.099f, -0.2f, 0.0f}) );
	REQUIRE(!bx::overlap(cap, bx::Vec3{  0.0f,  0.5f, 0.0f}) );
	REQUIRE(!bx::overlap(cap, bx::Vec3{  0.5f,  0.0f, 0.0f}) );
}

TEST_CASE("overlap bx::Cylinder/bx::Vec3 boundary", "[bounds]")
{
	const bx::Cylinder cyl = makeCylinder({0.0f, 0.0f, 0.0f}, 0.2f, 0.1f);

	REQUIRE(bx::overlap(cyl, bx::Vec3{0.0f, 0.0f, 0.0f}) );
	REQUIRE( bx::overlap(cyl, bx::Vec3{0.0f, 0.19f, 0.0f}) );
	REQUIRE(!bx::overlap(cyl, bx::Vec3{0.0f, 0.5f, 0.0f}) );
	REQUIRE(!bx::overlap(cyl, bx::Vec3{0.5f, 0.0f, 0.0f}) );
}

TEST_CASE("bx::Aabb containment", "[bounds]")
{
	bx::Aabb outer, inner;
	bx::toAabb(outer, {0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 10.0f});
	bx::toAabb(inner, {0.0f, 0.0f, 0.0f}, {5.0f, 5.0f, 5.0f});

	REQUIRE(bx::overlap(outer, inner.min) );
	REQUIRE(bx::overlap(outer, inner.max) );
	REQUIRE(!bx::overlap(inner, outer.max) );
}

TEST_CASE("bx::Aabb union via aabbExpand", "[bounds]")
{
	bx::Aabb a;
	bx::toAabb(a, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	aabbExpand(a, bx::Vec3{5.0f, 5.0f, 5.0f});
	REQUIRE(bx::overlap(a, bx::Vec3{5.0f, 5.0f, 5.0f}) );
	REQUIRE(bx::overlap(a, bx::Vec3{0.0f, 0.0f, 0.0f}) );

	aabbExpand(a, bx::Vec3{-3.0f, -3.0f, -3.0f});
	REQUIRE(bx::overlap(a, bx::Vec3{-3.0f, -3.0f, -3.0f}) );
}

TEST_CASE("bx::Aabb translation", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	const bx::Vec3 offset = {10.0f, 0.0f, 0.0f};
	bx::Aabb translated;
	translated.min = bx::add(aabb.min, offset);
	translated.max = bx::add(aabb.max, offset);

	REQUIRE(bx::isEqual(bx::getCenter(translated), {10.0f, 0.0f, 0.0f}, 0.001f) );
	REQUIRE(bx::isEqual(bx::getExtents(translated), {1.0f, 1.0f, 1.0f}, 0.001f) );
}

TEST_CASE("intersect bx::Ray/bx::Aabb per-axis fractions", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 1.0f});

	{
		const bx::Ray ray = { {1.1f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f} };
		bx::Hit hit;
		REQUIRE(bx::intersect(ray, aabb, &hit) );
		REQUIRE(bx::isEqual(hit.pos.x, 1.0f, 0.01f) );
	}

	{
		const bx::Ray ray = { {-1.1f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
		bx::Hit hit;
		REQUIRE(bx::intersect(ray, aabb, &hit) );
		REQUIRE(bx::isEqual(hit.pos.x, -1.0f, 0.01f) );
	}

	{
		const bx::Ray ray = { {0.0f, 1.1f, 0.0f}, {0.0f, -1.0f, 0.0f} };
		bx::Hit hit;
		REQUIRE(bx::intersect(ray, aabb, &hit) );
		REQUIRE(bx::isEqual(hit.pos.y, 1.0f, 0.01f) );
	}

	{
		const bx::Vec3 direction = {4.0f, -4.0f, 0.0f};
		const float expectedFraction = 0.123f;
		const bx::Vec3 expectedHit = {0.0f, 1.0f, 0.0f};
		const bx::Vec3 origin = bx::sub(expectedHit, bx::mul(direction, expectedFraction) );
		const bx::Ray ray = { origin, direction };
		bx::Hit hit;
		REQUIRE(bx::intersect(ray, aabb, &hit) );
		REQUIRE(bx::isEqual(hit.pos, expectedHit, 0.01f) );
	}
}

TEST_CASE("intersect bx::Ray/bx::Sphere fraction", "[bounds]")
{
	const bx::Sphere sphere = { {0.0f, 0.0f, 0.0f}, 1.0f };

	const bx::Ray ray = { {-5.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f} };
	bx::Hit hit;
	REQUIRE(bx::intersect(ray, sphere, &hit) );
	REQUIRE(bx::isEqual(hit.pos, {-1.0f, 0.0f, 0.0f}, 0.01f) );
	REQUIRE(bx::isEqual(hit.plane.dist, 4.0f, 0.01f) );
}

TEST_CASE("intersect bx::Ray/bx::Triangle edge hit", "[bounds]")
{
	const bx::Triangle tri = { {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f} };

	const bx::Ray ray = { {1.0f, -1.0f, -2.0f}, {0.0f, 0.0f, 1.0f} };
	REQUIRE(bx::intersect(ray, tri) );
}

TEST_CASE("intersect bx::Ray/bx::Triangle miss outside", "[bounds]")
{
	const bx::Triangle tri = { {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, -1.0f, 0.0f} };

	const bx::Ray ray = { {2.0f, 0.0f, -2.0f}, {0.0f, 0.0f, 1.0f} };
	REQUIRE(!bx::intersect(ray, tri) );
}

TEST_CASE("intersect bx::Ray/bx::Capsule multi-direction", "[bounds]")
{
	const bx::Capsule cap = makeCapsule({0.0f, 0.0f, 0.0f}, 0.5f, 0.5f);

	REQUIRE( bx::intersect(bx::Ray{ {0.0f, 0.0f, -2.0f}, { 0.0f, 0.0f, 1.0f} }, cap) );
	REQUIRE( bx::intersect(bx::Ray{ {2.0f, 0.0f,  0.0f}, {-1.0f, 0.0f, 0.0f} }, cap) );
	REQUIRE(!bx::intersect(bx::Ray{ {2.0f, 0.0f, -2.0f}, { 0.0f, 0.0f, 1.0f} }, cap) );
}

TEST_CASE("degenerate bx::Capsule acts as bx::Sphere", "[bounds]")
{
	const bx::Capsule cap = { {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0f };
	const bx::Sphere sphere = { {0.0f, 0.0f, 0.0f}, 1.0f };

	REQUIRE(bx::overlap(cap, bx::Vec3{0.5f, 0.0f, 0.0f}) == bx::overlap(sphere, bx::Vec3{0.5f, 0.0f, 0.0f}) );
	REQUIRE(bx::overlap(cap, bx::Vec3{1.5f, 0.0f, 0.0f}) == bx::overlap(sphere, bx::Vec3{1.5f, 0.0f, 0.0f}) );

}

TEST_CASE("calcMaxBoundingSphere", "[bounds]")
{
	const bx::Vec3 verts[] = {
		{-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f},
		{-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f},
	};

	bx::Sphere sphere;
	bx::calcMaxBoundingSphere(sphere, verts, 8, sizeof(bx::Vec3) );

	REQUIRE(bx::length(sphere.center) < 0.1f);

	REQUIRE(sphere.radius >= 1.73f - 0.01f);
	REQUIRE(sphere.radius <= 1.74f + 0.01f);
}

TEST_CASE("calcMinBoundingSphere", "[bounds]")
{
	const bx::Vec3 verts[] = {
		{-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f},
		{-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f},
	};

	bx::Sphere sphere;
	bx::calcMinBoundingSphere(sphere, verts, 8, sizeof(bx::Vec3) );

	for (int32_t ii = 0; ii < 8; ++ii)
	{
		REQUIRE(bx::overlap(sphere, verts[ii]) );
	}

	REQUIRE(sphere.radius >= 1.73f - 0.05f);
}

TEST_CASE("calcObb from cube vertices", "[bounds]")
{
	const bx::Vec3 verts[] = {
		{-1.0f, -1.0f, -1.0f}, { 1.0f, -1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f}, { 1.0f,  1.0f, -1.0f},
		{-1.0f, -1.0f,  1.0f}, { 1.0f, -1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f}, { 1.0f,  1.0f,  1.0f},
	};

	bx::Obb obb;
	calcObb(obb, verts, 8, sizeof(bx::Vec3) );

	for (int32_t ii = 0; ii < 8; ++ii)
	{
		REQUIRE(bx::overlap(obb, verts[ii]) );
	}
}

TEST_CASE("toObb from bx::Aabb round-trip", "[bounds]")
{
	bx::Aabb aabb;
	bx::toAabb(aabb, {1.0f, 2.0f, 3.0f}, {0.5f, 1.0f, 1.5f});

	bx::Obb obb;
	bx::toObb(obb, aabb);

	REQUIRE(bx::overlap(obb, bx::getCenter(aabb) ) );
	REQUIRE(bx::overlap(obb, aabb.min) );
	REQUIRE(bx::overlap(obb, aabb.max) );
}

TEST_CASE("toAabb from bx::Obb", "[bounds]")
{
	const bx::Obb obb = makeObb({0.0f, 0.0f, 0.0f}, {1.0f, 2.0f, 3.0f});
	bx::Aabb aabb;
	bx::toAabb(aabb, obb);

	REQUIRE(bx::isEqual(bx::getCenter(aabb), {0.0f, 0.0f, 0.0f}, 0.01f) );
	const bx::Vec3 ext = bx::getExtents(aabb);
	REQUIRE(ext.x >= 1.0f - 0.01f);
	REQUIRE(ext.y >= 2.0f - 0.01f);
	REQUIRE(ext.z >= 3.0f - 0.01f);
}

TEST_CASE("toAabb from vertex cloud", "[bounds]")
{
	const bx::Vec3 verts[] = {
		{-2.0f, 0.0f, 0.0f},
		{ 3.0f, 0.0f, 0.0f},
		{ 0.0f, -1.0f, 0.0f},
		{ 0.0f,  4.0f, 0.0f},
		{ 0.0f, 0.0f, -5.0f},
		{ 0.0f, 0.0f,  6.0f},
	};

	bx::Aabb aabb;
	bx::toAabb(aabb, verts, 6, sizeof(bx::Vec3) );

	REQUIRE(bx::isEqual(aabb.min, {-2.0f, -1.0f, -5.0f}, 0.0001f) );
	REQUIRE(bx::isEqual(aabb.max, { 3.0f,  4.0f,  6.0f}, 0.0001f) );
}
