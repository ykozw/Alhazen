#pragma once

#include "pch.hpp"
#include "core/ray.hpp"

// レイの浮かしを決める定数(pbrt ch4参照)
#define RAYEPSILON_SCALE_QUADRIC_SURFACE 5e-4f
#define RAYEPSILON_SCALE_PLANE_SURFACE 1e-3f

/*
-------------------------------------------------
Sphere vs Ray
-------------------------------------------------
*/
bool
intersectSphere(const Ray& ray, Vec3 o, float r2, Intersect* isect);
bool
intersectSphereCheck(const Ray& ray, Vec3 pos, float r2);

/*
-------------------------------------------------
Triangle vs Ray
-------------------------------------------------
*/
bool
intersectTriangle(const Ray& ray,
                  Vec3 v0,
                  Vec3 v1,
                  Vec3 v2,
                  Vec3 n0,
                  Vec3 n1,
                  Vec3 n2,
                  Vec2 t0,
                  Vec2 t1,
                  Vec2 t2,
                  Intersect* isect);
bool
intersectTriangleCheck(const Ray& ray, Vec3 v0, Vec3 v1, Vec3 v2);

/*
-------------------------------------------------
Rectangle vs Ray
TODO: 追加する
-------------------------------------------------
*/

/*
-------------------------------------------------
Plane vs Ray
TODO: 追加する
-------------------------------------------------
*/
bool
intersectPlane(const Ray& ray, Vec3 dir, float dist, Intersect* isect);
bool
intersectPlaneCheck(const Ray& ray, Vec3 dir, float dist);

#include "intersect.inl"
