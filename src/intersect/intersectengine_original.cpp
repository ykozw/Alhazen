#include "intersect/intersectengine.hpp"
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
SceneGeom::SceneGeom()
{
#if 1
    isectEngine_ = std::make_unique<IsectEngineBasic>();
#else
    isectEngine_ = std::make_unique<IsectEngineEmbreeV3>();
#endif
    geometory_ = isectEngine_->createScene();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool
SceneGeom::intersect(const Ray& ray, bool skipLight, Intersect* isect) const
{
    bool isHit = false;
    isHit = geometory_->intersect(ray, isect);
    // Lightを巡回する
    // TODO: ライトを登録してそこからプリミティブIDを得るような形にする
    if (!skipLight)
    {
        for (const auto& light : lights_)
        {
            if (light->intersect(ray, isect))
            {
                AL_ASSERT_DEBUG(isect->bsdf);
                isect->sceneObject = light.get();
                isHit = true;
            }
        }
        //
        if (lightsBVH_.intersect(ray, isect))
        {
            isHit = true;
        }
    }
    return isHit;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SceneGeom::intersectCheck(const Ray& ray, bool skipLight) const
{
    //
    if (geometory_->intersectCheck(ray))
    {
        return true;
    }
    // Lightを巡回する
    // TODO: ライトを登録してそこからプリミティブIDを得るような形にする
    if (!skipLight)
    {
        for (const auto& light : lights_)
        {
            if (light->intersectCheck(ray))
            {
                return true;
            }
        }
        //
        if (lightsBVH_.intersectCheck(ray))
        {
            return true;
        }
    }
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const std::vector<LightPtr>& SceneGeom::lights() const
{
    return lights_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SceneGeom::isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const
{
    if (!geometory_->isVisible(p0, p1))
    {
        return false;
    }
    // TODO: 実装
    if (!skipLight)
    {
        AL_ASSERT_ALWAYS(false);
    }
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SceneGeom::addShape(ShapePtr shape) { geometory_->addShape(shape); }

/*
-------------------------------------------------
-------------------------------------------------
*/
void SceneGeom::addLight(LightPtr light)
{
    this->lights_.push_back(light);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SceneGeom::buildScene()
{
    // ライトがなかったら全天球ライトを配置する
    if (lights_.empty())
    {
        auto light = std::make_shared<ConstantLight>();
        light->init(Spectrum::White);
        lights_.push_back(light);
    }
    else
    {
        std::vector<LightPtr> aabbLights(lights_.size());
        auto ite = std::copy_if(lights_.begin(), lights_.end(), aabbLights.begin(), [](const LightPtr& light) {return light->aabb().validate(); });
        aabbLights.resize(std::distance(aabbLights.begin(), ite));
        lightsBVH_.construct(aabbLights);
        //
        lights_.erase(std::remove_if(lights_.begin(),
                                     lights_.end(),
                                     [](const LightPtr& light) {
                                         return light->aabb().validate();
                                     }),
                      lights_.end());
    }
    //
    geometory_->buildScene();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AABB SceneGeom::aabb() const
{
    return geometory_->aabb();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
std::unique_ptr<IsectScene> IsectEngineBasic::createScene()
{
    return std::make_unique<IntersectSceneBasic>();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void IntersectSceneBasic::addShape(ShapePtr shape) { shapes_.push_back(shape); }

/*
-------------------------------------------------
-------------------------------------------------
*/
void IntersectSceneBasic::addMesh(
    int32_t numVtx,
    int32_t numFace,
    const std::function<Vec3(int32_t vi)>& getVtx,
    const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace,
    const InterpolateFun& interpolateFun)
{
    /*
    TODOs
    - 頂点法線などはEmbreeに渡す必要はないがどこに管理するべきなのか？
    */
    // 全ての頂点情報を集める
    std::vector<Vec3> vs(numVtx);
    std::vector<Vec3> ns(numVtx);
    std::vector<Vec2> ts(numVtx);
    for (int32_t vi=0;vi<numVtx;++vi)
    {
        vs[vi] = getVtx(vi);
    }
    std::vector<MeshFace> fs(numFace);
    for (int32_t fi=0;fi<numFace;++fi)
    {
        auto f = getFace(fi);
        MeshFace& face = fs[fi];
        face.vi[0] = f[0];
        face.vi[1] = f[1];
        face.vi[2] = f[2];
        face.ni[0];
        face.ti;
        face.ti;
    }
    //
    MeshInfo meshInfo;
    meshInfo.bvh_.construct(vs, ns, ts, fs);
    meshInfo.interpolateFun = interpolateFun;
    meshInfos_.push_back(meshInfo);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void IntersectSceneBasic::buildScene()
{
    // BVHの作成
    shapeBvh_.construct(shapes_);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool IntersectSceneBasic::intersect(const Ray& ray,
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
bool IntersectSceneBasic::intersectCheck(const Ray& ray) const
{
    //
    ++g_numIsectTotal;
    ++g_numIsectCheck;
// Shapeを巡回する
#if 0 // 総当たり
    for (auto& shape : shapes_) {
        if (shape->intersectCheck(ray)) {
            return true;
        }
    }
#else
    if (shapeBvh_.intersectCheck(ray))
    {
        return true;
    }
#endif

    return false;
}

/*
-------------------------------------------------
指定した場所間が可視か
可視の場合はtrueが返る(isect系と意味が逆になっている)事に注意
-------------------------------------------------
*/
bool IntersectSceneBasic::isVisible(const Vec3& p0,
                               const Vec3& p1) const
{
    // NOTE: g_numIsectTotalはダブルカウントになるのでカウントしない事
    // ++g_numIsectTotal;
    ++g_numIsectVisible;
    //
    const Vec3 d = p1 - p0;
    Ray ray(p0, d);
    ray.maxt = d.length();
    return !intersectCheck(ray);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AABB IntersectSceneBasic::aabb() const
{
    AABB aabb;
    //
    for (auto& shape : shapes_)
    {
        aabb.addAABB(shape->aabb());
    }
    // NOTE: Lightは含んでいないことに注意
    return aabb;
}
