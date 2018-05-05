#pragma once

#include "core/intersect.hpp"
#include "core/errfloat.hpp"

/*
-------------------------------------------------
"Numerically Stable Method for Solving Quadratic Equations"
https://people.csail.mit.edu/bkph/articles/Quadratics.pdf
-------------------------------------------------
*/
INLINE bool solveQuadratic(ErrFloat a, ErrFloat b, ErrFloat c, ErrFloat* t0, ErrFloat* t1)
{
#if 1
    /*
    解の判定のみdoubleで計算する
    ErrFloatのみで判定すると抜けが非常に多くなってしまう
    pbrtのsqrt(D)の誤差を適当に決める方式も抜けがひどくなる
    */
    const ErrFloat D = b * b - 4.0f * a * c;
    const double Dhigh = double(float(b)) * double(float(b)) - 4.0 * (double)float(a) * double(float(c));
    // 解なし
    if (Dhigh < 0.0)
    {
        return false;
    }
    const ErrFloat Dsqrt = D.sqrt();
#else
    // pbrt方式
    const double D = double(float(b)) * double(float(b)) - 4.0 * (double)float(a) * double(float(c));
    if (D < 0.0)
    {
        return false;
    }
    const double tmp = std::sqrt(D);
#define MachineEpsilon (std::numeric_limits<float>::epsilon() * 100.0)
    const ErrFloat Dsqrt(float(tmp), float(MachineEpsilon * tmp));
#endif
    ErrFloat q;
    if (float(b) < 0.0f)
    {
        q = -0.5f * (b - Dsqrt);
    }
    else
    {
        q = -0.5f * (b + Dsqrt);
    }
    *t0 = q / a;
    *t1 = c / q;
    if (float(*t1) < float(*t0))
    {
        std::swap(*t0, *t1);
    }
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectSphereShapeCore(
    const Ray& ray,
    Vec3 o,
    ErrFloat r2,
    float* t)
{
    const Vec3 dif = ray.o - o;
    const ErrFloat ox(dif.x()); // rs
    const ErrFloat oy(dif.y());
    const ErrFloat oz(dif.z());
    const ErrFloat dx(ray.d.x());
    const ErrFloat dy(ray.d.y());
    const ErrFloat dz(ray.d.z());
    const ErrFloat a = dx * dx + dy * dy + dz * dz;
    const ErrFloat b = 2 * (dx * ox + dy * oy + dz * oz);
    const ErrFloat c = ox * ox + oy * oy + oz * oz - r2;
    //
    ErrFloat t0, t1;
    if (!solveQuadratic(a, b, c, &t0, &t1))
    {
        return false;
    }
    //
    if ((t0.high() > ray.maxt) || (t1.low() <= 0.0f))
    {
        return false;
    }
    //
    ErrFloat rayt;
    if (t0.low() <= 0.0f)
    {
        rayt = t1;
        if (t1.high() > ray.maxt)
        {
            return false;
        }
    }
    else
    {
        rayt = t0;
    }
    //
    *t = float(rayt.low());
    //
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool intersectSphere(
    const Ray& ray,
    Vec3 o,
    ErrFloat r2,
    Intersect* isect)
{
    float t;
    if (!intersectSphereShapeCore(ray, o, r2, &t))
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
    ErrFloat r2)
{
    float t;
    return intersectSphereShapeCore(ray, pos, r2, &t);
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
