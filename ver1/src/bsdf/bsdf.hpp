#pragma once

#include "core/object.hpp"
#include "core/image.hpp"
#include "core/math.hpp"
#include "core/rng.hpp"
#include "bsdfutil.hpp"
#include "sampler/sampler.hpp"

typedef std::shared_ptr<class BSDF> BSDFPtr;

/*
-------------------------------------------------
BSDF
-------------------------------------------------
*/
class BSDF : public Object
{
public:
    /*
    -------------------------------------------------
    定義済みBSDF
    -------------------------------------------------
    */
    static BSDFPtr vantaBlack;
    static BSDFPtr gray18;
    static BSDFPtr gray50;
    static BSDFPtr white;

public:
    BSDF();
    BSDF(const ObjectProp& objectProp);
    virtual ~BSDF() {}
    std::string name() { return name_; }
    std::string id() { return id_; }
    virtual bool isDeltaFunc() const { return false; }
    virtual float pdf(Vec3 localWo, Vec3 localWi) const = 0;
    virtual Spectrum bsdf(Vec3 localWo, Vec3 localWi) const = 0;
    virtual Spectrum bsdfSample(Vec3 localWo,
                                Sampler* rng,
                                Vec3* localWi,
                                float* pdf) const = 0;
    Spectrum estimateTotalRefrectance(const int32_t sampleNum,
                                      bool dontUseBsdfSample = false) const;
    Spectrum estimateDirectionalRefrectance(
      Vec3 wo,
      int32_t sampleNum,
      bool dontUseBsdfSample = false) const;

protected:
    std::string name_ = "UNKNOWN";
    std::string id_ = "UNKNOWN";

};

/*
-------------------------------------------------
BSDFs
複数のBSDFが合わさったBSDF
-------------------------------------------------
*/
class BSDFs : public BSDF
{
public:
    BSDFs() = default;
    BSDFs(const ObjectProp& objectProp);
    void add(BSDFPtr bsdf);
    float pdf(Vec3 localWo, Vec3 localWi) const;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const;

private:
    std::vector<BSDFPtr> bsdfs_;
};

/*
-------------------------------------------------
MicrofacetDistribution(NormalDistoributionFunction:NDF)
マイクロファセットの方向の分布。D(ωh)
-------------------------------------------------
*/
class MicrofacetDistribution
{
public:
    virtual ~MicrofacetDistribution() {}
    virtual float eval(Vec3 wh) = 0;
    virtual float pdf(Vec3 wo, Vec3 wi) = 0;
    virtual void sample(Vec3 wo,
                        Sampler* sampler,
                        Vec3* wi,
                        float* pdf) const = 0;
};
typedef std::shared_ptr<MicrofacetDistribution> MicrofacetDistributionPtr;

/*
-------------------------------------------------
BlinnNDF
-------------------------------------------------
*/
class BlinnNDF : public MicrofacetDistribution
{
public:
    BlinnNDF() = default;
    BlinnNDF(float e);
    float eval(Vec3 wh) override;
    float pdf(Vec3 wo, Vec3 wi) override;
    void sample(Vec3 wo,
                Sampler* sampler,
                Vec3* wi,
                float* pdf) const override;

private:
    float e_ = 0.0f;
};

/*
-------------------------------------------------
GeometricTerm
幾何項。G(ωo,ωi,ωh)
-------------------------------------------------
*/
class GeometricTerm
{
public:
    virtual ~GeometricTerm() {}
    virtual float eval(Vec3 wo, Vec3 wi, Vec3 wh) const = 0;
};
typedef std::shared_ptr<GeometricTerm> GeometricTermPtr;

/*
-------------------------------------------------
GeometricTermDefault
無限の深さのV字を仮定した幾何項. pbr本 p455
-------------------------------------------------
*/
class GeometricTermDefault : public GeometricTerm
{
public:
    float eval(Vec3 wo, Vec3 wi, Vec3 wh) const override;
};

/*
-------------------------------------------------
FresnelTerm
F(ωo)
-------------------------------------------------
*/
class FresnelTerm
{
public:
    virtual ~FresnelTerm() {}
    // cos(i), i=(微小面法線)dot(入射方向)。
    virtual Spectrum eval(float surfNormalDotIncomeDir) const = 0;
};
typedef std::shared_ptr<FresnelTerm> FresnelTermPtr;

/*
-------------------------------------------------
-------------------------------------------------
*/
class FresnelNone : public FresnelTerm
{
public:
    Spectrum eval(float surfNormalDotIncomeDir) const override;
};

/*
-------------------------------------------------
FresnelConductor
伝導体のフレネル
-------------------------------------------------
*/
class FresnelConductor : public FresnelTerm
{
public:
    FresnelConductor(const Spectrum& eta, const Spectrum& k);
    Spectrum eval(float surfNormalDotIncomeDir) const override;

private:
    Spectrum eta_;
    Spectrum k_;
};

/*
-------------------------------------------------
FresnelDielectric
絶縁体のフレネル
-------------------------------------------------
*/
class FresnelDielectric : public FresnelTerm
{
public:
    FresnelDielectric(float etai, float etat);
    Spectrum eval(float surfNormalDotIncomeDir) const;

private:
    float etai_;
    float etat_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class Lambertian : public BSDF
{
public:
    Lambertian() = default;
    Lambertian(const ObjectProp& objectProp);
    Lambertian(const Spectrum& spectrum);
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    Spectrum baseColor_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class OrenNayar : public BSDF
{
public:
    OrenNayar(const Spectrum& baseColor, float sigma);
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    Spectrum baseColor_;
    float a_;
    float b_;
};

/*
-------------------------------------------------
Mirror
-------------------------------------------------
*/
class Mirror : public BSDF
{
public:
    Mirror(const ObjectProp& objectProp);
    bool isDeltaFunc() const override;
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    // float ior_;
};

/*
-------------------------------------------------
Glass
-------------------------------------------------
*/
class Glass : public BSDF
{
public:
    Glass(const ObjectProp& objectProp);
    bool isDeltaFunc() const override;
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    float ior_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class MicrofacetBSDF : public BSDF
{
public:
    MicrofacetBSDF() = default;
    MicrofacetBSDF(const ObjectProp& objectProp);
    MicrofacetBSDF(const Spectrum& baseColor,
                   FresnelTermPtr fTerm,
                   MicrofacetDistributionPtr dTerm);
    Spectrum bsdf(Vec3 wo, Vec3 wi) const override;
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    float G(Vec3 wo, Vec3 wi, Vec3 wh) const
    {
        float NdotWh = absCosTheta(wh);
        float NdotWo = absCosTheta(wo);
        float NdotWi = absCosTheta(wi);
        float WOdotWh = std::fabsf(Vec3::dot(wo, wh));
        return std::min(1.0f,
                        std::min((2.f * NdotWh * NdotWo / WOdotWh),
                                 (2.f * NdotWh * NdotWi / WOdotWh)));
    }
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    // Microfacet Private Data
    Spectrum R;
    MicrofacetDistributionPtr distribution_;
    FresnelTermPtr fresnel_;
};

/*
-------------------------------------------------
TorranceSparrow
-------------------------------------------------
*/
class TorranceSparrow : public BSDF
{
public:
    TorranceSparrow(const Spectrum& baseColor,
                    const MicrofacetDistributionPtr& dTerm,
                    const GeometricTermPtr& gTerm,
                    const FresnelTermPtr fTerm);
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    Spectrum baseColor_;
    MicrofacetDistributionPtr dTerm_;
    GeometricTermPtr gTerm_;
    FresnelTermPtr fTerm_;
};

/*
-------------------------------------------------
Ward
-------------------------------------------------
*/
class Ward : public BSDF
{
public:
    Ward(const ObjectProp& objectProp);
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    float alpha_x;
    float alpha_y;
    Vec3 cs_;
    Vec3 cd_;
    bool isotropic_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class Walter : public BSDF
{
public:
    Walter(const ObjectProp& objectProp);
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    float Kd_;
    float Ks_;
    float alphaG_;
    float ior_;
    bool useFresnel_;
};

/*
-------------------------------------------------
AshikhminShirley
-------------------------------------------------
*/
class AshikhminShirley : public BSDF
{
public:
    AshikhminShirley(const ObjectProp& objectProp);
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    float rs_;
    float rd_;
    float nu_;
    float nv_;
    bool isotropic_;
    bool coupled_diffuse_;
};

/*
-------------------------------------------------
DisneyBRDF
ほぼ https://github.com/wdas/brdf/blob/master/src/brdfs/disney.brdf の移植
-------------------------------------------------
*/
class DisneyBRDF : public BSDF
{
public:
    DisneyBRDF(const ObjectProp& objectProp);
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    float D_GTR1(Vec3 m, float MdotN2) const;
    float D_GTR2Aniso(Vec3 m, float MdotN2) const;

private:
    Spectrum baseColor_;
    float metallic_;
    float subsurface_;
    float specular_;
    float roughness_;
    float specularTint_;
    float anisotropic_;
    float sheen_;
    float sheenTint_;
    float clearcoat_;
    float clearcoatGloss_;
    //
    float alphaX_;
    float alphaY_;
#if defined(USE_AUTO_IMPORTANCE)
    AutoImportance autoImportance_;
#endif
};

/*
-------------------------------------------------
MeasuredBSDF
-------------------------------------------------
*/
class MeasuredBSDF : public BSDF
{
public:
    MeasuredBSDF(const std::string& fileName);
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
                        Sampler* sampler,
                        Vec3* localWi,
                        float* pdf) const override;

private:
    std::vector<float> brdf;
};

/*
-------------------------------------------------
MTLBSDF
.mtlファイルのBSDF
テクスチャを導入するとマテリアルクラスを作らないといけなくなるが
テクスチャなしであればBSDFのままいけるのでテストとしてつくる
-------------------------------------------------
*/
class MTLBSDF : public BSDF
{
public:
    MTLBSDF(
        const Spectrum& diffuse,
        const Spectrum& specular,
        const Spectrum& transmittance,
        const Spectrum& emission,
        float shiniess,
        float ior,
        float dissolve,
        int32_t illum );
    float pdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdf(Vec3 localWo, Vec3 localWi) const override;
    Spectrum bsdfSample(Vec3 localWo,
        Sampler* sampler,
        Vec3* localWi,
        float* pdf) const override;
private:
    BSDFs bsdfs_;
};