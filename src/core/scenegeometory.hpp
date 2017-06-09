#pragma once

#include "pch.hpp"
#include "core/object.hpp"
#include "core/math.hpp"
#include "shape/shape.hpp"
#include "light/light.hpp"

//-------------------------------------------------
// SceneGeometory
//-------------------------------------------------
class SceneGeometory
{
public:
    void add(ShapePtr shape);
    void add(LightPtr light);
    const std::vector<LightPtr>& lights() const;
    bool intersect(const Ray& ray, bool skipLight, _Out_ Intersect* isect) const;
    bool intersectCheck(const Ray& ray, bool skipLight) const;
    bool isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const;
    //
private:
    std::vector<ShapePtr> shapes_;
    // ShapeとLight
    // Lightのみ
    std::vector<LightPtr> lights_;
};
