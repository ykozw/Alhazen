#include "core/object.hpp"
#include "bsdf/bsdf.hpp"
#include "core/floatstreamstats.hpp"
#include "core/stats.hpp"
#include "sampler/sampler.hpp"

REGISTER_OBJECT(BSDF, Lambertian);
REGISTER_OBJECT(BSDF, Glass);
REGISTER_OBJECT(BSDF, MicrofacetBSDF);

BSDFPtr BSDF::vantaBlack = std::make_shared<Lambertian>(Spectrum::Black);
BSDFPtr BSDF::gray18 = std::make_shared<Lambertian>(
    Spectrum::createFromRGB({{0.18f, 0.18f, 0.18f}}, false));
BSDFPtr BSDF::gray50 = std::make_shared<Lambertian>(
    Spectrum::createFromRGB({{0.50f, 0.50f, 0.50f}}, false));
BSDFPtr BSDF::white = std::make_shared<Lambertian>(
    Spectrum::createFromRGB({{1.00f, 1.00f, 1.00f}}, false));

//
STATS_COUNTER("EvBsdfLamb", g_numEvalBsdfLambert, "Evals");
STATS_COUNTER("EvPdfLamb", g_numEvalPdfLambert, "Evals");
STATS_COUNTER("EvSampLamb", g_numSampleLambert, "Evals");


/*
-------------------------------------------------
-------------------------------------------------
*/
static float sqr(float x) { return x * x; }

/*
-------------------------------------------------
BSDF
-------------------------------------------------
*/
BSDF::BSDF(){}

/*
-------------------------------------------------
BSDF
-------------------------------------------------
*/
BSDF::BSDF(const ObjectProp& objectProp)
{
    name_ = objectProp.attribute("type").asString("unkown");
    id_ = objectProp.attribute("id").asString("unkown");
}

/*
-------------------------------------------------
TotalRefrectanceの推定
-------------------------------------------------
*/
Spectrum BSDF::estimateTotalRefrectance(const int32_t sampleNum,
                                        bool dontUseBsdfSample) const
{
#if 0
    const Vec2 dummyUV(0.0f, 0.0f);
    Spectrum ret(0.0f);
    XorShift128 rng;
    SamplerIndepent sampler;
    if (!dontUseBsdfSample)
    {
        for (int32_t i = 0; i < sampleNum; ++i)
        {
            const Vec3 wo = sampler.getHemisphere();
            const float pdfwo = 1.0f / (2.0f * PI);
            float pdfwi;
            Vec3 wi;
            Spectrum reflectance = bsdfSample(wo, dummyUV, rng, &wi, &pdfwi);
            if (pdfwi > 0.0f)
            {
                ret += reflectance * absCosTheta(wi) * absCosTheta(wo) / (pdfwo * pdfwi);
            }
        }
        return ret / (PI * float(sampleNum));
    }
    else
    {
        for (int32_t i = 0; i < sampleNum; ++i)
        {
            const Vec3 wo = remapSquareToHemisphere(Vec2(rng.nextFloat(), rng.nextFloat()));
            const Vec3 wi = remapSquareToHemisphere(Vec2(rng.nextFloat(), rng.nextFloat()));
            const float pdfwo = 1.0f / (2.0f * PI);
            const float pdfwi = 1.0f / (2.0f * PI);
            Spectrum reflectance = bsdf(wo, wi, dummyUV);
            ret += reflectance;
        }
        return ret / (float(sampleNum));
    }
#else
    return Spectrum::Black;
#endif
}

/*
-------------------------------------------------
方向付きのRefrectanceの推定
-------------------------------------------------
*/
Spectrum BSDF::estimateDirectionalRefrectance(Vec3 wo,
                                              int32_t sampleNum,
                                              bool dontUseBsdfSample) const
{
#if 0
    const Vec2 dummyUV(0.0f, 0.0f);
    XorShift128 rng;
    Spectrum ret(0.0f);
    //
    if (!dontUseBsdfSample)
    {
        for (int32_t i = 0; i < sampleNum; ++i)
        {
            Vec3 wi;
            float pdf = 0.0f;
            const Spectrum reflectance = bsdfSample(wo, dummyUV, rng, &wi, &pdf);
            if (pdf > 0.0f)
            {
                ret += reflectance * absCosTheta(wi) / pdf;
            }
        }
        return ret / float(sampleNum);
    }
    else
    {
        for (int32_t i = 0; i < sampleNum; ++i)
        {
            const Vec3 wi = remapSquareToHemisphere(Vec2(rng.nextFloat(), rng.nextFloat()));
            const Spectrum reflectance = bsdf(wo, wi, dummyUV);
            ret += reflectance;
        }
        return ret / float(sampleNum);
    }
#else
    AL_ASSERT_ALWAYS(false);
    return Spectrum(0.0f);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
BlinnNDF::BlinnNDF(float e) : e_(e) {}

/*
-------------------------------------------------
-------------------------------------------------
*/
float BlinnNDF::eval(Vec3 wh)
{
    float act = absCosTheta(wh);
    return (e_ + 2.0f) * (INV_PI * 0.5f) * std::powf(act, e_);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float BlinnNDF::pdf(Vec3 wo, Vec3 wi)
{
    const Vec3 wh = (wo + wi).normalize();
    if (wh.hasNan())
    {
        return 0.0f;
    }
    const float acth = absCosTheta(wh);
    const float wodwh = Vec3::dot(wo, wh);
    if (wodwh <= 0.0f)
    {
        return 0.0f;
    }
    const float pdf = ((e_ + 1.0f) * powf(acth, e_)) / (8.0f * PI * wodwh);
    return pdf;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void BlinnNDF::sample(Vec3 wo, Sampler* sampler, Vec3* wi, float* pdf) const
{
    const float u1 = sampler->get1d();
    const float u2 = sampler->get1d();
    const float cosTheta = std::powf(u1, 1.0f / (e_ + 1.0f));
    AL_ASSERT_DEBUG(cosTheta <= 1.0f);
    const float sinTheta =
        std::sqrtf(std::max(0.0f, 1.0f - cosTheta * cosTheta));
    const float phi = u2 * 2.0f * PI;
    Vec3 wh(sinTheta * std::cosf(phi), sinTheta * std::sinf(phi), cosTheta);
    if (!sameHemisphere(wo, wh))
    {
        wh = -wh;
    }
    //
    *wi = -wh.reflect(wo);
    //
    const float wodwh = Vec3::dot(wo, wh);
    if (wodwh <= 0.0f)
    {
        *pdf = 0.0f;
        return;
    }
    *pdf = ((e_ + 1.0f) * powf(cosTheta, e_)) / (8.0f * PI * wodwh);

#if 0
    // Compute sampled half-angle vector $\wh$ for Blinn distribution
    float costheta = powf(u1, 1.f / (exponent + 1));
    float sintheta = sqrtf(max(0.f, 1.f - costheta*costheta));
    float phi = u2 * 2.f * M_PI;
    Vector wh = SphericalDirection(sintheta, costheta, phi);
    if (!SameHemisphere(wo, wh)) wh = -wh;

    // Compute incident direction by reflecting about $\wh$
    *wi = -wo + 2.f * Dot(wo, wh) * wh;

    // Compute PDF for $\wi$ from Blinn distribution
    float blinn_pdf = ((exponent + 1.f) * powf(costheta, exponent)) /
        (2.f * M_PI * 4.f * Dot(wo, wh));
    if (Dot(wo, wh) <= 0.f) blinn_pdf = 0.f;
    *pdf = blinn_pdf;
#endif
}


/*
-------------------------------------------------
-------------------------------------------------
*/
float GeometricTermDefault::eval(Vec3 wo, Vec3 wi, Vec3 wh) const
{
    const float ndwh = absCosTheta(wh);
    const float ndwo = absCosTheta(wo);
    const float ndwi = absCosTheta(wi);
    const float wodwh = fabsf(Vec3::dot(wi, wh));
    //
    const float whwo = (2.0f * ndwh * ndwo) / wodwh;
    const float whwi = (2.0f * ndwh * ndwi) / wodwh;
    const float r = alMin(2.0f, alMin(whwo, whwi));
    return r;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum FresnelNone::eval(float surfNormalDotIncomeDir) const
{
    // TODO: 怪しい
    static_cast<void>(surfNormalDotIncomeDir);
    return Spectrum(1.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
FresnelConductor::FresnelConductor(const Spectrum& eta, const Spectrum& k)
    : eta_(eta), k_(k)
{
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum FresnelConductor::eval(float surfNormalDotIncomeDir) const
{
    const float cosi = fabsf(surfNormalDotIncomeDir);
    const Spectrum tmp = (eta_ * eta_ + k_ * k_) * cosi * cosi;
    const Spectrum rparl2 = (tmp - (2.0f * eta_ * cosi) + Spectrum(1.0f)) /
                            (tmp + (2.0f * eta_ * cosi) + Spectrum(1.0f));
    const Spectrum tmp_f = eta_ * eta_ + k_ * k_;
    const Spectrum rperp2 =
        (tmp_f - (2.0f * eta_ * cosi) + Spectrum(cosi * cosi)) /
        (tmp_f + (2.f * eta_ * cosi) + Spectrum(cosi * cosi));
    return (rparl2 + rperp2) * 0.5f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE
FresnelDielectric::FresnelDielectric(float etai, float etat)
    : etai_(etai), etat_(etat)
{
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Spectrum FresnelDielectric::eval(float surfNormalDotIncomeDir) const
{
    const float cosi = surfNormalDotIncomeDir;
    AL_ASSERT_DEBUG(-1.0f <= cosi && cosi <= 1.0f);
    const bool isInside = cosi <= 0.0f;
    float ei = etai_;
    float et = etat_;
    if (isInside)
    {
        std::swap(ei, et);
    }
    // Snellの法則から入射方向を算出
    const float sint = ei / et * sqrtf(1.0f - cosi * cosi);
    // 完全反射の場合
    if (sint >= 1.0f)
    {
        // TODO: 怪しい
        return Spectrum(1.0f);
    }
    else
    {
        const float abscosi = fabsf(cosi);
        const float cost = sqrtf(1.0f - sint * sint);
        const float rparl =
            (et * abscosi - ei * cost) / (et * abscosi + ei * cost);
        const float rperp =
            (ei * abscosi - et * cost) / (ei * abscosi + et * cost);
        // TODO: 怪しい
        return Spectrum((rparl * rparl + rperp * rperp) * 0.5f);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Lambertian::Lambertian(const Spectrum& spectrum) { baseColor_ = spectrum; }

/*
-------------------------------------------------
-------------------------------------------------
*/
Lambertian::Lambertian(const ObjectProp& objectProp) : BSDF(objectProp)
{
    // HACK:
    // reflectance以外も持つ場合があるから"reflectance"で検索掛けるようにした方がいいかも。
    ObjectProp reflectanceProp = objectProp.findChildBy("name", "reflectance");
    //
    if (reflectanceProp.tag() == "")
    {
        baseColor_ = Spectrum::createFromRGB({{0.5f, 0.5f, 0.5f}}, false);
    }
    // rgbの場合
    else if (reflectanceProp.tag() == "rgb")
    {
        const auto spectrum = str2spectrum(
            reflectanceProp.attribute("value").asString("0.5 0.5 0.5"));
        baseColor_ = spectrum;
    }
    // srgbの場合
    else if (reflectanceProp.tag() == "srgb")
    {
        // TODO: 実装
        assert(false);
    }
    // テクスチャの場合
    else if (reflectanceProp.tag() == "texture")
    {
        // TODO: 実装
        assert(false);
    }
    // そのほかの場合はデフォルトで0.5の色
    else
    {
        baseColor_ = Spectrum::createFromRGB({{0.5f, 0.5f, 0.5f}}, false);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Lambertian::bsdf(Vec3 localWo, Vec3 localWi) const
{
    ++g_numEvalBsdfLambert;
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return Spectrum(0.0f);
    }
    return baseColor_ * INV_PI;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Lambertian::pdf(Vec3 localWo, Vec3 localWi) const
{
    ++g_numEvalPdfLambert;
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return 0.0f;
    }
    /*
    bsdfSample()でwiを半球上でcosine weight付きでサンプルしているので
    Integrate[Sin[\[Theta]]*Cos[\[Theta]], {\[Phi], 0, 2*Pi}, {\[Theta], 0,
    Pi/2}] = Pi
    */
    return absCosTheta(localWi) * INV_PI;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Lambertian::bsdfSample(Vec3 localWo,
                                Sampler* sampler,
                                Vec3* localWi,
                                float* pdf) const
{
    //
    ++g_numSampleLambert;
    //
    float pdfHS;
    *localWi = sampler->getHemisphereCosineWeighted(&pdfHS);
    // 裏面の場合
    if (localWo.z() < 0.)
    {
        localWi->setZ(localWi->z() * -1.0f);
    }
    //
    *pdf = this->pdf(localWo, *localWi);
    return bsdf(localWo, *localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
OrenNayar::OrenNayar(const Spectrum& baseColor, float sigma)
    : BSDF(ObjectProp()), baseColor_(baseColor)
{
    const float sigma2 = sigma * sigma;
    a_ = 1.0f - sigma2 / (2.0f * (sigma2 + 0.33f));
    b_ = (0.45f * sigma2) / (sigma2 + 0.09f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float OrenNayar::pdf(Vec3 localWo, Vec3 localWi) const
{
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return 0.0f;
    }
    return absCosTheta(localWi) * INV_PI;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum OrenNayar::bsdf(Vec3 localWo, Vec3 localWi) const
{
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return Spectrum(0.0f);
    }
    //
    float cosPhii, sinPhii;
    float cosPhio, sinPhio;
    sincosPhi(localWi, &sinPhii, &cosPhii);
    sincosPhi(localWo, &sinPhio, &cosPhio);
    const float cosPhiiPhio = cosPhii * cosPhio + sinPhii * sinPhio;
    const float cosPhiiPhioClamped = alMax(cosPhiiPhio, 0.0f);
    // θi < θoか？(cosの大小と逆になる)
    const bool isThetaiSmaller = cosTheta(localWi) > cosTheta(localWo);
    float sinalpha;
    float tanbeta;
    if (isThetaiSmaller)
    {
        sinalpha = sinTheta(localWo);
        tanbeta = sinTheta(localWi) / cosTheta(localWi);
    }
    else
    {
        sinalpha = sinTheta(localWi);
        tanbeta = sinTheta(localWo) / cosTheta(localWo);
    }
    const Spectrum r = baseColor_ * INV_PI *
                       (a_ + b_ * cosPhiiPhioClamped * sinalpha * tanbeta);
    return r;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum OrenNayar::bsdfSample(Vec3 localWo,
                               Sampler* sampler,
                               Vec3* localWi,
                               float* aPdf) const
{
    float pdfHS;
    *localWi = sampler->getHemisphereCosineWeighted(&pdfHS);
    // 裏面であればwiも裏返す
    if (localWo.z() < 0.)
    {
        localWi->setZ(-localWi->z());
    }
    //
    *aPdf = pdf(localWo, *localWi);
    return bsdf(localWo, *localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
REGISTER_OBJECT(BSDF, Mirror);

/*
-------------------------------------------------
-------------------------------------------------
*/
Mirror::Mirror(const ObjectProp& objectProp) : BSDF(objectProp)
{
    // TODO: リフレクタンスを指定できるようにする
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Mirror::isDeltaFunc() const { return true; }

/*
-------------------------------------------------
-------------------------------------------------
*/
float Mirror::pdf(Vec3 localWo, Vec3 localWi) const
{
    // δ関数なので常に0を返す。
    // 普通の用途ではまず呼び出されないのでASSERT()を置いておく
    AL_ASSERT_DEBUG(false);
    return 0.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Mirror::bsdf(Vec3 localWo, Vec3 localWi) const
{
    // δ関数なので常に0を返す。
    // 普通の用途ではまず呼び出されないのでASSERT()を置いておく
    AL_ASSERT_DEBUG(false);
    return Spectrum::Black;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Mirror::bsdfSample(Vec3 localWo,
                            Sampler* sampler,
                            Vec3* localWi,
                            float* pdf) const
{
    // 単純なreflect
    *localWi = Vec3(-localWo.x(), -localWo.y(), localWo.z());
    *pdf = 1.0f;
    // 反射率 1.0
    return Spectrum(1.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Glass::Glass(const ObjectProp& objectProp) : BSDF(objectProp)
{
    ior_ = objectProp.findChildBy("name", "ior").asFloat(2.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Glass::isDeltaFunc() const { return true; }

/*
-------------------------------------------------
-------------------------------------------------
*/
float Glass::pdf(Vec3 localWo, Vec3 localWi) const
{
    // δ関数なので常に0を返す。
    // 普通の用途ではまず呼び出されないのでASSERT()を置いておく
    AL_ASSERT_DEBUG(false);
    return 0.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Glass::bsdf(Vec3 localWo, Vec3 localWi) const
{
    // δ関数なので常に0を返す。
    // 普通の用途ではまず呼び出されないのでASSERT()を置いておく
    AL_ASSERT_DEBUG(false);
    return Spectrum::Black;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Glass::bsdfSample(Vec3 localWo,
                           Sampler* sampler,
                           Vec3* localWi,
                           float* pdf) const
{
    //
    const float glassProb = sampler->get1d();
    // Snellの法則から入射方向を算出
    const float cosi = alMin(localWo.z(), 1.0f);
    float ei = 1.0f;
    float et = ior_;
    const float sini = sqrtf(1.0f - cosi * cosi);
    // 内から外の場合であればswap()
    const bool fromOut = (cosi >= 0.0f);
    if (!fromOut)
    {
        std::swap(ei, et);
    }
    //
    const float sint = ei / et * sini;
    const float cos2t = 1.0f - sint * sint;
    const Vec3 reflectDir(-localWo.x(), -localWo.y(), localWo.z());
    //
    Spectrum spectrum;
    // 完全反射の場合
    if (cos2t <= 0.0f)
    {
        // 単純なreflect
        *localWi = reflectDir;
        // 反射率 1.0
        spectrum = Spectrum(1.0f);
    }
    // 完全反射とはならない場合
    else
    {
        const float cost = sqrtf(cos2t);
        // 透過方向の算出

        const float z = fromOut ? -cost : cost;
        float x, y;
        Vec2 xy(-localWo.x(), -localWo.y());
        const float lensq = xy.lengthSq();
        if (lensq == 0.0f)
        {
            x = 0.0f;
            y = 0.0f;
        }
        else
        {
            const float s = sqrtf((1.0f - cost * cost) / lensq);
            x = xy.x() * s;
            y = xy.y() * s;
        }
        Vec3 refractDir(x, y, z);
        AL_ASSERT_DEBUG(refractDir.isNormalized());

        // Schlick
        // TODO: Schlick近似ではなくDielectric用の計算をちゃんといれる
        const float t0 = ior_ - 1.0f;
        const float t1 = ior_ + 1.0f;
        const float R0 = (t0 * t0) / (t1 * t1);

        //
        const float prob = 0.5f;

        const bool isTransmit = (glassProb < prob);
        // 透過する場合
        if (isTransmit)
        {
            //
            *localWi = refractDir;
            // 反射量
            const float c = 1.0f - fabsf(refractDir.z());
            const float reflection = R0 + (1.0f - R0) * powf(c, 5.0f);
            // 透過量
            const float transmission = 1.0f - reflection;
            // 界面上での立体角変化分
            const float a2 = (ei * ei) / (et * et);
            // ロシアンルーレット分
            spectrum = Spectrum(transmission * a2 / prob);
            //
            AL_ASSERT_DEBUG(!spectrum.hasNaN());
        }
        // 反射する場合
        else
        {
            //
            *localWi = reflectDir;
            // 反射量
            const float c = 1.0f - fabsf(reflectDir.z());
            const float reflection = R0 + (1.0f - R0) * powf(c, 5.0f);
            // ロシアンルーレット分
            spectrum = Spectrum(reflection / (1.0f - prob));
            //
            AL_ASSERT_DEBUG(!spectrum.hasNaN());
        }
    }
    //
    AL_ASSERT_DEBUG(!spectrum.hasNaN());
    //
    *pdf = 1.0f;
    //
    return spectrum;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
MicrofacetBSDF::MicrofacetBSDF(const ObjectProp& objectProp) : BSDF(objectProp)
{
    // HACK: しばらくは決め打ちで作成する
    R = Spectrum::White;
    fresnel_ = std::make_shared<FresnelConductor>(
        Spectrum::createFromRGB({{1.0f, 1.0f, 1.0f}}, false),
        Spectrum::createFromRGB({{1.0f, 1.0f, 1.0f}}, false));

    // HACK: とりあえずのコード
    const float roughness =
        objectProp.findChildBy("name", "roughness").asFloat(1000.0f);
    distribution_ = std::make_shared<BlinnNDF>(roughness);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
MicrofacetBSDF::MicrofacetBSDF(const Spectrum& baseColor,
                               FresnelTermPtr fTerm,
                               MicrofacetDistributionPtr dTerm)
{
    R = baseColor;
    distribution_ = dTerm;
    fresnel_ = fTerm;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum MicrofacetBSDF::bsdf(Vec3 wo, Vec3 wi) const
{
    float cosThetaO = absCosTheta(wo);
    float cosThetaI = absCosTheta(wi);
    if (cosThetaI == 0.f || cosThetaO == 0.f)
    {
        return Spectrum(0.0f);
    }
    //
    Vec3 wh = wi + wo;
    // 全て0であったら無効
    if (!wh.any())
    {
        return Spectrum(0.0f);
    }
    wh.normalize();
    const float cosThetaH = Vec3::dot(wi, wh);
    const Spectrum F = fresnel_->eval(cosThetaH);
    return R * distribution_->eval(wh) * G(wo, wi, wh) * F /
           (4.0f * cosThetaI * cosThetaO);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float MicrofacetBSDF::pdf(Vec3 localWo, Vec3 localWi) const
{
    if (!sameHemisphere(localWo, localWi))
    {
        return 0.0f;
    }
    return distribution_->pdf(localWo, localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum MicrofacetBSDF::bsdfSample(Vec3 localWo,
                                    Sampler* sampler,
                                    Vec3* localWi,
                                    float* pdf) const
{
    distribution_->sample(localWo, sampler, localWi, pdf);
    if (!sameHemisphere(localWo, *localWi))
    {
        *pdf = 0.0f;
        return Spectrum(0.0f);
    }
    //
    return bsdf(localWo, *localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TorranceSparrow::TorranceSparrow(const Spectrum& baseColor,
                                 const MicrofacetDistributionPtr& dTerm,
                                 const GeometricTermPtr& gTerm,
                                 const FresnelTermPtr fTerm)
    : BSDF(ObjectProp()), baseColor_(baseColor), dTerm_(dTerm), gTerm_(gTerm),
      fTerm_(fTerm)
{
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float TorranceSparrow::pdf(Vec3 localWo, Vec3 localWi) const
{
    if (!sameHemisphere(localWo, localWi))
    {
        return 0.0f;
    }
    return dTerm_->pdf(localWo, localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum TorranceSparrow::bsdf(Vec3 localWo, Vec3 localWi) const
{
    //
    const float cto = absCosTheta(localWo);
    const float cti = absCosTheta(localWi);
    //
    if (cto == 0.0f || cti == 0.0f)
    {
        return Spectrum(0.0f);
    }
    // マイクロファセットの法線
    Vec3 wh = (localWo + localWi);
    if (!wh.any())
    {
        return Spectrum(0.0f);
    }
    wh.normalize();
    //
    const float d = dTerm_->eval(wh);
    // フレネル用の微小面と入射方向のcos(i)はwhで考えることに注意
    const float cth = Vec3::dot(localWi, wh);
    const Spectrum f = fTerm_->eval(cth);
    const float g = gTerm_->eval(localWo, localWi, wh);
    const Spectrum r = (baseColor_ * d * g * f) / (4.0f * cto * cti);
    return r;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum TorranceSparrow::bsdfSample(Vec3 localWo,
                                     Sampler* sampler,
                                     Vec3* localWi,
                                     float* pdf) const
{
    dTerm_->sample(localWo, sampler, localWi, pdf);
    if (!sameHemisphere(localWo, *localWi))
    {
        return Spectrum(0.0f);
    }
    return bsdf(localWo, *localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
REGISTER_OBJECT(BSDF, Ward);

/*
-------------------------------------------------
-------------------------------------------------
*/
Ward::Ward(const ObjectProp& objectProp) : BSDF(objectProp)
{
    alpha_x = 0.15f;
    alpha_y = 0.15f;
    cs_ = Vec3(1.0f, 1.0f, 1.0f);
    cd_ = Vec3(1.0f, 0.5f, 0.5f);
    isotropic_ = false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Ward::pdf(Vec3 localWo, Vec3 localWi) const
{
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return 0.0f;
    }
    return absCosTheta(localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Ward::bsdf(Vec3 localWo, Vec3 localWi) const
{
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return Spectrum(0.0f);
    }
    //
    const Vec3 X(1.0f, 0.0f, 0.0f);
    const Vec3 Y(0.0f, 1.0f, 0.0f);
    //
    const Vec3 H = (localWi + localWo).normalize();
    //
    float ax = alpha_x;
    float ay = isotropic_ ? alpha_x : alpha_y;
    float exponent = -(sqr(Vec3::dot(H, X) / ax) + sqr(Vec3::dot(H, Y) / ay)) /
                     sqr(cosTheta(H));
    float spec =
        1.0f / (4.0f * PI * ax * ay *
                sqrtf(cosTheta(localWi) * Vec3::dot(localWo, localWi)));
    spec *= exp(exponent);
    //
    const Vec3 col = cd_ * INV_PI + cs_ * spec;
    return Spectrum::createFromRGB({{col.x(), col.y(), col.z()}}, false);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Ward::bsdfSample(const Vec3 localWo,
                          Sampler* sampler,
                          Vec3* localWi,
                          float* pdf) const
{
    float pdfHS;
    *localWi = sampler->getHemisphereCosineWeighted(&pdfHS);
    // 裏面の場合
    if (localWo.z() < 0.)
    {
        localWi->setZ(localWi->z() * -1.0f);
    }
    //
    *pdf = this->pdf(localWo, *localWi);
    return bsdf(localWo, *localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
REGISTER_OBJECT(BSDF, Walter);

/*
-------------------------------------------------
-------------------------------------------------
*/
Walter::Walter(const ObjectProp& objectProp) : BSDF(objectProp)
{
    Kd_ = 0.0f;
    Ks_ = 0.1f;
    alphaG_ = 0.1f;
    ior_ = 2.0f;
    useFresnel_ = false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Walter::pdf(Vec3 localWo, Vec3 localWi) const
{
    // TODO: 本当のImportanceSampling
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return 0.0f;
    }
    return absCosTheta(localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
static float GGX(float NdotH, float alphaG)
{
    return alphaG * alphaG * INV_PI /
           (sqr(NdotH * NdotH * (alphaG * alphaG - 1.0f) + 1.0f));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
static float smithG_GGX(float Ndotv, float alphaG)
{
    return 2.0f / (1.0f + sqrtf(1.0f +
                                alphaG * alphaG * (1.0f - Ndotv * Ndotv) /
                                    (Ndotv * Ndotv)));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Walter::bsdf(Vec3 localWo, Vec3 localWi) const
{
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return Spectrum(0.0f);
    }
    //
    const Vec3 X(1.0f, 0.0f, 0.0f);
    const Vec3 Y(0.0f, 1.0f, 0.0f);
    const float NdotL = cosTheta(localWi);
    const float NdotV = cosTheta(localWo);
    if (NdotL < 0 || NdotV < 0)
    {
        return Spectrum::Black;
    }
    const Vec3 H = (localWi + localWo).normalize();
    const float NdotH = cosTheta(H);
    const float VdotH = Vec3::dot(localWo, H);
    const float D = GGX(NdotH, alphaG_);
    const float G = smithG_GGX(NdotL, alphaG_) * smithG_GGX(NdotV, alphaG_);
    // fresnel
    const float c = VdotH;
    const float g = sqrt(ior_ * ior_ + c * c - 1);
    const float F = useFresnel_
                        ? 0.5f * powf(g - c, 2.0f) / powf(g + c, 2.0f) *
                              (1.0f +
                               powf(c * (g + c) - 1.0f, 2.0f) /
                                   pow(c * (g - c) + 1.0f, 2.0f))
                        : 1.0f;
    const float val = Kd_ / PI + Ks_ * D * G * F / (4 * NdotL * NdotV);
    return Spectrum::createFromRGB({{val, val, val}}, false);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum Walter::bsdfSample(Vec3 localWo,
                            Sampler* sampler,
                            Vec3* localWi,
                            float* pdf) const
{
    float pdfHS;
    *localWi = sampler->getHemisphereCosineWeighted(&pdfHS);
    // 裏面の場合
    if (localWo.z() < 0.)
    {
        localWi->setZ(localWi->z() * -1.0f);
    }
    //
    *pdf = this->pdf(localWo, *localWi);
    return bsdf(localWo, *localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
REGISTER_OBJECT(BSDF, AshikhminShirley);

/*
-------------------------------------------------
-------------------------------------------------
*/
AshikhminShirley::AshikhminShirley(const ObjectProp& objectProp)
    : BSDF(objectProp)
{
    rs_ = 0.1f;
    rd_ = 1.0f;
    nu_ = 100.0f;
    nv_ = 100.0f;
    isotropic_ = true;
    coupled_diffuse_ = true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float AshikhminShirley::pdf(Vec3 localWo, Vec3 localWi) const
{
    // TODO: 本当のImportanceSampling
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return 0.0f;
    }
    return absCosTheta(localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Fresnel(float f0, float u)
{
    return f0 + (1.0f - f0) * pow(1.0f - u, 5.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum AshikhminShirley::bsdf(Vec3 localWo, Vec3 localWi) const
{
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return Spectrum(0.0f);
    }
    //
    const Vec3 L = localWi;
    const Vec3 V = localWo;
    const Vec3 N(0.0f, 0.0f, 1.0f);
    const Vec3 X(1.0f, 0.0f, 0.0f);
    const Vec3 Y(0.0f, 1.0f, 0.0f);
    //
    const Vec3 H = (L + V).normalize();
    const float HdotV = Vec3::dot(H, V);
    const float HdotX = Vec3::dot(H, X);
    const float HdotY = Vec3::dot(H, Y);
    const float NdotH = Vec3::dot(N, H);
    const float NdotV = Vec3::dot(N, V);
    const float NdotL = Vec3::dot(N, L);

    const float F = Fresnel(rs_, HdotV);
    const float norm_s =
        sqrt((nu_ + 1) * ((isotropic_ ? nu_ : nv_) + 1)) / (8 * PI);
    const float n = isotropic_ ? nu_
                               : (nu_ * sqr(HdotX) + nv_ * sqr(HdotY)) /
                                     (1.0f - sqr(NdotH));
    const float rho_s = norm_s * F * powf(alMax(NdotH, 0.0f), n) /
                        (HdotV * alMax(NdotV, NdotL));

    float rho_d = 28.0f / (23.0f * PI) * rd_ *
                  (1 - pow(1 - NdotV / 2.0f, 5.0f)) *
                  (1 - powf(1 - NdotL / 2.0f, 5.0f));
    if (coupled_diffuse_)
    {
        rho_d *= (1 - rs_);
    }
    const Vec3 val(rho_s + rho_d);
    return Spectrum::createFromRGB({{val.x(), val.y(), val.z()}}, false);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum AshikhminShirley::bsdfSample(Vec3 localWo,
                                      Sampler* sampler,
                                      Vec3* localWi,
                                      float* pdf) const
{
    float pdfHS;
    *localWi = sampler->getHemisphereCosineWeighted(&pdfHS);
    // 裏面の場合
    if (localWo.z() < 0.)
    {
        localWi->setZ(localWi->z() * -1.0f);
    }
    //
    *pdf = this->pdf(localWo, *localWi);
    return bsdf(localWo, *localWi);
}

// #define USE_AUTO_IMPORTANCE

REGISTER_OBJECT(BSDF, DisneyBRDF);

const float SPECULAR_CLAMP = 100.0f;

/*
-------------------------------------------------
-------------------------------------------------
*/
DisneyBRDF::DisneyBRDF(const ObjectProp& objectProp) : BSDF(objectProp)
{
    //
    std::string baseColorTexture = "none";
    // float baseColorGamma = 2.2f;
    // TODO: テクスチャ以外も取り扱えるようにする。
    baseColorTexture =
        objectProp.findChildBy("name", "basecolor").asString("none");
    // baseColorGamma = objectProp.findChildBy("name", "gamma").asFloat(2.2f);
    // //
    // FIXME: このBSDFのObjectPropではなく、BaseColorのObjectPropにする。
    baseColor_ = Spectrum::createFromRGB({{0.5f, 0.5f, 0.5f}}, false);
    metallic_ =
        objectProp.findChildBy("name", "metalic").asFloat(0.5f); // [0,1]
    subsurface_ =
        objectProp.findChildBy("name", "subsurface").asFloat(0.0f); // [0,1]
    specular_ =
        objectProp.findChildBy("name", "specular").asFloat(0.0f); // [0,1]
    roughness_ =
        objectProp.findChildBy("name", "roughness").asFloat(0.5f); // [0,1]
    specularTint_ =
        objectProp.findChildBy("name", "specularTint").asFloat(0.0f); // [0,1]
    anisotropic_ =
        objectProp.findChildBy("name", "anisotropic").asFloat(0.0f); // [0,1]
    sheen_ = objectProp.findChildBy("name", "sheen").asFloat(0.0f);  // [0,1]
    sheenTint_ =
        objectProp.findChildBy("name", "sheenTint").asFloat(0.0f); // [0,1]
    clearcoat_ =
        objectProp.findChildBy("name", "clearcoat").asFloat(0.0f); // [0,1]
    clearcoatGloss_ =
        objectProp.findChildBy("name", "clearcoatGloss").asFloat(0.0f); // [0,1]
                                                                        //
#if defined(USE_AUTO_IMPORTANCE)
    autoImportance_.setBRDF(this);
#endif
    //
    const float aspect = sqrt(1.0f - anisotropic_ * 0.9f);
    alphaX_ = alMax(0.001f, sqr(roughness_) / aspect);
    alphaY_ = alMax(0.001f, sqr(roughness_) * aspect);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float DisneyBRDF::D_GTR1(Vec3 m, float MdotN2) const
{
    auto alpha = alLerp(clearcoatGloss_, 0.1f, 0.001f);
    auto a2 = sqr(alpha);
    float denominator = logf(a2) * (1.0f + (a2 - 1.0f) * MdotN2);
    return (a2 - 1.0f) * INV_PI / denominator;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float DisneyBRDF::D_GTR2Aniso(Vec3 m, float MdotN2) const
{
    float HdotU = Vec3::dot(m, Vec3(1.0f, 0.0f, 0.0f));
    float HdotV = Vec3::dot(m, Vec3(0.0f, 1.0f, 0.0f));

    float denominator = alphaX_ * alphaY_ * sqr(sqr(HdotU / alphaX_) +
                                                sqr(HdotV / alphaY_) + MdotN2);
    return INV_PI / denominator;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float DisneyBRDF::pdf(Vec3 localWo, Vec3 localWi) const
{
#if defined(USE_AUTO_IMPORTANCE)
    return autoImportance_.pdf(localWo, localWi);
#elif 1
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return 0.0f;
    }
    return absCosTheta(localWi);
#else
    return evalSpecularPdf(localWo, localWi);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
static float SchlickFresnel(float u)
{
    //
    const float m = alClamp(1.0f - u, 0.0f, 1.0f);
    const float m2 = m * m;
    return m2 * m2 * m; // pow(m,5)
}

/*
-------------------------------------------------
-------------------------------------------------
*/
static float GTR1(float NdotH, float a)
{
    //
    if (a >= 1.0f)
    {
        return INV_PI;
    }
    const float a2 = a * a;
    const float t = 1.0f + (a2 - 1.0f) * NdotH * NdotH;
    return (a2 - 1.0f) / (logf(a2) * t) * INV_PI;
}

#if 0
/*
-------------------------------------------------
-------------------------------------------------
*/
static float GTR2(float NdotH, float a)
{
    //
    const float a2 = a * a;
    const float tmp0 = (a2 - 1.0f);
    const float tmp1 = tmp0 * NdotH * NdotH;
    const float t = 1.0f + tmp1;
    const float tmp = a2 / (t * t) * INV_PI;

    /*
    HACK: 非常にまずいがclamp。
    MIS時にLightサンプルでどうしてもtがほぼ0になることがありうる
    */
    return alClamp(tmp, 0.0f, SPECULAR_CLAMP);
}
#endif

/*
-------------------------------------------------
-------------------------------------------------
*/
static float
GTR2_aniso(float NdotH, float HdotX, float HdotY, float ax, float ay)
{
    //
    const float tmp0 = PI * ax * ay;
    const float tmp10 = sqr(HdotX / ax);
    const float tmp11 = sqr(HdotY / ay);
    const float tmp12 = NdotH * NdotH;
    const float tmp13 = tmp10 + tmp11 + tmp12;
    const float tmp1 = sqr(tmp13);
    const float tmp2 = 1.0f / (tmp0 * tmp1);
    /*
    HACK: 非常にまずいがclamp。
    MIS時にLightサンプルでどうしてもtがほぼ0になることがありうる
    */
    return alClamp(tmp2, 0.0f, SPECULAR_CLAMP);
}

///*
//*/
// static float smithG_GGX(float Ndotv, float alphaG)
//{
//    const float a = alphaG * alphaG;
//    const float b = Ndotv * Ndotv;
//    return 1.0f / (Ndotv + sqrtf(a + b - a*b));
//}

/*
-------------------------------------------------
-------------------------------------------------
*/
static Vec3 mon2lin(Vec3 x)
{
    return Vec3(powf(x[0], 2.2f), powf(x[1], 2.2f), powf(x[2], 2.2f));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum DisneyBRDF::bsdf(Vec3 localWo, Vec3 localWi) const
{
    // 同じ面で無い場合は0
    if (!sameHemisphere(localWo, localWi))
    {
        return Spectrum(0.0f);
    }
    //
    const Vec3 N(0.0f, 0.0f, 1.0f);
    const Vec3 X(1.0f, 0.0f, 0.0f);
    const Vec3 Y(0.0f, 1.0f, 0.0f);

    //
    const float NdotL = cosTheta(localWi);
    const float NdotV = cosTheta(localWo);
    if (NdotL < 0.0f || NdotV < 0.0f)
    {
        Spectrum::createFromRGB({{0.0f, 0.0f, 0.0f}}, false);
    }

    //
    const Vec3 H = (localWi + localWo).normalize();
    const float NdotH = Vec3::dot(N, H);
    const float LdotH = Vec3::dot(localWi, H);

    //
    SpectrumRGB baseColorRgb;
    baseColor_.toRGB(baseColorRgb);
    const Vec3 Cdlin =
        mon2lin(Vec3(baseColorRgb.r, baseColorRgb.g, baseColorRgb.b));
    const float Cdlum = 0.3f * Cdlin[0] + 0.6f * Cdlin[1] + 0.1f * Cdlin[2];

    //
    const Vec3 Ctint = Cdlum > 0.0f ? Cdlin / Cdlum : Vec3(1.0f);
    const Vec3 Cspec0 =
        alLerp(specular_ * 0.08f * alLerp(Vec3(1.0f), Ctint, specularTint_),
               Cdlin,
               metallic_);
    const Vec3 Csheen = alLerp(Vec3(1.0f), Ctint, sheenTint_);

    //
    const float FL = SchlickFresnel(NdotL);
    const float FV = SchlickFresnel(NdotV);
    const float Fd90 = 0.5f + 2.0f * LdotH * LdotH * roughness_;
    const float Fd = alLerp(1.0f, Fd90, FL) * alLerp(1.0f, Fd90, FV);

    //
    const float Fss90 = LdotH * LdotH * roughness_;
    const float Fss = alLerp(1.0f, Fss90, FL) * alLerp(1.0f, Fss90, FV);
    const float ss = 1.25f * (Fss * (1.0f / (NdotL + NdotV) - 0.5f) + 0.5f);

    // specular
    const float Ds =
        GTR2_aniso(NdotH, Vec3::dot(H, X), Vec3::dot(H, Y), alphaX_, alphaY_);
    // const float Ds = GTR2(NdotH, ax);
    const float FH = SchlickFresnel(LdotH);
    const Vec3 Fs = alLerp(Cspec0, Vec3(1.0f), FH);
    const float roughg = sqr(roughness_ * 0.5f + 0.5f);
    const float Gs = smithG_GGX(NdotL, roughg) * smithG_GGX(NdotV, roughg);

    // sheen
    const Vec3 Fsheen = FH * sheen_ * Csheen;

    // clearcoat
    const float Dr = GTR1(NdotH, alLerp(0.1f, 0.001f, clearcoatGloss_));
    const float Fr = alLerp(0.04f, 1.0f, FH);
    const float Gr = smithG_GGX(NdotL, 0.25f) * smithG_GGX(NdotV, 0.25f);

    //
    const Vec3 rgb = (INV_PI * alLerp(Fd, ss, subsurface_) * Cdlin + Fsheen) *
                         (1.0f - metallic_) +
                     Gs * Fs * Ds + Vec3(0.25f * clearcoat_ * Gr * Fr * Dr);
    AL_ASSERT_DEBUG(!rgb.hasNan());
    return SpectrumRGB::createFromRGB({{rgb.x(), rgb.y(), rgb.z()}}, false);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum DisneyBRDF::bsdfSample(Vec3 localWo,
                                Sampler* sampler,
                                Vec3* localWi,
                                float* pdf) const
{
#if defined(USE_AUTO_IMPORTANCE)
    autoImportance_.sample(localWo, rng, localWi, pdf);
    const Spectrum reflectance = bsdf(localWo, *localWi, uv);
    return reflectance;
#elif 1
    float pdfHS;
    *localWi = sampler->getHemisphereCosineWeighted(&pdfHS);
    // 裏面の場合
    if (localWo.z() < 0.0f)
    {
        localWi->setZ(localWi->z() * -1.0f);
    }
    //
    *pdf = this->pdf(localWo, *localWi);
    return bsdf(localWo, *localWi);
#else
// TODO
// http://renderloop.blogspot.jp/2015/07/implementing-disney-principled-brdf-in.html
// あたりを参考にちゃんとISする
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
// const int32_t BRDF_SAMPLING_RES_THETA_H = 90;
// const int32_t BRDF_SAMPLING_RES_THETA_D = 90;
// const int32_t BRDF_SAMPLING_RES_PHI_D = 360;

/*
-------------------------------------------------
-------------------------------------------------
*/
MeasuredBSDF::MeasuredBSDF(const std::string& fileName) : BSDF(ObjectProp())
{
    //
    std::ifstream file(fileName.c_str(),
                       std::ios_base::in | std::ifstream::binary);
    AL_ASSERT_DEBUG(file.is_open());
    //
    std::array<int32_t, 3> dims;
    file >> dims[0];
    file >> dims[1];
    file >> dims[2];
    const int32_t n = dims[0] * dims[1] * dims[2];
    // AL_ASSERT_DEBUG(n == BRDF_SAMPLING_RES_THETA_H *
    // BRDF_SAMPLING_RES_THETA_D
    // * BRDF_SAMPLING_RES_PHI_D / 2);
    //
    std::vector<double> tmpBuffer;
    tmpBuffer.resize(sizeof(double) * 3 * n);
    file.read((char*)tmpBuffer.data(), tmpBuffer.size());
    //
    brdf.reserve(tmpBuffer.size());
    for (auto& v : tmpBuffer)
    {
        brdf.push_back((float)v);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float MeasuredBSDF::pdf(Vec3 localWo, Vec3 localWi) const
{
    // TODO: 実装
    // http://people.csail.mit.edu/wojciech/BRDFDatabase/code/BRDFRead.cpp
    // を参考に。
    assert(!"needs impliment");
    return 1.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum MeasuredBSDF::bsdf(Vec3 localWo, Vec3 localWi) const
{
    // TODO: 実装
    assert(!"needs impliment");
    return Spectrum(0.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum MeasuredBSDF::bsdfSample(Vec3 localWo,
                                  Sampler* sampler,
                                  Vec3* localWi,
                                  float* pdf) const
{
    // TODO: 実装
    assert(!"needs impliment");
    return Spectrum(0.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
MTLBSDF::MTLBSDF(
        const Spectrum& diffuse,
        const Spectrum& specular,
        const Spectrum& transmittance,
        const Spectrum& emission,
        float shiniess,
        float ior,
        float dissolve,
        int32_t illum)
{
    // diffuse
    bsdfs_.add(std::make_shared<Lambertian>(diffuse));
    // specular
    bsdfs_.add(
        std::make_shared<MicrofacetBSDF>(
            specular,
            //std::make_shared<FresnelDielectric>(10.0f, 1.0f),
            std::make_shared<FresnelNone>(),
            std::make_shared<BlinnNDF>(shiniess)));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float MTLBSDF::pdf(Vec3 localWo, Vec3 localWi) const
{
    return bsdfs_.pdf(localWo, localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum MTLBSDF::bsdf(Vec3 localWo, Vec3 localWi) const
{
    return bsdfs_.bsdf(localWo, localWi);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum MTLBSDF::bsdfSample(Vec3 localWo,
    Sampler* sampler,
    Vec3* localWi,
    float* pdf) const
{
    return bsdfs_.bsdfSample(localWo, sampler, localWi, pdf);
}
