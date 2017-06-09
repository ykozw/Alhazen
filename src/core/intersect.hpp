#pragma once

#include "pch.hpp"
#include "core/ray.hpp"
#include "stats.hpp"

// レイの浮かしを決める定数(pbrt ch4参照)
#define RAYEPSILON_SCALE_QUADRIC_SURFACE 5e-4f
#define RAYEPSILON_SCALE_PLANE_SURFACE 1e-3f

//-------------------------------------------------
// Sphere vs Ray
//-------------------------------------------------
bool intersectSphere(
    const Ray& ray,
    const Vec3& o,
    float r,
    _Inout_ Intersect* isect);
bool intersectSphereCheck(
    const Ray& ray,
    const Vec3& pos,
    float r);

//-------------------------------------------------
// Triangle vs Ray
//-------------------------------------------------
bool intersectTriangle(
    const Ray& ray,
    const Vec3& v0,
    const Vec3& v1,
    const Vec3& v2,
    const Vec3& n0,
    const Vec3& n1,
    const Vec3& n2,
    const Vec2& t0,
    const Vec2& t1,
    const Vec2& t2,
    _Inout_ Intersect* isect);
bool intersectTriangleCheck(
    const Ray& ray,
    const Vec3& v0,
    const Vec3& v1,
    const Vec3& v2 );

//-------------------------------------------------
// Rectangle vs Ray
//-------------------------------------------------
// TODO: 追加する

//-------------------------------------------------
// Plane vs Ray
//-------------------------------------------------
bool intersectPlane(
    const Ray& ray,
    const Vec3& dir,
    float dist,
    _Inout_ Intersect* isect);
bool intersectPlaneCheck(
    const Ray& ray,
    const Vec3& dir,
    float dist);

#include "intersect.inl"
