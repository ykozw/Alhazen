#include "catch2/catch.hpp"
#include "bsdf/bsdf.hpp"
#include "core/floatstreamstats.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
static void testBSDFcore(BSDFPtr bsdf)
{
    //
    SamplerIndepent samplerWo;
    SamplerHalton samplerWi;
    samplerWo.setHash(0x01);
    samplerWi.setHash(0x01);
    //
    for (int32_t i = 0; i < 128; ++i)
    {
        //
        FloatStreamStats<> statsEnergyConservation;
        FloatStreamStats<> statsPdfTotal;
        FloatStreamStats<> statsRhd0;
        FloatStreamStats<> statsRhd1;
        //
        samplerWo.startSample(i);
        float pdf;
        const Vec3 wo = samplerWo.getHemisphere(&pdf);
        //
        for (int32_t sn = 0; sn < 1024 * 16; ++sn)
        {
            samplerWi.startSample(sn);
            float pdfSphere;
            const Vec3 wi = samplerWi.getHemisphere(&pdfSphere);
            const Spectrum ref0 = bsdf->bsdf(wo, wi);
            const Spectrum ref1 = bsdf->bsdf(wi, wo);
            // ヘルムホルツの相反性
            const auto same = [](const Spectrum& lhs, const Spectrum& rhs) {
                const Spectrum d = lhs - rhs;
                const float eps = 0.0001f;
                return (fabsf(d.r) < eps) && (fabsf(d.g) < eps) &&
                    (fabsf(d.b) < eps);
            };
            AL_ASSERT_ALWAYS(same(ref0, ref1));
            //
            const float ref = ref0.r;
            // 負になっていないかチェック
            AL_ASSERT_ALWAYS(ref >= 0.0f);
            // 総エネルギー
            const float e = ref * std::fabsf(wi.z()) * 2.0f * PI;
            statsEnergyConservation.add(e);
            // ∫pdfdΩ = 1
            const float pdf = bsdf->pdf(wo, wi);
            const float contribution = pdf * 2.0f * PI;
            statsPdfTotal.add(contribution);
            // rhdの算出(Bruteforce)
            statsRhd0.add(e);
            // rhdの算出(ImportanceSampling)
            Vec3 wiIS;
            float pdfIS;
            const Spectrum refIS =
                bsdf->bsdfSample(wo, &samplerWi, &wiIS, &pdfIS);
            if (pdfIS != 0.0f)
            {
                statsRhd1.add(refIS.r * std::fabsf(wiIS.z()) / pdfIS);
            }
        }
        AL_ASSERT_ALWAYS(statsEnergyConservation.mu() <= 1.0f);
        AL_ASSERT_ALWAYS(std::fabsf(statsPdfTotal.mu() - 1.0f) < 0.01f);
        AL_ASSERT_ALWAYS(
            std::fabsf(statsRhd0.mu() / statsRhd1.mu() - 1.0f) < 0.01f);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("BlinnNDF", "BSDF")
{
    // TODO: テストが止まってしまっている
    return;
    //
    SamplerIndepent samplerWo;
    SamplerHalton samplerWi;
    SamplerHalton samplerWh;
    samplerWo.setHash(0x01);
    samplerWi.setHash(0x02);
    samplerWh.setHash(0x03);
    MicrofacetDistributionPtr nd = std::make_shared<BlinnNDF>(0.0f);

    //// TODO: pdf()が全周で1になっているかのチェック
    // for (int32_t sn=0;sn<1024;++sn)
    //{
    //    const Vec3 wh = samplerWh->getSphere();
    //    const float pdf = nd->pdf();
    // pdfとevalの違いがわからない。

    //}

    //
    for (int32_t i = 0; i < 128; ++i)
    {
        //
        FloatStreamStats<> statsEnergyConservation;
        FloatStreamStats<> statsPdfTotal;
        FloatStreamStats<> statsRhd0;
        FloatStreamStats<> statsRhd1;
        //
        samplerWo.startSample(i);
        float pdfSphere;
        const Vec3 wo = samplerWo.getSphere(&pdfSphere);
        //
        for (int32_t sn = 0; sn < 1024; ++sn)
        {
            samplerWi.startSample(sn);
            float pdfSphere;
            const Vec3 wi = samplerWi.getSphere(&pdfSphere);
            // HalfVectorの作成
            const Vec3 wh = (wi + wo).normalized();
            const float ref = nd->eval(wh);
            // 負になっていないかチェック
            AL_ASSERT_ALWAYS(ref >= 0.0f);
            // 総エネルギー
            const float e = ref * std::fabsf(wi.z()) * 2.0f * PI;
            statsEnergyConservation.add(e);
            // ∫pdfdΩ = 1
            const float pdf = nd->pdf(wo, wi);
            const float contribution = pdf * 4.0f * PI; // 全周なので4PI
            statsPdfTotal.add(contribution);
            // rhdの算出(Bruteforce)
            statsRhd0.add(e);
            // rhdの算出(ImportanceSampling)
            Vec3 wiIS;
            float pdfIS;
            nd->sample(wo, &samplerWi, &wiIS, &pdfIS);
            // statsRhd1.add(refIS.r * std::fabsf(wiIS.z) / pdfIS);
        }
        AL_ASSERT_ALWAYS(statsEnergyConservation.mu() <= 1.0f);
        AL_ASSERT_ALWAYS(std::fabsf(statsPdfTotal.mu() - 1.0f) < 0.01f);
        AL_ASSERT_ALWAYS(
            std::fabsf(statsRhd0.mu() / statsRhd1.mu() - 1.0f) < 0.01f);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("Lambertian", "BSDF")
{
    return;
    testBSDFcore(std::make_shared<Lambertian>(Spectrum(1.0f)));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("OrenNayar", "BSDF")
{
    return;

    testBSDFcore(std::make_shared<OrenNayar>(
        Spectrum::createFromRGB({ { 1.0f, 1.0f, 1.0f } }, false), 0.5f));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("testMISC", "BSDF")
{
    /*
    チェックしないといけない項目
    - cosine weightは最初からかかってる前提のBSDF? @pbrt と他のレンダラー
    - LightとLambertianの反射率を見てみる
    */
    //つぎはLightとLambertianの反射率を見てみる
    //
    SamplerIndepent sampler;
    // IndependSamplerHemisphere hemiSampler(123456789, true);
    const int32_t sn = 1024 * 16;
#if 0
    float ttt = 0.0f;
    for (int32_t i = 0; i<sn; ++i)
    {
        const Vec3 v = hemiSampler.sample(0);
        ttt += v.z;
    }
    ttt /= float(sn);
    printf("A: %f\n", ttt); // not cos 0.5。cosのときは0.66
#endif

    Lambertian lambert(Spectrum::createFromRGB({ { 1.0f, 1.0f, 1.0f } }, false));
    // const Vec3 view = Vec3(1.0f,1.0f,1.0f).normalized();
    Spectrum total;
    sampler.setHash(1);
    for (int32_t i = 0; i < sn; ++i)
    {
        sampler.startSample(i);
        float pdfWo, pdfWi;
        const Vec3 wo = sampler.getHemisphere(&pdfWo);
        const Vec3 wi = sampler.getHemisphere(&pdfWi);
        const Spectrum reflectance = lambert.bsdf(wo, wi);
        // const float cosweight = wi.z();
        total = total + reflectance;
    }
    total /= float(sn);
    // printf("B: %f\n", total.r); // cosine weight on/off関係なく
    // 1/PI(0.318)に収束する

    // bsdfSample()とbsdf()に違いが派生していないかチェック
    // const Spectrum rho0 = lambert.estimateTotalRefrectance(128 * 128, false);
    // const Spectrum rho1 = lambert.estimateTotalRefrectance(128 * 128, true);
    // printf("C: %f %f\n", rho0.r, rho1.r);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("MicrofacetBSDF", "BSDF")
{
    return;

    FresnelTermPtr f = std::make_shared<FresnelNone>();
    MicrofacetDistributionPtr nd = std::make_shared<BlinnNDF>(1.0f);
    BSDFPtr bsdf = std::make_shared<MicrofacetBSDF>(Spectrum(1.0f), f, nd);
    testBSDFcore(bsdf);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("AutoImportance", "BSDF")
{
#if 0
    AutoImportance ai;
    BSDFPtr bsdf = createObject<BSDF>("disney");
    //BSDFPtr bsdf = createObjectOld<BSDF>("diffuse", ObjectProp());
    ai.setBRDF(bsdf.get());
#endif
}
