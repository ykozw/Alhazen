﻿#pragma once

#include "pch.hpp"
#include "core/object.hpp"
#include "core/math.hpp"
#include "shape/shape.hpp"
#include "light/light.hpp"
#include "accelerator/bvh.hpp"

/*
-------------------------------------------------
TODO:
- 二段階BVHを入れるようにする
-------------------------------------------------
*/
class SceneGeometory AL_FINAL
{
public:
    void addShape(ShapePtr shape);
    void addLight(LightPtr light);
    void buildScene();
    const std::vector<LightPtr>& lights() const;
    bool intersect(const Ray& ray,
                   bool skipLight,
                   Intersect* isect) const;
    bool intersectCheck(const Ray& ray, bool skipLight) const;
    bool isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const;
    AABB aabb() const;

private:
    // Shape
    std::vector<ShapePtr> shapes_;
    ShapeBVH shapeBvh_;
    // Light
    std::vector<LightPtr> lights_;
};
