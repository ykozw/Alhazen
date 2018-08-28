#pragma once

#include "core/math.hpp"
#include "core/ray.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class AABB AL_FINAL
{
public:
    AABB();
    void clear();
    void addPoint(Vec3 point);
    void addSphere(Vec3 center, float radius);
    void addPoints(const Vec3* points, int32_t numPoints);
    Vec3 center() const;
    Vec3 min() const;
    Vec3 max() const;
    Vec3 size() const;
    void addAABB(const AABB& aabb);
    bool intersect(const Ray& ray, Intersect* isect) const;
    bool intersectCheck(const Ray& ray, float currentIntersectT) const;
    bool validate() const;
    bool isInside(Vec3 p) const;
    const Vec3& operator[](int32_t index) const;
    //
    AABB operator+(const AABB& other);

private:
    Vec3 mn;
    Vec3 mx;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class AABB2D AL_FINAL
{
public:
    AABB2D();
    AABB2D(Vec2 mn, Vec2 mx);
    AABB2D(float mnx, float mny, float mxx, float mxy);
    void clear();
    void addPoint(Vec2 point);
    void add(const AABB2D& aabb);
    float width() const;
    float height() const;
    Vec2 center() const;
    const Vec2& min() const;
    const Vec2& max() const;
    Vec2 onCoord(Vec2 coord) const;
    static AABB2D lerp(const AABB2D& lhs, const AABB2D& rhs, float factor);

private:
    Vec2 mn;
    Vec2 mx;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class BoundingSphere AL_FINAL
{
public:
    BoundingSphere(const AABB& aabb);
    Vec3 center() const;
    float radius() const;

private:
    float radius_ = -1.0f;
    ;
    Vec3 center_ = Vec3(0.0f, 0.0f, 0.0f);
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class Convexhull2d AL_FINAL
{
public:
    Convexhull2d(const std::vector<Vec2>& points);
};

#include "bounding.inl"
