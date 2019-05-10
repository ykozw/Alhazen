#include "core/bounding.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE AABB::AABB()
{
    clear();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void AABB::clear()
{
    const float maxv = std::numeric_limits<float>::max();
    const float minv = std::numeric_limits<float>::lowest();
    mn = Vec3(maxv, maxv, maxv);
    mx = Vec3(minv, minv, minv);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void AABB::addPoint(Vec3 point)
{
    mn = Vec3::min(point, mn);
    mx = Vec3::max(point, mx);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void AABB::addSphere(Vec3 center, float radius)
{
    mn = center - Vec3(radius);
    mx = center + Vec3(radius);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void AABB::addPoints(const Vec3* points, int32_t numPoints)
{
    for (int32_t i = 0; i < numPoints; ++i)
    {
        addPoint(points[i]);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 AABB::center() const
{
    return (mn + mx) * 0.5f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 AABB::min() const
{
    return mn;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 AABB::max() const
{
    return mx;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 AABB::size() const
{
    return max() - min();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void AABB::addAABB(const AABB& aabb)
{
    AL_ASSERT_DEBUG(aabb.validate());
    mn = Vec3::min(aabb.mn, mn);
    mx = Vec3::max(aabb.mx, mx);
}

/*
-------------------------------------------------
AABBとレイの交差判定
TODO: 全体的に改良の余地がある
-------------------------------------------------
*/
INLINE bool AABB::intersect(const Ray& ray, Intersect* isect) const
{
    //
    const AABB& aabb = *this;
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    tmin = (aabb[ray.sign[0]].x() - ray.o.x()) * ray.dinv.x();
    tmax = (aabb[1 - ray.sign[0]].x() - ray.o.x()) * ray.dinv.x();
    tymin = (aabb[ray.sign[1]].y() - ray.o.y()) * ray.dinv.y();
    tymax = (aabb[1 - ray.sign[1]].y() - ray.o.y()) * ray.dinv.y();
    int32_t hitAxis = 0;
    if ((tmin > tymax) || (tymin > tmax))
    {
        return false;
    }
    if (tymin > tmin)
    {
        tmin = tymin;
        hitAxis = 1;
    }
    if (tymax < tmax)
    {
        tmax = tymax;
    }
    tzmin = (aabb[ray.sign[2]].z() - ray.o.z()) * ray.dinv.z();
    tzmax = (aabb[1 - ray.sign[2]].z() - ray.o.z()) * ray.dinv.z();
    if ((tmin > tzmax) || (tzmin > tmax))
    {
        return false;
    }
    if (tzmin > tmin)
    {
        tmin = tzmin;
        hitAxis = 2;
    }
    if (tzmax < tmax)
    {
        tmax = tzmax;
    }
    const bool isHit = (tmin < isect->t) && (ray.mint < tmax) && (tmin < ray.maxt);

    // 交差が確定したらisectに交差情報を格納する
    if (isHit)
    {
        // 交差はとりあえずは外側のみを対象
        isect->t = tmin;
        isect->position = ray.o + ray.d * tmin;
        static const Vec3 ns[3][2] =
        {
            {
                Vec3(-1.0f, 0.0f, 0.0f),
                Vec3(+1.0f, 0.0f, 0.0f),
            },
            {
                Vec3(0.0f, -1.0f, 0.0f),
                Vec3(0.0f, +1.0f, 0.0f),
            },
            {
                Vec3(0.0f, 0.0f, -1.0f),
                Vec3(0.0f, 0.0f, +1.0f),
            }
        };
        isect->normal = ns[hitAxis][ray.sign[hitAxis]];
    }
    return isHit;
}

/*
-------------------------------------------------
AABBとレイの交差判定
使われる局面が、BVH内のAABBに対してのみであり、
衝突するかしないかわかればよいのでその情報だけ返す
http ://people.csail.mit.edu/amy/papers/box-jgt.pdf
-------------------------------------------------
*/
INLINE bool AABB::intersectCheck(const Ray& ray, float currentIntersectT) const
{
    //
    const AABB& aabb = *this;
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    tmin = (aabb[ray.sign[0]].x() - ray.o.x()) * ray.dinv.x();
    tmax = (aabb[1 - ray.sign[0]].x() - ray.o.x()) * ray.dinv.x();
    tymin = (aabb[ray.sign[1]].y() - ray.o.y()) * ray.dinv.y();
    tymax = (aabb[1 - ray.sign[1]].y() - ray.o.y()) * ray.dinv.y();
    if ((tmin > tymax) || (tymin > tmax))
    {
        return false;
    }
    if (tymin > tmin)
    {
        tmin = tymin;
    }
    if (tymax < tmax)
    {
        tmax = tymax;
    }
    tzmin = (aabb[ray.sign[2]].z() - ray.o.z()) * ray.dinv.z();
    tzmax = (aabb[1 - ray.sign[2]].z() - ray.o.z()) * ray.dinv.z();
    if ((tmin > tzmax) || (tzmin > tmax))
    {
        return false;
    }
    if (tzmin > tmin)
    {
        tmin = tzmin;
    }
    if (tzmax < tmax)
    {
        tmax = tzmax;
    }
    return (tmin < currentIntersectT) && (ray.mint < tmax) && (tmin < ray.maxt);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool AABB::validate() const
{
    // TODO: 最適化
    return
        (mx.x() >= mn.x()) &&
        (mx.y() >= mn.y()) &&
        (mx.z() >= mn.z());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool AABB::isInside(Vec3 p) const
{
    // TODO: 最適化
    return
        (mn.x() <= p.x()) && (p.x() <= mx.x()) &&
        (mn.y() <= p.y()) && (p.y() <= mx.y()) &&
        (mn.z() <= p.z()) && (p.z() <= mx.z());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE const Vec3& AABB::operator[](int32_t index) const
{
    AL_ASSERT_DEBUG(index == 0 || index == 1);
    return *(&mn + index);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE AABB AABB::operator + (const AABB& other)
{
    AABB aabb;
    aabb.addAABB(*this);
    aabb.addAABB(other);
    return aabb;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE AABB2D::AABB2D()
{
    clear();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE AABB2D::AABB2D(
    Vec2 amn,
    Vec2 amx)
{
    mn = amn;
    mx = amx;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE AABB2D::AABB2D(
    float mnx,
    float mny,
    float mxx,
    float mxy)
{
    mn = Vec2(mnx,mny);
    mx = Vec2(mxx,mxy);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void AABB2D::clear()
{
    const float fmx = std::numeric_limits<float>::max();
    const float fmn = std::numeric_limits<float>::lowest();
    mn = Vec2(fmx,fmx);
    mx = Vec2(fmn,fmn);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void AABB2D::addPoint(Vec2 point)
{
    mn = Vec2::min(mn,point);
    mx = Vec2::max(mx,point);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void AABB2D::add(const AABB2D& aabb)
{
    mn = Vec2::min(mn,aabb.mn);
    mx = Vec2::max(mx,aabb.mx);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float AABB2D::width() const
{
    return mx.x() - mn.x();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float AABB2D::height() const
{
    return mx.y() - mn.y();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec2 AABB2D::center() const
{
    return (mx + mn) * 0.5f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE const Vec2& AABB2D::min() const
{
    return mn;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE const Vec2& AABB2D::max() const
{
    return mx;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec2 AABB2D::onCoord(Vec2 coord) const
{
    const Vec2 d = mx - mn;
    return
        Vec2(
            mn.x() + d.x() * coord.x(),
            mn.y() + d.y() * coord.y());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE AABB2D AABB2D::lerp(const AABB2D& lhs, const AABB2D& rhs, float factor)
{
    const Vec2 nmn = lhs.min() * (1.0f - factor) + rhs.min() * factor;
    const Vec2 nmx = lhs.max() * (1.0f - factor) + rhs.max() * factor;
    AABB2D aabb;
    aabb.addPoint(nmn);
    aabb.addPoint(nmx);
    return aabb;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE BoundingSphere::BoundingSphere(const AABB& aabb)
{
    center_ = aabb.center();
    radius_ = Vec3::length(aabb.size()) * 0.5f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 BoundingSphere::center() const
{
    return center_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float BoundingSphere::radius() const
{
    return radius_;
}
