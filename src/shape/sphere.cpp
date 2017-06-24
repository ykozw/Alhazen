#include "pch.hpp"
#include "shape.hpp"
#include "core/object.hpp"
#include "bsdf/bsdf.hpp"

/*
-------------------------------------------------
Sphere
Sphereの移行が完了したらShpreを消す
-------------------------------------------------
*/
class Sphere
    :public Shape
{
public:
    Sphere(const ObjectProp& prop );
    Sphere(const Vec3& pos, float r);
    bool hasAABB() const override;
    AABB aabb() const override;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;
private:
    Vec3 pos_;
    float r_;
    AABB aabb_;
};
REGISTER_OBJECT(Shape,Sphere);

/*
-------------------------------------------------
-------------------------------------------------
*/
Sphere::Sphere(const ObjectProp& prop)
    :Shape(prop)
{
    // 幾何情報を取得
    pos_ = prop.findChildBy("name","center").asXYZ(Vec3(0.0f));
    r_ =  prop.findChildBy("name","radius").asFloat(1.0f);
    const Vec3 xdir = Vec3(r_, 0.0f, 0.0f);
    const Vec3 ydir = Vec3(0.0f, r_, 0.0f);
    const Vec3 zdir = Vec3(0.0f, 0.0f, r_);
    aabb_.addPoint(pos_ + xdir);
    aabb_.addPoint(pos_ - xdir);
    aabb_.addPoint(pos_ + ydir);
    aabb_.addPoint(pos_ - ydir);
    aabb_.addPoint(pos_ + zdir);
    aabb_.addPoint(pos_ - zdir);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Sphere::hasAABB() const
{
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AABB Sphere::aabb() const
{
    return aabb_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Sphere::intersect(const Ray& ray, _Inout_ Intersect* isect) const
{
    if (intersectSphere(ray, pos_, r_, isect))
    {
        isect->bsdf = bsdf_;
        return true;
    }
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Sphere::intersectCheck(const Ray& ray) const
{
    return intersectSphereCheck(ray, pos_, r_);
}
