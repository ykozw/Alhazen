#include "pch.hpp"
#include "core/rng.hpp"
#include "core/orthonormalbasis.hpp"
#include "integrator/integrator.hpp"
#include "sampler/sampler.hpp"


/*
-------------------------------------------------
-------------------------------------------------
*/
class AOSurfaceIntegrator
    :public LTEIntegrator
{
public:
    AOSurfaceIntegrator() {}
    AOSurfaceIntegrator(const ObjectProp& objectProp);
    bool preRendering(const SceneGeometory& scene, AllBSDFList& bsdfList) override;
    bool postRendering() override
    {
        return true;
    }
    Spectrum radiance(const Ray& ray, const SceneGeometory& scene, SamplerPtr sampler) override;
private:
    int32_t sampleNum_ = 0;
};
REGISTER_OBJECT(LTEIntegrator, AOSurfaceIntegrator);

/*
-------------------------------------------------
-------------------------------------------------
*/
AOSurfaceIntegrator::AOSurfaceIntegrator(const ObjectProp& objectProp)
{
    sampleNum_ = objectProp.findChildBy("name","sampleNum").asInt(64);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool AOSurfaceIntegrator::preRendering(const SceneGeometory& scene, AllBSDFList& bsdfList)
{
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum AOSurfaceIntegrator::radiance(
    const Ray& screenRay,
    const SceneGeometory& scene,
    SamplerPtr sampler)
{
    // 何もない場合は0を返す
    Intersect isect;
    const bool skipLight = true;
    if (!scene.intersect(screenRay, skipLight, &isect))
    {
        return Spectrum(0.0f);
    }
    OrthonormalBasis<> local;
    local.set(isect.normal);
    //
    float visibility = 0.0f;
    for(int32_t sn=0;sn<sampleNum_;++sn)
    {
        sampler->startSample(sn);
        float pdf;
        const Vec3 localDir = sampler->getHemisphereCosineWeighted(&pdf);
        const Vec3 worldDir = local.local2world(localDir);
        const Vec3 pos = isect.uppserSideOrigin();
        Ray newRay(pos, worldDir, isect.rayEpsilon);
        const bool skipLight = true;
        const bool isHit = scene.intersectCheck(newRay, skipLight);
        const float cosWeight = Vec3::dot(worldDir, isect.normal);
        const float contribution = !isHit ? cosWeight*2.0f / pdf : 0.0f;
        visibility += contribution;
    }
    const float v = visibility / sampleNum_;
    return Spectrum(v);
}
