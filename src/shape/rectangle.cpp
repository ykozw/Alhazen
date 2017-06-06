#include "pch.hpp"
#include "shape.hpp"
#include "core/orthonormalbasis.hpp"
#include "bsdf/bsdf.hpp"
#include "core/object.hpp"
#include "core/transform.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class RectangleShape
    :public Shape
{
public:
    RectangleShape(const ObjectProp& objectProp);
    bool hasAABB() const override;
    AABB aabb() const override;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;
private:
    std::array<Vec3, 4> verts_;
    std::array<Vec2, 4> uvs_;
    Vec3 n_;
    AABB aabb_;
};
REGISTER_OBJECT(Shape,RectangleShape);

/*
-------------------------------------------------
-------------------------------------------------
*/
RectangleShape::RectangleShape(const ObjectProp& objectProp)
    :Shape(objectProp)
{
    //
    verts_[0] = Vec3(-1.0f, -1.0f, 0.0f);
    verts_[1] = Vec3(1.0f, -1.0f, 0.0f);
    verts_[2] = Vec3(-1.0f, 1.0f, 0.0f);
    verts_[3] = Vec3(1.0f, 1.0f, 0.0f);
    uvs_[0] = Vec2(0.0f,0.0f);
    uvs_[1] = Vec2(1.0f, 0.0f);
    uvs_[2] = Vec2(0.0f, 1.0f);
    uvs_[3] = Vec2(1.0f, 1.0f);
    //
    const Transform transform(objectProp.findChildByTag("transform"));
    verts_[0] = transform.toWorld(verts_[0]);
    verts_[1] = transform.toWorld(verts_[1]);
    verts_[2] = transform.toWorld(verts_[2]);
    verts_[3] = transform.toWorld(verts_[3]);
    n_ = Vec3::cross(verts_[0] - verts_[1], verts_[0] - verts_[2]);
    n_.normalize();
    //
    aabb_.addPoints(
        static_cast<Vec3*>(verts_.data()),
        static_cast<int32_t>(verts_.size()));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool RectangleShape::hasAABB() const
{
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE AABB RectangleShape::aabb() const
{
    return aabb_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool RectangleShape::intersect(const Ray& ray, _Inout_ Intersect* isect) const
{
    const bool hit0 = intersectTriangle(ray, verts_[0], verts_[1], verts_[2], n_, n_, n_, uvs_[0], uvs_[1], uvs_[2], isect);
    const bool hit1 = intersectTriangle(ray, verts_[2], verts_[1], verts_[3], n_, n_, n_, uvs_[1], uvs_[2], uvs_[3], isect);
    if (!hit0 && !hit1)
    {
        return false;
    }
    isect->bsdf = bsdf_;
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool RectangleShape::intersectCheck(const Ray& ray) const
{
    // TODO: より軽い方法があるならそれに切り替える
    Intersect isDummy;
    return intersect(ray, &isDummy);
}
