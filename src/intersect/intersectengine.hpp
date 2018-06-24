#pragma once

#include "shape/shape.hpp"
#include "accelerator/bvh.hpp"
#include "core/math.hpp"
#include "core/object.hpp"
#include "core/ray.hpp"
#include "light/light.hpp"
#include "intersect/intersectengine.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectScene
{
public:
    struct Interpolated
    {
        Vec3 position;
        // シェーディング法線
        Vec3 ns;
        // ジオメトリ法線
        Vec3 ng;
    };
    typedef std::function<Interpolated(int32_t primId, Vec2 biuv)> InterpolateFun;
public:
    virtual void addShape(ShapePtr shape) = 0;
    virtual void addLight(LightPtr light) = 0;
    //
    virtual void addMesh(
        int32_t numVtx,
        int32_t numFace,
        const std::function<Vec3(int32_t idx)>& getVtx,
        const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace,
        const InterpolateFun& intepolate) = 0;
    //
    virtual void buildScene() = 0;
    virtual const std::vector<LightPtr>& lights() const = 0;
    virtual bool
    intersect(const Ray& ray, bool skipLight, Intersect* isect) const = 0;
    virtual bool intersectCheck(const Ray& ray, bool skipLight) const = 0;
    virtual bool
    isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const = 0;
    virtual AABB aabb() const = 0;

private:
};

std::unique_ptr<IsectScene> createIsectScene();

/*
-------------------------------------------------
-------------------------------------------------
*/
struct IsectSceneEmbree : public IsectScene
{
public:
    IsectSceneEmbree();
    ~IsectSceneEmbree();
    void addMesh(
        int32_t numVtx,
        int32_t numFace,
        const std::function<Vec3(int32_t idx)>& getVtx,
        const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace,
        const InterpolateFun& interpolate) override;

    //
    void addShape(ShapePtr shape) override { AL_ASSERT_ALWAYS(false); }
    void addLight(LightPtr light) override { AL_ASSERT_ALWAYS(false); }
    void buildScene() override;
    const std::vector<LightPtr>& lights() const override
    {
        AL_ASSERT_ALWAYS(false);
        static std::vector<LightPtr> l;
        return l;
    }
    bool
    intersect(const Ray& ray, bool skipLight, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray, bool skipLight) const override;
    bool
    isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const override;
    AABB aabb() const override;

public:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectEmbreeV3
{
public:
    IsectEmbreeV3();
    ~IsectEmbreeV3();
    std::unique_ptr<IsectSceneEmbree> createScene();

public:
    class Impl;
    std::unique_ptr<Impl> impl_;
};