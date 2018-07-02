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
    return geometory_->intersect(ray, skipLight, isect);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SceneGeom::intersectCheck(const Ray& ray, bool skipLight) const
{
    return geometory_->intersectCheck(ray, skipLight);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const std::vector<LightPtr>& SceneGeom::lights() const
{
    return geometory_->lights();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SceneGeom::isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const
{
    return geometory_->isVisible(p0, p1, skipLight);
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
void SceneGeom::addLight(LightPtr light) { geometory_->addLight(light); }

/*
-------------------------------------------------
-------------------------------------------------
*/
void SceneGeom::buildScene()
{
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
void IntersectSceneBasic::addLight(LightPtr light) { lights_.push_back(light); }

/*
-------------------------------------------------
-------------------------------------------------
*/
void IntersectSceneBasic::buildScene()
{
    // BVHの作成
    shapeBvh_.construct(shapes_);
    // ライトがなかったら全天球ライトを配置する
    if (lights_.empty())
    {
        auto light = std::make_shared<ConstantLight>();
        light->init(Spectrum::White);
        lights_.push_back(light);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const std::vector<LightPtr>& IntersectSceneBasic::lights() const { return lights_; }

/*
-------------------------------------------------
-------------------------------------------------
*/
bool IntersectSceneBasic::intersect(const Ray& ray,
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
    }
    //
    isect->setHit(isHit);
    //
    return isHit;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool IntersectSceneBasic::intersect2(const Ray& ray,
                                        Intersect* isect) const
{
    bool isHit = false;
    for (auto& mi : meshInfos_)
    {
        // TODO: bvh専用のIntersectにしてprimを返すようにする
        if (mi.bvh_.intersect(ray, isect))
        {
            AL_ASSERT_ALWAYS(false);
            //isect->sceneObject = mi.shape_.get();
            //isect->uvBicentric;
            //mi.interpolateFun(isect->uvBicentric, )
            isHit = true;
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
bool IntersectSceneBasic::intersectCheck(const Ray& ray, bool skipLight) const
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
bool IntersectSceneBasic::isVisible(const Vec3& p0,
                               const Vec3& p1,
                               bool skipLight) const
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
