#pragma once

#include "intersect.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectSphereShapeCore(
    const Ray& ray,
    Vec3 o,
    float r2,
    float* t,
    bool* intersectBackside)
{
    //
    const Vec3 rs = ray.o - o;
    // 球中心からレイ原点までの距離二乗
    const float rs2 = Vec3::dot(rs, rs);
    // 半径二乗
    const float lhs = Vec3::dot(ray.d, rs);
    const float D = lhs * lhs - rs2 + r2;
    if (D <= 0.0f)
    {
        return false;
    }
    //
    const float sqrtD = std::sqrtf(D);
    const float t0 = -lhs - sqrtD;
    const float t1 = -lhs + sqrtD;
    if (t0 < 0.0f && t1 < 0.0f)
    {
        return false;
    }
    else if (t0 >= 0.0f)
    {
        *intersectBackside = false;
        *t = t0;
    }
    else
    {
        *intersectBackside = true;
        *t = t1;
    }
    // 範囲チェック
    if (*t < ray.mint || ray.maxt < *t)
    {
        return false;
    }
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectSphere(
    const Ray& ray,
    Vec3 o,
    float r2,
    Intersect* isect)
{
    float t;
    bool intersectBackside = false;
    if (!intersectSphereShapeCore(ray, o, r2, &t, &intersectBackside))
    {
        return false;
    }
    if (isect->t < t)
    {
        return false;
    }
    //
    isect->position = ray.o + ray.d * t;
    isect->normal = (isect->position - o);
    isect->normal.normalize();
    if (intersectBackside)
    {
        //isect.normal = -isect.normal;
    }
    isect->t = t;
    isect->rayEpsilon = t * RAYEPSILON_SCALE_QUADRIC_SURFACE;
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectSphereCheck(
    const Ray& ray,
    Vec3 pos,
    float r2)
{
    float t;
    bool intersectBackside = false;
    return intersectSphereShapeCore(ray, pos, r2, &t, &intersectBackside);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectTriangleCore(
    const Ray& ray,
    Vec3 v0,
    Vec3 v1,
    Vec3 v2,
    float* t,
    float* u,
    float* v,
    bool* isFlip)
{
    //
    Vec3 e1, e2;  //Edge1, Edge2
    Vec3 P, Q, T;
    float det, inv_det;
    // v0を共有するエッジ
    e1 = v1 - v0;
    e2 = v2 - v0;
    // detとuの準備
    P = Vec3::cross(ray.d, e2);
    // ほぼ平行な場合かをチェック
    det = Vec3::dot(e1, P);
    if (det == 0.0f)
    {
        return false;
    }
    inv_det = 1.0f / det;
    // レイ原点からv0への距離
    T = ray.o - v0;
    // uを計算し、範囲内に収まっているかをチェック
    *u = Vec3::dot(T, P) * inv_det;
    if (*u < 0.0f || *u > 1.0f)
    {
        return false;
    }
    // vも同様の計算
    Q = Vec3::cross(T, e1);
    *v = Vec3::dot(ray.d, Q) * inv_det;
    if (*v < 0.0f || *u + *v  > 1.0f)
    {
        return false;
    }
    // tの範囲チェック
    *t = Vec3::dot(e2, Q) * inv_det;
    if (*t < ray.mint || ray.maxt < *t)
    {
        return false;
    }
    // 面の方向を返す
    *isFlip = (det < 0.0f);
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectTriangle(
    const Ray& ray,
   Vec3 v0,
   Vec3 v1,
   Vec3 v2,
   Vec3 n0,
   Vec3 n1,
   Vec3 n2,
   Vec2 t0,
   Vec2 t1,
   Vec2 t2,
   Intersect* isect)
{
    //
    float t, u, v;
    bool isFlip;
    if (!intersectTriangleCore(ray, v0, v1, v2, &t, &u, &v, &isFlip))
    {
        return false;
    }
    if (t >= isect->t)
    {
        return false;
    }
    // レイの交差の更新
    isect->t = t;
    isect->rayEpsilon = t * RAYEPSILON_SCALE_PLANE_SURFACE;
    isect->normal =
        n0 * (1.0f - u - v) +
        n1 * u +
        n2 * v;
    if (isFlip)
    {
        isect->normal = -isect->normal;
    }
    // FIXME: なぜか法線(Y,Z)が逆向きになっている
    /*std::swap(isect.normal.y, isect.normal.z);
    isect.normal.x = -isect.normal.x;*/
    //isect.normal = -isect.normal;
    isect->normal.normalize();
    isect->position =
        v0 * (1.0f - u - v) +
        v1 * u +
        v2 * v;
    isect->uv =
        t0 * (1.0f - u - v) +
        t1 * u +
        t2 * v;
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectTriangleCheck(
    const Ray& ray,
    Vec3 v0,
    Vec3 v1,
    Vec3 v2)
{
    float t, u, v;
    bool isFlip;
    return intersectTriangleCore(ray, v0, v1, v2, &t, &u, &v, &isFlip);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectPlaneShapeCore(
    const Ray& ray,
    Vec3 dir,
    const float dist,
    float* t)
{
    //
    const float nDotD = Vec3::dot(dir, ray.d);
    // 片面のみなので、レイの方向と平面の向きが同じだったら交差しない
    if (nDotD >= 0.0f)
    {
        return false;
    }
    // 距離の算出
    *t = (dist - Vec3::dot(dir, ray.o)) / nDotD;
    // 裏の場合はフリップ
    if (*t <= 0.0f)
    {
        return false;
    }
    // 範囲チェック
    if (*t < ray.mint || ray.maxt < *t)
    {
        return false;
    }
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectPlane(
    const Ray& ray,
    Vec3 dir,
    float dist,
    Intersect* isect)
{
    float t;
    if (!intersectPlaneShapeCore(ray, dir, dist, &t))
    {
        return false;
    }
    // TODO: two sided対応にした方がいいかも？
    if (isect->t < t)
    {
        return false;
    }
    //
    isect->t = t;
    isect->rayEpsilon = t * RAYEPSILON_SCALE_PLANE_SURFACE;
    isect->normal = dir;
    isect->position = ray.o + ray.d * t;
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectPlaneCheck(
    const Ray& ray,
    Vec3 dir,
    float dist)
{
    //
    float t;
    return intersectPlaneShapeCore(ray, dir, dist, &t);
}
