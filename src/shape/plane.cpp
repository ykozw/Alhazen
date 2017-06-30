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
class PlaneShape
    :public Shape
{
public:
    PlaneShape(const ObjectProp& objectProp);
    PlaneShape(const Vec3& dir, float dist);
    bool hasAABB() const override;
    AABB aabb() const override;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;
private:
    Vec3 dir_;
    float dist_;
    OrthonormalBasis<> local_;
};
REGISTER_OBJECT(Shape,PlaneShape);

/*
-------------------------------------------------
-------------------------------------------------
*/
PlaneShape::PlaneShape(const ObjectProp& objectProp)
    : Shape(objectProp)
{
    // 基本はY-upの平面
    dir_ = Vec3(0.0f,1.0f,0.0);
    dist_ = 0.0f;
    // 
    const Transform transform(objectProp.findChildByTag("transform"));
    // 方向の回転なので、逆転地行列を使う
    dir_ = transform.toWorldDir(dir_);
    local_.set(dir_);
    // Y-upなのでe42の値がそのままdistになる
    dist_ = transform.toWorld_.e42;
}
/*
-------------------------------------------------
-------------------------------------------------
*/
PlaneShape::PlaneShape(const Vec3& dir, float dist)
    :dir_(dir),
     dist_(dist),
     local_(dir),
    Shape(ObjectProp())
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool PlaneShape::hasAABB() const
{
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AABB PlaneShape::aabb() const
{
    AL_ASSERT_DEBUG(false);
    return AABB();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool PlaneShape::intersect(const Ray& ray, _Inout_ Intersect* isect) const
{
    if (!intersectPlane(ray, dir_, dist_, isect))
    {
        return false;
    }
    // ダミーのUV座標を計算する
    const float uvSpaceWidth = 1.0;
    const Vec3 localPos = local_.world2local(isect->position);
    Vec2& uv = isect->uv;
    const float x = fmodf(fabsf(localPos.x()), uvSpaceWidth);
    const float y = fmodf(fabsf(localPos.y()), uvSpaceWidth);
    uv = Vec2(x,y);
    isect->bsdf = bsdf_;
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool PlaneShape::intersectCheck(const Ray& ray) const
{
    return intersectPlaneCheck(ray, dir_, dist_);
}
