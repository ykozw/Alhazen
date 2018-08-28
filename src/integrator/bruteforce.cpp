#include "bsdf/bsdf.hpp"
#include "core/orthonormalbasis.hpp"
#include "integrator.hpp"
#include "core/object.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class BruteForceIntegrator AL_FINAL : public LTEIntegrator
{
public:
    BruteForceIntegrator() = default;
    BruteForceIntegrator(const ObjectProp& objectProp);
    bool preRendering(const SceneGeom& scene) override;
    bool postRendering() override { return true; }
    Spectrum radiance(const Ray& ray,
                      const SceneGeom& scene,
                      Sampler* sampler) const override;
private:
};

REGISTER_OBJECT(LTEIntegrator, BruteForceIntegrator);

/*
-------------------------------------------------
-------------------------------------------------
*/
BruteForceIntegrator::BruteForceIntegrator(const ObjectProp& objectProp)
{
    static_cast<void>(objectProp);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool BruteForceIntegrator::preRendering(const SceneGeom& scene)
{
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum BruteForceIntegrator::radiance(const Ray& screenRay,
                                        const SceneGeom& scene,
                                        Sampler* sampler) const
{
    //
    Spectrum throughput = Spectrum(1.0f);
    Spectrum lighting = Spectrum(0.0f);
    Ray ray = screenRay;
    const int32_t maxBounce = 100;
    // bool isOutside = true;
    // float ior = 1.0f;
    //
    for (int32_t bounce = 0; bounce < maxBounce; ++bounce)
    {
        // ロシアンルーレット確率は現在のスループットから決定する
        if (bounce > maxBounce)
        {
            break;
        }
#if 0
        else if (bounce > 5)
        {
            const float prob = alMin(throughput.luminance(), 0.5f);
            if (rng_.nextFloat() > prob)
            {
                break;
            }
            throughput = throughput / prob;
        }
#endif
        //
        Intersect isect;
        // 何も衝突しなかったらContributionが0で終了
        // ライトを直接サンプルするわけではないのでライトと交差する必要がある
        bool skipLight = false;
        if (!scene.intersect(ray, skipLight, &isect))
        {
            break;
        }
        //
        if (!isect.emission.isBlack())
        {
            // const float d = isect.t;
            lighting += isect.emission * throughput;
        }
        //
        const BSDF* bsdf = isect.bsdf;
        Vec3 localWi;
        const OrthonormalBasis<> local(isect.normal);
        const Vec3 localWo = local.world2local(-ray.d);
//
#define USE_COSINE_IS
#if defined(USE_COSINE_IS)
        float pdfHS;
        localWi = sampler->getHemisphereCosineWeighted(&pdfHS);
#else
        localWi = sampler->getHemisphere();
#endif
        Spectrum reflection = bsdf->bsdf(localWo, localWi);
        // 反射率が0の場合は終了
        if (reflection.isBlack())
        {
            break;
        }
        const Vec3 worldWi = local.local2world(localWi);

#if defined(USE_COSINE_IS)
        const float pdfHsOverCosTheta = 1.0f; /* = localWi.z / pdfHS; */
        throughput *= reflection * pdfHsOverCosTheta * 2.0f * PI *
                      0.5f; // NOTE: 最後の0.5はどこから...?
#else
        throughput *= reflection * localWi.z * 2.0f * PI;
#endif
        // 次のrayを作成する
        const Vec3 rayOrig = (localWi.z() >= 0.0f) ? isect.uppserSideOrigin()
                                                   : isect.belowSideOrigin();
        ray = Ray(rayOrig, worldWi, isect.rayEpsilon);
    }
    //
    AL_ASSERT_DEBUG(!lighting.hasNaN());
    return lighting;
}
