#include "pch.hpp"
#include "integrator.hpp"
#include "core/orthonormalbasis.hpp"
#include "bsdf/bsdf.hpp"

//
#include "core/object.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class PTSurfaceIntegrator
    :public SurfaceIntegrator
{
public:
    PTSurfaceIntegrator() = default;
    PTSurfaceIntegrator(const ObjectProp& objectProp);
    bool preRendering(const SceneGeometory& scene, AllBSDFList& bsdfList) override;
    bool postRendering() override
    {
        return true;
    }
    //
    Spectrum radiance(
        const Ray& ray,
        const SceneGeometory& scene,
        SamplerPtr sampler) override;
    //
    SurfaceIntegratorPtr clone() override
    {
        PTSurfaceIntegrator* pt = new PTSurfaceIntegrator;
        pt->defaultBSDF_ = defaultBSDF_;
        return SurfaceIntegratorPtr(pt);
    }
private:
    Spectrum estimateDirectLight(
        const SceneGeometory& scene,
        const Intersect& isect,
        const OrthonormalBasis<>& local,
        const Vec3& localWo,
        SamplerPtr sampler);
    Spectrum estimateOneLight(
        const SceneGeometory& scene,
        const Intersect& isect,
        const OrthonormalBasis<>& local,
        const Vec3& localWo,
        const LightPtr& light,
        SamplerPtr samler);
private:
    BSDFPtr defaultBSDF_;

    // 直接光のライトの選択の戦略
    enum class DirectLighitingSelectStrategy
    {
        // 毎回全てのライトをサンプル
        All,
        // 等確率で一つを選択する
        UniformOne,
        // TODO: ライトの光量による重みづけ
        // TODO: シェーディング点近くへの重みづけ
    };
    DirectLighitingSelectStrategy directLighitingLightSelectStrategy_ = DirectLighitingSelectStrategy::UniformOne;
};

REGISTER_OBJECT(SurfaceIntegrator,PTSurfaceIntegrator);

/*
-------------------------------------------------
-------------------------------------------------
*/
PTSurfaceIntegrator::PTSurfaceIntegrator(const ObjectProp& objectProp)
{
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool PTSurfaceIntegrator::preRendering(const SceneGeometory& scene, AllBSDFList& bsdfList)
{
    defaultBSDF_ = bsdfList.find("default");
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum PTSurfaceIntegrator::radiance(
    const Ray& screenRay,
    const SceneGeometory& scene,
    SamplerPtr sampler)
{
    //
    Spectrum throughput = Spectrum(1.0f);
    Spectrum lighting = Spectrum(0.0f);
    Ray ray = screenRay;
    const int32_t maxpathNo = 8;
    bool isOutside = true;
    const float rrProb = sampler->get1d();
    //
    for (int32_t pathNo = 0; pathNo < maxpathNo; ++pathNo)
    {
        // ロシアンルーレット確率は現在のスループットから決定する
        if (pathNo > maxpathNo)
        {
            break;
        }
#if 0
        // TODO: ロシアンルーレット開始はプロパティ化する
        else if (pathNo > 5)
        {
            const float prob = alMin(throughput.luminance(), 1.0f);
            if (rrProb > prob)
            {
                break;
            }
            throughput = throughput / prob;
            AL_ASSERT_DEBUG(!throughput.hasNaN());
        }
#endif
        // 交差判定。何にも交差しない場合は終了。
        Intersect isect;
        const bool skipLight = (pathNo == 0) ? false : true;
        if (!scene.intersect(ray, skipLight, &isect))
        {
            break;
        }
        // PrimaryRayかつその交差先がライトであった場合、
        // そのライトはNEEに含まれないのでここでサンプル
        if ((pathNo == 0) && isect.isLight)
        {
            const Light* light = static_cast<const Light*>(isect.shape);
            Spectrum e = light->emittion(ray.o,ray.d);
            lighting += e;
            // HACK: とりあえず反射率0のライトのみとする
            break;
        }
        //
        BSDFPtr bsdf = isect.bsdf;
        float pdfBSDF;
        Vec3 localWi;
        const OrthonormalBasis<> local(isect.normal);
        const Vec3 localWo = local.world2local(-ray.d);
        // 直接光の推定
        const Spectrum directLighting = estimateDirectLight(scene, isect, local, localWo, sampler);
        AL_ASSERT_DEBUG(!directLighting.hasNaN());
        lighting += directLighting * throughput;
        //
        AL_ASSERT_DEBUG(!lighting.hasNaN());
        // 反射率の算出
        const Spectrum reflectance = bsdf->bsdfSample(localWo, sampler, &localWi, &pdfBSDF);
        AL_ASSERT_DEBUG(!reflectance.hasNaN());
        // PDFが0の場合は終了
        if (pdfBSDF == 0.0f)
        {
            break;
        }
        // 反射率が0の場合は終了
        if (reflectance.isBlack())
        {
            break;
        }
        const Vec3 worldWi = local.local2world(localWi);
        throughput = throughput * reflectance * fabsf(localWi.z) / pdfBSDF * INV_PI; // TODO: invpiはどうやって取り除くのか？pdf()の単位を変えればいいのか？
        AL_ASSERT_DEBUG(!throughput.hasNaN());
        // 次のrayを作成する
        const Vec3 rayOrig = (localWi.z >= 0.0f) ? isect.uppserSideOrigin() : isect.belowSideOrigin();
        ray = Ray(rayOrig, worldWi, isect.rayEpsilon);
    }
    //
    AL_ASSERT_DEBUG(!lighting.hasNaN());
    return lighting;
}

/*
-------------------------------------------------
指定したisect/localWiでの直接光全体のContributionを推定
-------------------------------------------------
*/
Spectrum PTSurfaceIntegrator::estimateDirectLight(
    const SceneGeometory& scene,
    const Intersect& isect,
    const OrthonormalBasis<>& local,
    const Vec3& localWo,
    SamplerPtr sampler)
{
    switch (directLighitingLightSelectStrategy_)
    {
        // 全てのライトを評価
    case DirectLighitingSelectStrategy::All:
        {
            const std::vector<LightPtr>& lights = scene.lights();
            Spectrum estimated;
            for (const auto& light : lights)
            {
                estimated += estimateOneLight(scene, isect, local, localWo, light, sampler);
            }
            return estimated;
        }
        break;
        // ライトを一つだけ選択
    case DirectLighitingSelectStrategy::UniformOne:
        {
            const std::vector<LightPtr>& lights = scene.lights();
            if (lights.empty())
            {
                Spectrum::createAsBlack();
            }
            // ライトを一つ選択する
            const uint32_t lightIndex = sampler->getSize(uint32_t(lights.size()));
            const LightPtr choochenLight = lights[lightIndex];
            // ライトをサンプルする場合にライトに対してintersect()を呼んではいけない
            AL_ASSERT_DEBUG(isect.shape != choochenLight.get());
            //
            const Spectrum oneLightEstimated = estimateOneLight(scene, isect, local, localWo, choochenLight, sampler);
            return oneLightEstimated * (float)lights.size();
        }
        break;
    default:
        AL_ASSERT_ALWAYS(false);
        return Spectrum::createAsBlack();
        break;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
static float misBalanceHeuristic(float fPdf, float gPdf)
{
    return fPdf / (fPdf + gPdf);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
static float misPowerHeuristic(float fPdf, float gPdf)
{
    const float fPdf2 = fPdf * fPdf;
    const float gPdf2 = gPdf * gPdf;
    return fPdf2 / (fPdf2 + gPdf2);
}

/*
-------------------------------------------------
指定したisect/localWiでの指定されたライトの直接光のContributionを推定
-------------------------------------------------
*/
Spectrum PTSurfaceIntegrator::estimateOneLight(
    const SceneGeometory& scene,
    const Intersect& isect,
    const OrthonormalBasis<>& local,
    const Vec3& localWo,
    const LightPtr& light,
    SamplerPtr sampler)
{
    BSDFPtr bsdf = isect.bsdf;
    /*
    サンプル方法の選択
    ライトとBSDFのδ関数の組み合わせによって変化する
    TODO: もう少しいい名前にする。
    */
    enum class SampleStrategy
    {
        // 常に0を返す
        AlwaysZero,
        // ライトのサンプルだけを行う
        OnlyLightSample,
        // BSDFのサンプルだけを行う
        OnlyBSDFSample,
        // ライトとBSDFのサンプルを行いMISする
        MISSample,
    };
    const auto takeStragety = [](const BSDFPtr bsdf, const LightPtr light) -> SampleStrategy
    {
        if (bsdf->isDeltaFunc())
        {
            if (light->isDeltaFunc())
            {
                return SampleStrategy::AlwaysZero;
            }
            else
            {
                return SampleStrategy::OnlyBSDFSample;
            }
        }
        else
        {
            if (light->isDeltaFunc())
            {
                return SampleStrategy::OnlyLightSample;
            }
            else
            {
                return SampleStrategy::MISSample;
            }
        }
    };
    const SampleStrategy sampleStrategy = takeStragety(bsdf, light);

    //
    switch (sampleStrategy)
    {
    case SampleStrategy::AlwaysZero:
        return Spectrum::createAsBlack();
    case SampleStrategy::OnlyLightSample:
        {
            Spectrum spectrum(0.0f);
            //
            for (int32_t i=0;i<4;++i)
            {
                // 向かう先の算出
                Vec3 localWi;
                float pdf;
                const Spectrum reflectance = bsdf->bsdfSample(localWo, sampler, &localWi, &pdf);
                const Vec3 worldWi = local.local2world(localWi);
                // レイ指定でのEmittionの取得
                // TODO: あれ、これは方向が逆になってない？
                const Spectrum emittion = light->emittion(isect.uppserSideOrigin(), worldWi);
                const float cosTerm = fabsf(localWi.z);
                spectrum += reflectance * emittion * 4.0f * cosTerm / pdf;
            }
            return spectrum;
        }
        break;
    case SampleStrategy::MISSample:
        {
            Spectrum spectrum(0.0f);

            // ライトサンプルのみ
            //const int32_t numLightSample = light->sampleNum();
            const int32_t numLightSample = 16; // HACK: 固定
            Spectrum weightResult(0.0f);
            for (int32_t sampleNo = 0; sampleNo < numLightSample; ++sampleNo)
            {
                // ライトのサンプリング
                const auto lightSampling = [](
                    SamplerPtr sampler,
                    LightPtr light,
                    BSDFPtr bsdf,
                    Vec3 isectPos,
                    const OrthonormalBasis<>& local,
                    const Vec3 localWo,
                    const SceneGeometory& scene,
                    bool isUseMIS )
                {
                    // ライト上のサンプル
                    Vec3 lightSamplePos;
                    float pdfLight;
                    const Spectrum emittion =
                        light->sampleLe(
                            sampler,
                            isectPos,
                            &lightSamplePos,
                            &pdfLight);

                    // TODO: ガラス面を対応する
                    // 可視であれば、そのライトのSpectrumを足し上げる
                    const Vec3 p0 = isectPos;
                    const Vec3 p1 = lightSamplePos;
                    // NOTE: 本当はライトも含めて行わないといけない(ライトの前にライトがある可能性がある)
                    const bool skipLight = true;
                    const bool isVisible = scene.isVisible(p0, p1, skipLight);
                    if (!isVisible)
                    {
                        return Spectrum::createAsBlack();
                    }
                    
                    // reflectanceの算出
                    const Vec3 toLight = lightSamplePos - isectPos;
                    const Vec3 worldWi = toLight.normalized();
                    const Vec3 localWi = local.world2local(worldWi);
                    const Spectrum reflectance = bsdf->bsdf(localWo, localWi);
                    float weight = 1.0f;
                    if (isUseMIS)
                    {
                        const float pdfBSDF = bsdf->pdf(localWo, localWi);
                        weight = misBalanceHeuristic(pdfLight, pdfBSDF);
                    }
                    else
                    {
                        weight = 1.0f;
                    }
                    //
                    // 最終的なspectrum値の算出
                    const Spectrum spectrum = reflectance * emittion * std::fabsf(localWi.z) * weight / pdfLight;
                    //const Spectrum spectrum = Spectrum::createFromRGB({ weight ,0.0f, 0.0f }, false);
                    AL_ASSERT_DEBUG(!spectrum.hasNaN());
                    return spectrum;
                };

                // BSDFのサンプリング
                const auto bsdfSampling = [](
                    SamplerPtr sampler,
                    LightPtr light,
                    BSDFPtr bsdf,
                    Vec3 isectPos,
                    const OrthonormalBasis<>& local,
                    const Vec3 localWo,
                    const SceneGeometory& scene,
                    bool isUseMIS)
                {
                    Vec3 localWi;
                    float pdfBSDF;
                    const Spectrum f = bsdf->bsdfSample(localWo, sampler, &localWi, &pdfBSDF);
                    const Vec3 worldWi = local.local2world(localWi);
                    const float pdfLight = light->pdf(isectPos, worldWi);
                    float weight = 1.0f;
                    if (isUseMIS)
                    {
                        weight = misBalanceHeuristic(pdfBSDF, pdfLight);
                    }
                    else
                    {
                        weight = 1.0f;
                    }
                    const Spectrum L = light->emittion(isectPos, worldWi);
                    //
                    if (pdfBSDF == 0.0f || L.isBlack())
                    {
                        return Spectrum::createAsBlack();
                    }
                    //
                    const Vec3 p0 = isectPos;
                    const Vec3 p1 = p0 + worldWi * 100000.0f;
                    // NOTE: 本当はライトも含めて行わないといけない(ライトの前にライトがある可能性がある)
                    const bool skipLight = true;
                    const bool visible = scene.isVisible(p0, p1, skipLight);
                    if (!visible)
                    {
                        return Spectrum::createAsBlack();
                    }
                    const Spectrum spectrum = f * L * absCosTheta(localWo) * weight / pdfBSDF;
                    //const Spectrum spectrum = Spectrum::createFromRGB({ 0.0f, weight, 0.0f }, false);
                    AL_ASSERT_DEBUG(!spectrum.hasNaN());
                    return spectrum;
                };

                // MISの選択
                // TODO: 前の方にあるSampleStrategyとまとめて扱えるようにする。
                enum class LightSampleStrategy
                {
                    LightAndBSDF,
                    OnlyBSDFSampling,
                    OnlyLightSampling,
                };
                LightSampleStrategy lightSampleStrategy = LightSampleStrategy::LightAndBSDF;
                //LightSampleStrategy lightSampleStrategy = LightSampleStrategy::OnlyLightSampling;
                //LightSampleStrategy lightSampleStrategy = LightSampleStrategy::OnlyBSDFSampling;
                
                switch (lightSampleStrategy)
                {
                case LightSampleStrategy::LightAndBSDF:
                    spectrum += lightSampling(sampler, light, bsdf, isect.uppserSideOrigin(), local, localWo, scene, true);
                    spectrum += bsdfSampling(sampler, light, bsdf, isect.uppserSideOrigin(), local, localWo, scene, true);
                    break;
                case LightSampleStrategy::OnlyLightSampling:
                    spectrum += lightSampling(sampler, light, bsdf, isect.uppserSideOrigin(), local, localWo, scene, false);
                    break;
                case LightSampleStrategy::OnlyBSDFSampling:
                    spectrum += bsdfSampling(sampler, light, bsdf, isect.uppserSideOrigin(), local, localWo, scene, false);
                    break;
                default:
                    AL_ASSERT_ALWAYS(false);
                    break;
                }
            }
            AL_ASSERT_DEBUG(!spectrum.hasNaN());
            return spectrum / float(numLightSample);
        }
        break;
    default:
        AL_ASSERT_ALWAYS(false);
        return Spectrum::createAsBlack();
        break;
    }
}
