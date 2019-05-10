#pragma once

#include "fwd.hpp"
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
    Shape(const ObjectProp& prop) {}
    virtual ~Shape() {}
    virtual void mapToIntersectEngine(IsectScene* isectScene) {}
    virtual void resolveMaterial(const std::vector<BSDFPtr>& bsdfs) {}

    // TODO: これらは廃止する
    virtual AABB aabb() const = 0;
    virtual bool intersect(const Ray& ray, Intersect* isect) const = 0;
    virtual bool intersectCheck(const Ray& ray) const = 0;

protected:
};
typedef std::shared_ptr<Shape> ShapePtr;
