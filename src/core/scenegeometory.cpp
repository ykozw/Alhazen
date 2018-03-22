#include "pch.hpp"
#include "core/scenegeometory.hpp"
#include "core/stats.hpp"

//
STATS_COUNTER("Isect", g_numIsectTotal, "Rays");
STATS_COUNTER("IsectIsect", g_numIsect, "Rays");
STATS_COUNTER("IsectCheck", g_numIsectCheck, "Rays");
STATS_COUNTER("IsectVisib", g_numIsectVisible, "Rays");

/*
-------------------------------------------------
-------------------------------------------------
*/
void
SceneGeometory::addShape(ShapePtr shape)
{
    shapes_.push_back(shape);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void
SceneGeometory::addLight(LightPtr light)
{
    lights_.push_back(light);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void
SceneGeometory::buildScene()
{
    // BVHの作成
    shapeBvh_.construct(shapes_);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const std::vector<LightPtr>&
SceneGeometory::lights() const
{
    return lights_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool
SceneGeometory::intersect(const Ray& ray,
                          bool skipLight,
                          Intersect* isect) const
{
    //
    ++g_numIsectTotal;
    ++g_numIsect;
    //
    bool isHit = false;

#if 0 // 総当たりの場合
    // Shapeを巡回する
    for (auto& shape : shapes_) {
        // 衝突のたびにRayのtminが更新されるので最も近いShapeが得られる
        if (shape->intersect(ray, isect)) {
            isect->sceneObject = shape.get();
            isHit = true;
        }
    }
#else // BVHの場合(少ないShape数だと逆にこちらのほうが遅いことに注意)
    // shapeとの交差
    isHit |= shapeBvh_.intersect(ray, isect);
#endif

    // Lightを巡回する
    if (!skipLight) {
        for (const auto& light : lights_) {
            if (light->intersect(ray, isect)) {
                AL_ASSERT_DEBUG(isect->bsdf);
                isect->sceneObject = light.get();
                isHit = true;
            }
        }
    }
    //
    isect->setHit(isHit);
    //
    return isHit;
}

/*
-------------------------------------------------
交差があるかのチェック。
交差がある場合はtrueが返る
-------------------------------------------------
*/
bool
SceneGeometory::intersectCheck(const Ray& ray, bool skipLight) const
{
    //
    ++g_numIsectTotal;
    ++g_numIsectCheck;
    // Shapeを巡回する
    for (auto& shape : shapes_) {
        if (shape->intersectCheck(ray)) {
            return true;
        }
    }

    // Lightを巡回する
    if (!skipLight) {
        for (const auto& light : lights_) {
            if (light->intersectCheck(ray)) {
                return true;
            }
        }
    }
    return false;
}

/*
-------------------------------------------------
指定した場所間が可視か
可視の場合はtrueが返る(isect系と意味が逆になっている)事に注意
-------------------------------------------------
*/
bool
SceneGeometory::isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const
{
    // NOTE: g_numIsectTotalはダブルカウントになるのでカウントしない事
    // ++g_numIsectTotal;
    ++g_numIsectVisible;
    //
    const Vec3 d = p1 - p0;
    Ray ray(p0, d);
    ray.maxt = d.length();
    return !intersectCheck(ray, skipLight);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AABB
SceneGeometory::aabb() const
{
    AABB aabb;
    //
    for (auto& shape : shapes_) {
        aabb.addAABB(shape->aabb());
    }
    // NOTE: Lightは含んでいないことに注意
    return aabb;
}
