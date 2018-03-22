#pragma once

#include "pch.hpp"
#include "core/ray.hpp"
#include "core/intersect.hpp"
#include "core/bounding.hpp"
#include "core/object.hpp"
#include "bsdf/bsdf.hpp"

/*
-------------------------------------------------
シーン中に配置されるShapeの基底
次のような前提を置く
- 必ず有限のサイズであること。
  これを満たすため、Planeなどの形状はサポートしない
-------------------------------------------------
*/
class Shape : public SceneObject
{
public:
    Shape(const ObjectProp& prop);
    virtual ~Shape() {}
    virtual AABB aabb() const = 0;
    virtual bool intersect(const Ray& ray, Intersect* isect) const = 0;
    virtual bool intersectCheck(const Ray& ray) const = 0;
    void setBSDF(AllBSDFList bsdfs);
    virtual bool hasMultiBSDFs() { return false; }
    BSDFPtr getBSDF() { return bsdf_; }

protected:
    // 複数のBSDFを持つShapeの場合はhasMultiBSDFs()と合わせてオーバーロードする
    virtual void setBSDFs(AllBSDFList bsdfs);

protected:
    // 単体のBSDFを持つときのみこのBSDFを使う
    std::string bsdfName_;
    BSDFPtr bsdf_;
};
typedef std::shared_ptr<Shape> ShapePtr;
