#pragma once

#include "core/object.hpp"
#include "core/math.hpp"
#include "shape/shape.hpp"
#include "light/light.hpp"
#include "accelerator/bvh.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectScene
{
public:
    virtual void addShape(ShapePtr shape) = 0;
    virtual void addLight(LightPtr light) = 0;
    virtual void buildScene() = 0;
    virtual const std::vector<LightPtr>& lights() const  = 0;
    virtual bool intersect(const Ray& ray,
        bool skipLight,
        Intersect* isect) const = 0;
    virtual bool intersectCheck(const Ray& ray, bool skipLight) const = 0;
    virtual bool isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const = 0;
    virtual AABB aabb() const = 0;
private:
};

std::unique_ptr<IsectScene> createIsectScene();
