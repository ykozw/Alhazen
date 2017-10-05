#pragma once

#include "pch.hpp"
#include "core/ray.hpp"
#include "core/intersect.hpp"
#include "core/aabb.hpp"
#include "core/object.hpp"
#include "bsdf/bsdf.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class Shape
    :public SceneObject
{
public:
    Shape(const ObjectProp& prop);
    virtual ~Shape() {}
    virtual bool hasAABB() const = 0;
    virtual AABB aabb() const = 0;
    virtual bool intersect(const Ray& ray, _Inout_ Intersect* isect) const = 0;
    virtual bool intersectCheck(const Ray& ray) const = 0;
    void setBSDF(AllBSDFList bsdfs);
    virtual bool hasMultiBSDFs() { return false; }
    BSDFPtr getBSDF() { return bsdf_;  }
protected:
    // 複数のBSDFを持つShapeの場合はhasMultiBSDFs()と合わせてオーバーロードする
    virtual void setBSDFs(AllBSDFList bsdfs);
protected:
    // 単体のBSDFを持つときのみこのBSDFを使う
    std::string bsdfName_;
    BSDFPtr bsdf_;
};
typedef std::shared_ptr<Shape> ShapePtr;
