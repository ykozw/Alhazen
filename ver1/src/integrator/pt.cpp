#include "integrator.hpp"
#include "bsdf/bsdf.hpp"
#include "core/orthonormalbasis.hpp"
#include "core/stats.hpp"
#include "core/object.hpp"

//
STATS_COUNTER("EvDirect", g_numEstimateOneLine, "Evals");

/*
-------------------------------------------------
ライトの影響球管理
-------------------------------------------------
*/
class LightSphereBVH
{
public:
    // ライトのBVHの構築
    void construct(
        const std::vector<LightPtr>& lights,
        Sampler* sampler)
    {
        logging("LightSphereBVH::construct() start");
        // BVHを作成する
        bvh_.construct(int32_t(lights.size()), [&](int32_t lightIdx)
        {
            // HACK: このalphaは決め打ち
            const float alpha = 0.05f;
            const float xi = sampler->get1d();
            const float dist0 = std::sqrtf(1.0f / alpha);
            const float dist1 = std::sqrtf(1.0f / xi);
            //
            auto& light = lights[lightIdx];
            const float radius = std::max(dist0,dist1);
            const Vec3 lightCenter = light->aabb().center();
            AABB aabb;
            aabb.addSphere(lightCenter, radius);
            return aabb;
        });
        logging("LightSphereBVH::construct() end");
    }
    //
    int32_t selectLight(Vec3 point, const std::vector<LightPtr>& lights, Sampler* sampler, float* pdf) const
    {
        class Local
        {
        public:
            static void enumerateVolume(const BVHBuilder& bvh, int32_t nodeIndex, Vec3 point, const std::function<void(int32_t)>& onFindVolume)
            {
                // その点を含む一番下のボリュームまで下りていく
                auto& node = bvh.nodes_[nodeIndex];
                if (!node.aabb.isInside(point))
                {
                    return;
                }
                // 葉であった場合
                if (node.isLeaf())
                {
                    onFindVolume(node.index);
                    return;
                }
                // 節であった場合
                else
                {
                    enumerateVolume(bvh, node.childlen[0], point, onFindVolume);
                    enumerateVolume(bvh, node.childlen[1], point, onFindVolume);
                }
            }
        };
        //
        struct LightInfo
        {
            int32_t lightIdx = -1;
            float pdf = 0.0f;
            float cdf = 0.0f;
        };
        float cdfTotal = 0.0f;
        int32_t lightInfosLen = 0;
        std::array<LightInfo, 1024> lightInfos;

        // 全てのボリュームの採用確率を計算する
        Local::enumerateVolume(bvh_, 0, point, [&](int32_t volumeIndex)
        {
            // HACK: バッファが溢れたら丸々無視
            if (lightInfos.size() <= lightInfosLen)
            {
                return;
            }
            // HACK: とりあえずボリュームに含まれていたら採用確立は全て同じに。
            const float dist = Vec3::distance(point, lights[volumeIndex]->aabb().center());
            const float pdf = 1.0f/ dist;
            cdfTotal += pdf;
            lightInfos[lightInfosLen] = { volumeIndex, pdf, cdfTotal };
            ++lightInfosLen;
        });
        const float invCdfTotal = 1.0f / cdfTotal;
        for(int32_t li=0;li<lightInfosLen;++li)
        {
            auto& lightInfo = lightInfos[li];
            lightInfo.pdf *= invCdfTotal;
            lightInfo.cdf *= invCdfTotal;
        }

        // 実際に採用する
        const float cdf = sampler->get1d();
        const auto ite =
            std::lower_bound(lightInfos.begin(),
                             lightInfos.begin()+lightInfosLen,
                             cdf,
                             [](const LightInfo& lightInfo, float value) {
                                 return (lightInfo.cdf < value);
                             });
        if (ite == lightInfos.end())
        {
            return -1;
        }
        *pdf = ite->pdf;
        return ite->lightIdx;
    }

private:
    BVHBuilder bvh_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class PTSurfaceIntegrator AL_FINAL : public LTEIntegrator
{
public:
    PTSurfaceIntegrator() = default;
    PTSurfaceIntegrator(const ObjectProp& objectProp);
    bool preRendering(const SceneGeom& scene) override;
    bool postRendering() override { return true; }
    //
    Spectrum radiance(const Ray& ray,
                      const SceneGeom& scene,
                      Sampler* sampler) const override;

private:
    Spectrum estimateDirectLight(const SceneGeom& scene,
                                 const Intersect& isect,
                                 const OrthonormalBasis<>& local,
                                 const Vec3 localWo,
                                 Sampler* sampler) const;
    Spectrum estimateOneLight(const SceneGeom& scene,
                              const Intersect& isect,
                              const OrthonormalBasis<>& local,
                              const Vec3 localWo,
                              const Light* light,
                              Sampler* samler) const;

    //
    void onStartFrame(const SceneGeom& scene, int32_t frameNo) override;

private:
    // 直接光のライトの選択の戦略
    enum class DirectLighitingSelectStrategy
    {
        // 毎回全てのライトをサンプル
        All,
        // 等確率で一つを選択する
        UniformOne,
        // 距離の二乗の逆数で重みづけして一つ選択する
        SelectDistSqr,
        // 確率的カリング
        StocasticCulling
    };
    DirectLighitingSelectStrategy directLighitingLightSelectStrategy_ =
        //DirectLighitingSelectStrategy::All;
        //DirectLighitingSelectStrategy::UniformOne;
        //DirectLighitingSelectStrategy::SelectDistSqr;
        DirectLighitingSelectStrategy::StocasticCulling;
        
    //
    LightSphereBVH lightSphereBVH_;
};

REGISTER_OBJECT(LTEIntegrator, PTSurfaceIntegrator);

/*
-------------------------------------------------
-------------------------------------------------
*/
PTSurfaceIntegrator::PTSurfaceIntegrator(const ObjectProp& objectProp) {}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool PTSurfaceIntegrator::preRendering(const SceneGeom& scene)
{
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void PTSurfaceIntegrator::onStartFrame(const SceneGeom& scene, int32_t frameNo)
{
    // lightSphereBVHの構築
    SamplerHalton sampler;
    sampler.setHash(0x12345);
    sampler.startSample(frameNo);
    lightSphereBVH_.construct(scene.lights(), &sampler);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum PTSurfaceIntegrator::radiance(const Ray& screenRay,
                                       const SceneGeom& scene,
                                       Sampler* sampler) const
{
    //
    Spectrum throughput = Spectrum(1.0f);
    Spectrum lighting = Spectrum(0.0f);
    Ray ray = screenRay;
    const int32_t maxpathNo = 8;
    // bool isOutside = true;
    // const float rrProb = sampler->get1d();
    //
    for (int32_t pathNo = 0; pathNo < maxpathNo; ++pathNo)
    {
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
        const bool skipLight = false; (pathNo == 0) ? false : true;
        if (!scene.intersect(ray, skipLight, &isect))
        {
            break;
        }
        // 交差でのemissionの反映
        lighting += isect.emission;
        // 無限遠と交差した場合は終了
        if (isect.isHitWithInf())
        {
            break;
        }
        //
        BSDF* bsdf = isect.bsdf;
        float pdfBSDF;
        Vec3 localWi;
        const OrthonormalBasis<> local(isect.normal);
        const Vec3 localWo = local.world2local(-ray.d);
        // 直接光の推定
        const Spectrum directLighting =
            estimateDirectLight(scene, isect, local, localWo, sampler);
        AL_ASSERT_DEBUG(!directLighting.hasNaN());
        lighting += directLighting * throughput;
        //
        AL_ASSERT_DEBUG(!lighting.hasNaN());
        // 反射率の算出
        const Spectrum reflectance =
            bsdf->bsdfSample(localWo, sampler, &localWi, &pdfBSDF);
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
        throughput = throughput * reflectance * fabsf(localWi.z()) / pdfBSDF *
                     INV_PI; // TODO:
                             // invpiはどうやって取り除くのか？pdf()の単位を変えればいいのか？
        AL_ASSERT_DEBUG(!throughput.hasNaN());
        // 次のrayを作成する
        const Vec3 rayOrig = (localWi.z() >= 0.0f) ? isect.uppserSideOrigin()
                                                   : isect.belowSideOrigin();
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
Spectrum
PTSurfaceIntegrator::estimateDirectLight(const SceneGeom& scene,
                                         const Intersect& isect,
                                         const OrthonormalBasis<>& local,
                                         const Vec3 localWo,
                                         Sampler* sampler) const
{
    const auto& lights = scene.lights();
    if (lights.empty())
    {
        return Spectrum::Black;
    }
    //
    switch (directLighitingLightSelectStrategy_)
    {
    // 全てのライトを評価
    case DirectLighitingSelectStrategy::All:
    {
        //printf("AL");
        Spectrum estimated;
        for (const auto& light : lights)
        {
            estimated += estimateOneLight(
                scene, isect, local, localWo, light.get(), sampler);
        }
        return estimated;
    }
    break;
    // ライトを一つだけ選択
    case DirectLighitingSelectStrategy::UniformOne:
    {
        // ライトを一つ選択する
        const uint32_t lightIndex = sampler->getSize(uint32_t(lights.size()));
        const auto& choochenLight = lights[lightIndex];
        /*
        交差点のSceneObjectとサンプルするLightが同一だった場合は終了
        HACK:
        本当はライト自体がそのライトに照らされる場合があるが、省略してしまう
        */
        if (isect.sceneObject == choochenLight.get())
        {
            return Spectrum::Black;
        }
        //
        const Spectrum oneLightEstimated = estimateOneLight(
            scene, isect, local, localWo, choochenLight.get(), sampler);
        return oneLightEstimated * (float)lights.size();
    }
    break;
    // ライトを距離で重みづけ
    case DirectLighitingSelectStrategy::SelectDistSqr:
    {
        //
        struct LightInfo
        {
            int32_t lightIdx = 0;
            float pdf = 0.0f;
            float cdf = 0.0f;
        };
        float cdfTotal = 0.0f;
        int32_t lightInfosLen = 0;
        std::array<LightInfo, 1024> lightInfos;

        for (int32_t lightIndex = 0; lightIndex < int32_t(lights.size()) ;++lightIndex)
        {
            auto& light = lights[lightIndex];
            // HACK: バッファが溢れたら丸々無視
            if (lightInfos.size() <= lightInfosLen)
            {
                break;
            }
            // HACK: とりあえずボリュームに含まれていたら採用確立は全て同じに。
            const float dist = Vec3::distance(isect.position, light->aabb().center());
            const float pdf = 1.0f / dist;
            cdfTotal += pdf;
            lightInfos[lightInfosLen] = { lightIndex, pdf, cdfTotal };
            ++lightInfosLen;
        }
        const float invCdfTotal = 1.0f / cdfTotal;
        for (auto& lightInfo : lightInfos)
        {
            lightInfo.pdf *= invCdfTotal;
            lightInfo.cdf *= invCdfTotal;
        }
        //
        const float cdf = sampler->get1d();
        const auto ite =
            std::lower_bound(lightInfos.begin(),
                lightInfos.begin() + lightInfosLen,
                cdf,
                [](const LightInfo& lightInfo, float value) {
            return (lightInfo.cdf < value);
        });
        //
        const auto& choochenLight = lights[ite->lightIdx];
        /*
        交差点のSceneObjectとサンプルするLightが同一だった場合は終了
        HACK:
        本当はライト自体がそのライトに照らされる場合があるが、省略してしまう
        */
        if (isect.sceneObject == choochenLight.get())
        {
            return Spectrum::Black;
        }
        //
        const Spectrum oneLightEstimated = estimateOneLight(
            scene, isect, local, localWo, choochenLight.get(), sampler);
        return oneLightEstimated / ite->pdf;
    }
    break;
    case DirectLighitingSelectStrategy::StocasticCulling:
    {
        float lightSelectPdf = 0.0f;
        const int32_t lightIndex = lightSphereBVH_.selectLight(isect.position, lights, sampler, &lightSelectPdf);
        if (lightIndex == -1)
        {
            return Spectrum::Black;
        }
        const auto& choochenLight = lights[lightIndex];
        /*
        交差点のSceneObjectとサンプルするLightが同一だった場合は終了
        HACK:
        本当はライト自体がそのライトに照らされる場合があるが、省略してしまう
        */
        if (isect.sceneObject == choochenLight.get())
        {
            return Spectrum::Black;
        }
        //
        const Spectrum oneLightEstimated = estimateOneLight(
            scene, isect, local, localWo, choochenLight.get(), sampler);
        return oneLightEstimated / lightSelectPdf;
    }
    break;
    default:
        AL_ASSERT_ALWAYS(false);
        return Spectrum::Black;
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

#if 0
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
#endif

/*
-------------------------------------------------
指定したisect/localWiでの指定されたライトの直接光のContributionを推定
-------------------------------------------------
*/
Spectrum PTSurfaceIntegrator::estimateOneLight(const SceneGeom& scene,
                                               const Intersect& isect,
                                               const OrthonormalBasis<>& local,
                                               const Vec3 localWo,
                                               const Light* light,
                                               Sampler* sampler) const
{
    ++g_numEstimateOneLine;
    //
    BSDF* bsdf = isect.bsdf;
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
    const auto takeStragety = [](const BSDF* bsdf,
                                 const Light* light) -> SampleStrategy {
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

    // ライトのサンプリング
    const auto lightSampling = [](Sampler* sampler,
                                  const Light* light,
                                  BSDF* bsdf,
                                  Vec3 isectPos,
                                  const OrthonormalBasis<>& onb,
                                  const Vec3 localWo,
                                  const SceneGeom& scene,
                                  bool isUseMIS) {
        // ライト上のサンプル
        Vec3 lightSamplePos;
        float pdfLight;
        const Spectrum emission =
            light->sampleLe(sampler, isectPos, &lightSamplePos, &pdfLight);

        // TODO: ガラス面を対応する
        // 可視であれば、そのライトのSpectrumを足し上げる
        const Vec3 p0 = isectPos;
        const Vec3 p1 = lightSamplePos;
        // NOTE:
        // 本当はライトも含めて行わないといけない(ライトの前にライトがある可能性がある)
        const bool skipLight = true;
        const bool isVisible = scene.isVisible(p0, p1, skipLight);
        if (!isVisible)
        {
            return Spectrum::Black;
        }

        // reflectanceの算出
        const Vec3 toLight = lightSamplePos - isectPos;
        const Vec3 worldWi = toLight.normalized();
        const Vec3 localWi = onb.world2local(worldWi);
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
        const Spectrum spectrum = reflectance * emission *
                                  std::fabsf(localWi.z()) * weight / pdfLight;
        // const Spectrum spectrum = Spectrum::createFromRGB({ weight ,0.0f,
        // 0.0f }, false);
        AL_ASSERT_DEBUG(!spectrum.hasNaN());
        return spectrum;
    };
    // BSDFのサンプリング
    const auto bsdfSampling = [](Sampler* sampler,
                                 const Light* light,
                                 BSDF* bsdf,
                                 Vec3 isectPos,
                                 const OrthonormalBasis<>& local,
                                 const Vec3 localWo,
                                 const SceneGeom& scene,
                                 bool isUseMIS) {
        Vec3 localWi;
        float pdfBSDF;
        const Spectrum f =
            bsdf->bsdfSample(localWo, sampler, &localWi, &pdfBSDF);
        const Vec3 worldWi = local.local2world(localWi);
        float weight = 1.0f;
        if (isUseMIS)
        {
            const float pdfLight = light->pdf(isectPos, worldWi);
            weight = misBalanceHeuristic(pdfBSDF, pdfLight);
        }
        else
        {
            weight = 1.0f;
        }
        const Spectrum L = light->emission(isectPos, worldWi);
        //
        if (pdfBSDF == 0.0f || L.isBlack())
        {
            return Spectrum::Black;
        }
        //
        const Vec3 p0 = isectPos;
        const Vec3 p1 = p0 + worldWi * 100000.0f;
        // NOTE:
        // 本当はライトも含めて行わないといけない(ライトの前にライトがある可能性がある)
        const bool skipLight = true;
        const bool visible = scene.isVisible(p0, p1, skipLight);
        if (!visible)
        {
            return Spectrum::Black;
        }
        const Spectrum spectrum =
            f * L * absCosTheta(localWo) * weight / pdfBSDF;
        // const Spectrum spectrum = Spectrum::createFromRGB({ 0.0f, weight,
        // 0.0f }, false);
        AL_ASSERT_DEBUG(!spectrum.hasNaN());
        return spectrum;
    };
    //
    switch (sampleStrategy)
    {
    case SampleStrategy::AlwaysZero:
        return Spectrum::Black;
    case SampleStrategy::OnlyLightSample:
    {
        Spectrum spectrum(0.0f);
        //
        for (int32_t i = 0; i < 4; ++i)
        {
            // 向かう先の算出
            Vec3 localWi;
            float pdf;
            const Spectrum reflectance =
                bsdf->bsdfSample(localWo, sampler, &localWi, &pdf);
            const Vec3 worldWi = local.local2world(localWi);
            // レイ指定でのEmittionの取得
            // TODO: あれ、これは方向が逆になってない？
            const Spectrum emission =
                light->emission(isect.uppserSideOrigin(), worldWi);
            const float cosTerm = fabsf(localWi.z());
            spectrum += reflectance * emission * 4.0f * cosTerm / pdf;
        }
        return spectrum;
    }
    break;
    case SampleStrategy::OnlyBSDFSample:
    {
        Spectrum spectrum(0.0f);
        spectrum += bsdfSampling(sampler,
                                 light,
                                 bsdf,
                                 isect.uppserSideOrigin(),
                                 local,
                                 localWo,
                                 scene,
                                 false);
        return spectrum;
    }
    break;
    case SampleStrategy::MISSample:
    {
        Spectrum spectrum(0.0f);

        // ライトサンプルのみ
        // const int32_t numLightSample = light->sampleNum();
        const int32_t numLightSample = 1; // HACK: 固定
        Spectrum weightResult(0.0f);
        for (int32_t sampleNo = 0; sampleNo < numLightSample; ++sampleNo)
        {
            // MISの選択
            // TODO: 前の方にあるSampleStrategyとまとめて扱えるようにする。
            enum class LightSampleStrategy
            {
                LightAndBSDF,
                OnlyBSDFSampling,
                OnlyLightSampling,
            };
            LightSampleStrategy lightSampleStrategy =
                LightSampleStrategy::LightAndBSDF;
            // LightSampleStrategy lightSampleStrategy =
            // LightSampleStrategy::OnlyLightSampling; LightSampleStrategy
            // lightSampleStrategy = LightSampleStrategy::OnlyBSDFSampling;

            switch (lightSampleStrategy)
            {
            case LightSampleStrategy::LightAndBSDF:
                spectrum += lightSampling(sampler,
                                          light,
                                          bsdf,
                                          isect.uppserSideOrigin(),
                                          local,
                                          localWo,
                                          scene,
                                          true);
                spectrum += bsdfSampling(sampler,
                                         light,
                                         bsdf,
                                         isect.uppserSideOrigin(),
                                         local,
                                         localWo,
                                         scene,
                                         true);
                break;
            case LightSampleStrategy::OnlyLightSampling:
                spectrum += lightSampling(sampler,
                                          light,
                                          bsdf,
                                          isect.uppserSideOrigin(),
                                          local,
                                          localWo,
                                          scene,
                                          false);
                break;
            case LightSampleStrategy::OnlyBSDFSampling:
                spectrum += bsdfSampling(sampler,
                                         light,
                                         bsdf,
                                         isect.uppserSideOrigin(),
                                         local,
                                         localWo,
                                         scene,
                                         false);
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
        return Spectrum::Black;
        break;
    }
}
