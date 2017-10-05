#include "pch.hpp"
#include "scenegeometory.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
void SceneGeometory::add(ShapePtr shape)
{
    shapes_.push_back(shape);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SceneGeometory::add(LightPtr light)
{
    lights_.push_back(light);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const std::vector<LightPtr>& SceneGeometory::lights() const
{
    return lights_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SceneGeometory::intersect(
    const Ray& ray,
    bool skipLight,
    _Inout_ Intersect* isect) const
{
    bool isHit = false;
    // Shapeを巡回する
    for (auto& shape : shapes_)
    {
        // 衝突のたびにRayのtminが更新されるので最も近いShapeが得られる
        if (shape->intersect(ray, isect))
        {
#if 1
            AL_ASSERT_DEBUG(isect->bsdf);
#else
            if (!shape->hasMultiBSDFs())
            {
                isect->bsdf = shape->getBSDF();
            }
#endif
            isect->sceneObject = shape.get();
            isHit = true;
        }
    }

    // Lightを巡回する
    if (!skipLight)
    {
        for (const auto& light : lights_)
        {
            if (light->intersect(ray, isect))
            {
                AL_ASSERT_DEBUG(isect->bsdf);
                isect->sceneObject = light.get();
                isect->isLight = true;
                isHit = true;
            }
        }
    }

    return isHit;
}

/*
-------------------------------------------------
交差があるかのチェック。
交差がある場合はtrueが返る
-------------------------------------------------
*/
bool SceneGeometory::intersectCheck(const Ray& ray, bool skipLight) const
{
    // Shapeを巡回する
    for (auto& shape : shapes_)
    {
        if (shape->intersectCheck(ray))
        {
            return true;
        }
    }

    // Lightを巡回する
    if (!skipLight)
    {
        for (const auto& light : lights_)
        {
            if (light->intersectCheck(ray))
            {
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
bool SceneGeometory::isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const
{
    const Vec3 d = p1 - p0;
    Ray ray(p0, d);
    ray.maxt = d.length();
    return !intersectCheck(ray, skipLight);
}
