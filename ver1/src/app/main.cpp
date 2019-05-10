#include "app/alhazen.hpp"

#include "core/math.hpp"
#include "sampler/sampler.hpp"
#include "core/floatstreamstats.hpp"

//
template <typename T, typename U, typename V>
inline T Clamp(T val, U low, V high) {
    if (val < low)
        return low;
    else if (val > high)
        return high;
    else
        return val;
}

// BSDF Inline Functions
inline float CosTheta(const Vec3 &w) { return w.z(); }
inline float Cos2Theta(const Vec3 &w) { return w.z() * w.z(); }
inline float AbsCosTheta(const Vec3 &w) { return std::abs(w.z()); }
inline float Sin2Theta(const Vec3 &w) {
    return std::max((float)0, (float)1 - Cos2Theta(w));
}

inline float SinTheta(const Vec3 &w) { return std::sqrt(Sin2Theta(w)); }

inline float TanTheta(const Vec3 &w) { return SinTheta(w) / CosTheta(w); }

inline float Tan2Theta(const Vec3 &w) {
    return Sin2Theta(w) / Cos2Theta(w);
}

inline float CosPhi(const Vec3 &w) {
    float sinTheta = SinTheta(w);
    return (sinTheta == 0) ? 1 : Clamp(w.x() / sinTheta, -1.0f, 1.0f);
}

inline float SinPhi(const Vec3 &w) {
    float sinTheta = SinTheta(w);
    return (sinTheta == 0) ? 0 : Clamp(w.y() / sinTheta, -1.0f, 1.0f);
}

inline float Cos2Phi(const Vec3 &w) { return CosPhi(w) * CosPhi(w); }

inline float Sin2Phi(const Vec3 &w) { return SinPhi(w) * SinPhi(w); }


inline Vec3 SphericalDirection(float sinTheta, float cosTheta, float phi) {
    return Vec3(sinTheta * std::cosf(phi), sinTheta * std::sinf(phi),
        cosTheta);
}

inline float AbsDot(Vec3 v1, Vec3 v2)
{
    return std::abs(Vec3::dot(v1, v2));
}

inline bool SameHemisphere(const Vec3 &w, const Vec3 &wp) {
    return w.z() * wp.z() > 0;
}


class MicrofacetDistribution
{
public:
    MicrofacetDistribution(bool sampleVisibleArea)
        :sampleVisibleArea_(sampleVisibleArea)
    {}
    virtual ~MicrofacetDistribution() = default;
    virtual float D(Vec3 wh) const = 0;
    virtual float lambda(Vec3 w) const = 0;
    float G1(Vec3 w) const;
    float G(Vec3 wo, Vec3 wi) const;
    virtual Vec3 sampleWh(Vec3 wo, float u0, float u1) const = 0;
    float pdf(Vec3 wo, Vec3 wh) const;
protected:
    bool sampleVisibleArea_ = true;
};

class GGXDistribution 
    : public MicrofacetDistribution
{
public:
    GGXDistribution(float alphax, float alphay, bool sampleVisibleArea)
        : MicrofacetDistribution(sampleVisibleArea), alphaX_(alphax), alphaY_(alphay)
        {}
    float D(Vec3 wh) const override;
    Vec3 sampleWh(Vec3 wo, float u0, float u1) const override;

private:
    float lambda(Vec3 w) const override;
private:
    float alphaX_ = 0.0f;
    float alphaY_ = 0.0f;
};


float MicrofacetDistribution::G1(Vec3 w) const
{
    return 1.0f / (1.0f + lambda(w));
}

float MicrofacetDistribution::G(Vec3 wo, Vec3 wi) const
{
    return 1.0f / (1.0f + lambda(wo) + lambda(wi));
}

float MicrofacetDistribution::pdf(Vec3 wo, Vec3 wh) const
{
    if (sampleVisibleArea_)
    {
        return D(wh) * G1(wo) * AbsDot(wo, wh) / AbsCosTheta(wo);
    }
    else
    {
        return D(wh) * AbsCosTheta(wh);
    }
}


float GGXDistribution::D(const Vec3 wh) const
{
    float tan2Theta = Tan2Theta(wh);
    if (std::isinf(tan2Theta)) return 0.;
    const float cos4Theta = Cos2Theta(wh) * Cos2Theta(wh);
    float e =
        (Cos2Phi(wh) / (alphaX_ * alphaX_) + Sin2Phi(wh) / (alphaY_ * alphaY_)) *
        tan2Theta;
    return 1 / (float(M_PI) * alphaX_ * alphaY_ * cos4Theta * (1 + e) * (1 + e));
}


static void GGxSample11(float cosTheta, float U1, float U2,
    float *slope_x, float *slope_y) {
    // special case (normal incidence)
    if (cosTheta > .9999) {
        float r = sqrt(U1 / (1 - U1));
        float phi = 6.28318530718f * U2;
        *slope_x = r * cos(phi);
        *slope_y = r * sin(phi);
        return;
    }

    float sinTheta =
        std::sqrt(std::max((float)0, (float)1 - cosTheta * cosTheta));
    float tanTheta = sinTheta / cosTheta;
    float a = 1 / tanTheta;
    float G1 = 2 / (1 + std::sqrt(1.f + 1.f / (a * a)));

    // sample slope_x
    float A = 2 * U1 / G1 - 1;
    float tmp = 1.f / (A * A - 1.f);
    if (tmp > 1e10) tmp = 1e10;
    float B = tanTheta;
    float D = std::sqrt(
        std::max(float(B * B * tmp * tmp - (A * A - B * B) * tmp), float(0)));
    float slope_x_1 = B * tmp - D;
    float slope_x_2 = B * tmp + D;
    *slope_x = (A < 0 || slope_x_2 > 1.f / tanTheta) ? slope_x_1 : slope_x_2;

    // sample slope_y
    float S;
    if (U2 > 0.5f) {
        S = 1.f;
        U2 = 2.f * (U2 - .5f);
    }
    else {
        S = -1.f;
        U2 = 2.f * (.5f - U2);
    }
    float z =
        (U2 * (U2 * (U2 * 0.27385f - 0.73369f) + 0.46341f)) /
        (U2 * (U2 * (U2 * 0.093073f + 0.309420f) - 1.000000f) + 0.597999f);
    *slope_y = S * z * std::sqrt(1.f + *slope_x * *slope_x);

    //CHECK(!std::isinf(*slope_y));
    //CHECK(!std::isnan(*slope_y));
}

static Vec3
GGxSample(Vec3 wi, float alphaX, float alphaY, float U1, float U2)
{
    // 1. stretch wi
    Vec3 wiStretched = Vec3(alphaX * wi.x(), alphaY * wi.y(), wi.z()).normalized();

    // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
    float slope_x, slope_y;
    GGxSample11(CosTheta(wiStretched), U1, U2, &slope_x, &slope_y);

    // 3. rotate
    float tmp = CosPhi(wiStretched) * slope_x - SinPhi(wiStretched) * slope_y;
    slope_y = SinPhi(wiStretched) * slope_x + CosPhi(wiStretched) * slope_y;
    slope_x = tmp;

    // 4. unstretch
    slope_x = alphaX * slope_x;
    slope_y = alphaY * slope_y;

    // 5. compute normal
    return Vec3(-slope_x, -slope_y, 1.).normalized();
}

Vec3 GGXDistribution::sampleWh(
    Vec3 wo,
    float u0, float u1) const
{
    //
    if (sampleVisibleArea_)
    {
        bool flip = wo.z() < 0;
        Vec3 wh = GGxSample(flip ? -wo : wo, alphaX_, alphaY_, u0, u1);
        if (flip)
        {
            wh = -wh;
        }
        return wh;
    }
    //
    else
    {
        float cosTheta = 0, phi = (2 * PI) * u1;
        if (alphaX_ == alphaY_) {
            float tanTheta2 = alphaX_ * alphaX_ * u0 / (1.0f - u0);
            cosTheta = 1 / std::sqrt(1 + tanTheta2);
        }
        else {
            phi =
                std::atan(alphaY_ / alphaX_ * std::tan(2 * PI * u1 + .5f * PI));
            if (u1 > .5f) phi += PI;
            float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
            const float alphax2 = alphaX_ * alphaX_, alphay2 = alphaY_ * alphaY_;
            const float alpha2 =
                1 / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
            float tanTheta2 = alpha2 * u0 / (1 - u0);
            cosTheta = 1 / std::sqrt(1 + tanTheta2);
        }
        float sinTheta =
            std::sqrt(std::max((float)0., (float)1. - cosTheta * cosTheta));
        Vec3 wh = SphericalDirection(sinTheta, cosTheta, phi);
        if (!SameHemisphere(wo, wh)) wh = -wh;
        return wh;
    }
}

float GGXDistribution::lambda(const Vec3 w) const
{
    float absTanTheta = std::abs(TanTheta(w));
    if (std::isinf(absTanTheta)) return 0.;
    // Compute _alpha_ for direction _w_
    float alpha =
        std::sqrt(Cos2Phi(w) * alphaX_ * alphaX_ + Sin2Phi(w) * alphaY_ * alphaY_);
    float alpha2Tan2Theta = (alpha * absTanTheta) * (alpha * absTanTheta);
    return (-1 + std::sqrt(1.f + alpha2Tan2Theta)) / 2;
}


void test0()
{
    const float alphaX = 0.1f;
    const float alphaY = 0.1f;
    GGXDistribution ggx(alphaX, alphaY, true);
    SamplerHalton sampler;
    sampler.setHash(0x123);

#if 1
    /*
    ∫ D * dot(ωg,ωm) dωm = 1
    \int _{ \Omega  }^{  }{ \omega _{ g }\cdot \omega _{ m }D\left( \omega _{ m } \right) d\omega _{ m } } =1
    */
    {
        FloatStreamStats<> fs;
        for (int32_t sn = 0; sn < 1024 * 16; ++sn)
        {
            sampler.startSample(sn);
            float hsPdf;
            const Vec3 w = sampler.getHemisphere(&hsPdf);
            fs.add(ggx.D(w) * w.z() / hsPdf);
        }
        printf("%f\n", fs.mu());
    }
#endif    

    // sampleWh()の半球積分が1
    // https://github.com/mmp/pbrt-v3/blob/9f717d847a807793fa966cf0eaa366852efef167/src/core/reflection.cpp#L415
    {
        FloatStreamStats<> fs;
        for (int32_t sn = 0; sn < 1024 * 16; ++sn)
        {
            sampler.startSample(sn);
            const Vec3 wo = Vec3(0.0f, 1.0f, 1.0f).normalized();
            const Vec3 wh = ggx.sampleWh(wo, sampler.get1d(), sampler.get1d());
            const Vec3 wi = -wh.reflect(wo);
            //
            if (!SameHemisphere(wo, wi))
            {
                continue;
            }
            const float f0 = ggx.pdf(wo, wh);
            const float f1 = 4.0f * Vec3::dot(wo, wh);
            const float pdf = ggx.pdf(wo, wh) / (4.0f * Vec3::dot(wo, wh));
            fs.add(pdf * 4.0f * PI );
        }
        printf("%f\n", fs.mu());
    }

    // TOOD: weak white funnannnnnnnnnceテストを入れる
    

    // TODO: 
    for (int32_t sn = 0; sn<1024; ++sn)
    {
        sampler.startSample(sn);
    }


}

/*
-------------------------------------------------
main
-------------------------------------------------
*/
int32_t main(int32_t argc, char* argv[])
{
    test0();
    return 0;
    //
    Alhazen app;
    return app.run(argc, argv);
}
