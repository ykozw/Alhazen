#pragma once

#include "core/math.hpp"
#include "core/ray.hpp"
#include "core/scenegeometory.hpp"

/*
TODOs
- addShape()を崩して、addMesh()と同じ構造にする
- Lightを持つ構造はここには用意したくない。
  シーンを複数作るようにして呼び出し側でそういうのを解決するようにする
*/

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
struct IsectSceneEmbree : public IsectScene
{
public:
    IsectSceneEmbree() = default;
    IsectSceneEmbree(RTCDevice device);
    ~IsectSceneEmbree();
    IsectGeomID addMesh(
        int32_t numVtx,
        int32_t numFace,
        const std::function<Vec3(int32_t idx)>& getVtx,
        const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace);
    
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
    bool intersect(const Ray& ray,
        bool skipLight,
        Intersect* isect) const override;
    bool intersectCheck(const Ray& ray, bool skipLight) const override;
    bool isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const override;
    AABB aabb() const override;

public:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectEmbree2
{
public:
    IsectEmbree2();
    ~IsectEmbree2();
    std::unique_ptr<IsectSceneEmbree> createScene();

public:
    class Impl;
    std::unique_ptr<Impl> impl_;
};