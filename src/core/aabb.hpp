#pragma once

#include "pch.hpp"
#include "core/math.hpp"
#include "core/ray.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class AABB
{
public:
    AABB();
    void clear();
    void addPoint(const Vec3& point);
    void addPoints(_In_reads_(numPoints) const Vec3* points, int32_t numPoints);
    Vec3 center() const;
    const Vec3& min() const;
    const Vec3& max() const;
    Vec3 size() const;
    void addAABB(const AABB& aabb);
    bool intersect(const Ray& ray, Intersect* isect) const;
    bool intersectCheck(const Ray& ray, float currentIntersectT) const;
    bool validate() const;
    bool isInside(const Vec3& p) const;
    const Vec3& operator[](int32_t index) const;
    //
    AABB operator + (const AABB& other);
private:
    Vec3 mn;
    Vec3 mx;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class AABB2D
{
public:
    AABB2D();
    AABB2D(const Vec2& mn, const Vec2& mx);
    AABB2D(float mnx, float mny, float mxx, float mxy);
    void clear();
    void addPoint(const Vec2& point);
    void add(const AABB2D& aabb);
    float width() const;
    float height() const;
    Vec2 center() const;
    const Vec2& min() const;
    const Vec2& max() const;
    Vec2 onCoord(const Vec2& coord) const;
    static AABB2D lerp(const AABB2D& lhs, const AABB2D& rhs, float factor);
private:
    Vec2 mn;
    Vec2 mx;
};

#include "aabb.inl"
