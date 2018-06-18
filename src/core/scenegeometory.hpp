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
struct IsectGeomID
{
public:
    IsectGeomID(int32_t id) : id_(id) {}
    int32_t id() const { return id_; }
public:
    int32_t id_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectScene
{
public:
    virtual void addShape(ShapePtr shape) = 0;
    virtual void addLight(LightPtr light) = 0;
    //
    virtual IsectGeomID addMesh(
        int32_t numVtx,
        int32_t numFace,
        const std::function<Vec3(int32_t idx)>& getVtx,
        const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace)
    {}
    //
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
