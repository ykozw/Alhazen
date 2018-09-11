#include "app/alhazen.hpp"

#include "core/math.hpp"
#include "sampler/sampler.hpp"
#include "core/floatstreamstats.hpp"

//
float clamp(float v, float lo, float hi)
{
    if (v < lo)
    {
        return lo;
    }
    else if (hi < v)
    {
        return hi;
    }
    else
    {
        return v;
    }
}

//std::mt19937 mt(0x123);
//std::uniform_real_distribution<float> dist;
//const auto rng = [&]() -> float
//{
//    return dist(mt);
//};

constexpr float Pi = 3.14159265f;

namespace {

    // ----------------------------------------------------------------------------
    // Utility functions to compute trigonometric funtions for a vector
    // ----------------------------------------------------------------------------

    static inline float cosTheta(const Vec3 &w) {
        return w.z();
    }

    static inline float cos2Theta(const Vec3 &w) {
        float c = cosTheta(w);
        return c * c;
    }

    static inline float sinTheta(const Vec3 &w) {
        return std::sqrtf(std::max(0.0f, 1.0f - cos2Theta(w)));
    }

    static inline float tanTheta(const Vec3 &w) {
        return sinTheta(w) / cosTheta(w);
    }

    static inline float cosPhi(const Vec3 &w) {
        if (w.z() == 1.0f)
        {
            return 0.0f;
        }
        return w.x() / sinTheta(w);
    }

    static inline float cos2Phi(const Vec3 &w)
    {
        float c = cosPhi(w);
        return c * c;
    }

    static inline float sinPhi(const Vec3 &w)
    {
        if (w.z() == 1.0f)
        {
            return 0.0f;
        }
        return w.y() / sinTheta(w);
    }

    static inline float sin2Phi(const Vec3 &w)
    {
        const float s = sinPhi(w);
        return s * s;
    }

    // Rational approximation of inverse error function
    // This method is borrowed from PBRT v3 
    static inline float erfinv(float x)
    {
        float w, p;
        x = clamp(x, -0.99999f, 0.99999f);
        w = -std::log((1 - x) * (1 + x));
        if (w < 5) {
            w = w - 2.5f;
            p = 2.81022636e-08f;
            p = 3.43273939e-07f + p * w;
            p = -3.5233877e-06f + p * w;
            p = -4.39150654e-06f + p * w;
            p = 0.00021858087f + p * w;
            p = -0.00125372503f + p * w;
            p = -0.00417768164f + p * w;
            p = 0.246640727f + p * w;
            p = 1.50140941f + p * w;
        }
        else {
            w = std::sqrt(w) - 3;
            p = -0.000200214257f;
            p = 0.000100950558f + p * w;
            p = 0.00134934322f + p * w;
            p = -0.00367342844f + p * w;
            p = 0.00573950773f + p * w;
            p = -0.0076224613f + p * w;
            p = 0.00943887047f + p * w;
            p = 1.00167406f + p * w;
            p = 2.83297682f + p * w;
        }
        return p * x;
    }

    // Rational approximation of error function
    // This method is borrowed from PBRT v3
    static inline float erf(float x)
    {
        // constants
        float a1 = 0.254829592f;
        float a2 = -0.284496736f;
        float a3 = 1.421413741f;
        float a4 = -1.453152027f;
        float a5 = 1.061405429f;
        float p = 0.3275911f;

        // Save the sign of x
        int sign = 1;
        if (x < 0) sign = -1;
        x = std::abs(x);

        // A&S formula 7.1.26
        float t = 1 / (1 + p * x);
        float y =
            1 -
            (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);

        return sign * y;
    }

}  // anonymous namespace


   // ----------------------------------------------------------------------------
   // The interface for microfacet distribution
   // ----------------------------------------------------------------------------
class MicrofacetDistribution {
public:
    MicrofacetDistribution(float alphax, float alphay, bool sampleVisible)
        : alphax_{ alphax }
        , alphay_{ alphay }
        , sampleVisible_{ sampleVisible } {
    }

    virtual ~MicrofacetDistribution() {
    }

    // Sample microfacet normal
    virtual Vec3 sampleWm(Sampler* sampler, const Vec3 &wo) const = 0;

    // The lambda function, which appears in the slope-space sampling
    virtual float lambda(const Vec3 &wo) const = 0;

    // Smith's masking function
    float G1(const Vec3 &wo) const {
        return 1.0f / (1.0f + lambda(wo));
    }

    // Smith's masking-shadowing function
    float G(const Vec3 &wo, const Vec3 &wi) const {
        return G1(wo) * G1(wi);
    }

    // Weighting function
    float weight(const Vec3 &wo, const Vec3 &wi, const Vec3 &wm) const {
        if (!sampleVisible_) {
            return std::abs(Vec3::dot(wi,wm)) * G(wo, wi) /
                std::max(1.0e-8f, std::abs(cosTheta(wi) * cosTheta(wm)));
        }
        else {
            return G1(wo);
        }
    }

    // Private parameters
    float alphax_, alphay_;
    bool sampleVisible_;
};

// ----------------------------------------------------------------------------
// Beckmann distribution
// ----------------------------------------------------------------------------
class BeckmannDistribution : public MicrofacetDistribution {
public:
    BeckmannDistribution(float alphax, float alphay, bool sampleVisible)
        : MicrofacetDistribution{ alphax, alphay, sampleVisible } {
    }

    Vec3 sampleWm(Sampler* sampler, const Vec3& wo) const override {
        const Vec2 uv = sampler->get2d();
        const float U1 = uv.x();
        const float U2 = uv.y();

        if (!sampleVisible_) {
            // Sample half-vector without taking normal visibility into consideration
            float tan2Theta, phi;
            if (alphax_ == alphay_) {
                // Isotropic case
                // Following smapling formula can be found in [Walter et al. 2007]
                // "Microfacet Models for Refraction through Rough Surfaces"
                float alpha = alphax_;
                float logSample = std::log(1.0f - U1);
                tan2Theta = -alpha * alpha * logSample;
                phi = 2.0f * Pi * U2;
            }
            else {
                // Anisotropic case
                // Following sampling strategy is analytically derived from 
                // P.15 of [Heitz et al. 2014]
                // "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
                float logSample = std::log(1.0f - U1);
                phi = std::atan(alphay_ / alphax_ * std::tan(2.0f * Pi * U2 + 0.5f * Pi));
                if (U2 > 0.5) {
                    phi += Pi;
                }

                const float sinPhi = std::sin(phi);
                const float cosPhi = std::cos(phi);
                const float alphax2 = alphax_ * alphax_;
                const float alphay2 = alphay_ * alphay_;
                tan2Theta = -logSample / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
            }

            // Compute normal direction from angle information sampled above
            const float cosTheta = 1.0f / std::sqrt(1.0f + tan2Theta);
            const float sinTheta = std::sqrtf(std::max(0.0f, 1.0f - cosTheta * cosTheta));
            Vec3 wm(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);

            if (wm.z() < 0.0)
            {
                wm = -wm;
            }

            return wm;
        }
        else {
            // Sample microfacet normals by considering only visible normals
            const bool flip = wo.z() < 0.0f;
            Vec3 wm = sampleBeckmann(sampler, flip ? -wo : wo, alphax_, alphay_);

            if (wm.z() < 0.0f)
            {
                wm = -wm;
            }

            return wm;
        }
    }

    float lambda(const Vec3 &wo) const override {
        using ::erf;

        float cosThetaO = cosTheta(wo);
        float sinThetaO = sinTheta(wo);
        float absTanThetaO = std::abs(tanTheta(wo));
        if (std::isinf(absTanThetaO)) return 0.;

        float alpha = std::sqrt(cosThetaO * cosThetaO * alphax_ * alphax_ +
            sinThetaO * sinThetaO * alphay_ * alphay_);
        float a = 1.0f / (alpha * absTanThetaO);
        return (erf(a) - 1.0f) / 2.0f + std::exp(-a * a) / (2.0f * a * std::sqrt(Pi) + 1.0e-6f);
    }

    // Sample microfacet normal through slope distribution
    static Vec3 sampleBeckmann(Sampler* sampler, const Vec3 &wi, float alphax, float alphay) {
        // 1. stretch wi
        Vec3 wiStretched = Vec3(alphax * wi.x(), alphay * wi.y(), wi.z()).normalized();

        // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
        float slopex, slopey;
        sampleBeckman_P22_11(sampler, cosTheta(wiStretched), &slopex, &slopey);

        // 3. rotate
        float tmp = cosPhi(wiStretched) * slopex - sinPhi(wiStretched) * slopey;
        slopey = sinPhi(wiStretched) * slopex + cosPhi(wiStretched) * slopey;
        slopex = tmp;

        // 4. unstretch
        slopex = alphax * slopex;
        slopey = alphay * slopey;

        // 5. compute normal
        return Vec3(-slopex, -slopey, 1.0).normalized();
    }

    // Sample slope distribution with alphax and alphay equal to one
    static void sampleBeckman_P22_11(Sampler* sampler, float cosThetaI, float *slopex, float *slopey)
    {
        using ::erf;
        const Vec2 uv = sampler->get2d();
        const float U1 = uv.x();
        const float U2 = uv.y();

        // The special case where the ray comes from normal direction
        // The following sampling is equivarent to the sampling of
        // microfacet normals (not slopes) on isotropic rough surface
        if (cosThetaI > 0.9999) {
            float r = std::sqrt(-std::log(1.0f - U1));
            float sinPhi = std::sin(2 * Pi * U2);
            float cosPhi = std::cos(2 * Pi * U2);
            *slopex = r * cosPhi;
            *slopey = r * sinPhi;
            return;
        }

        float sinThetaI = std::sqrt(std::max((float)0, (float)1 - cosThetaI * cosThetaI));
        float tanThetaI = sinThetaI / cosThetaI;
        float cotThetaI = 1 / tanThetaI;
        const float erfCotThetaI = erf(cotThetaI);

        // Initialize lower and higher bounds for bisection method
        float lower = -1.0;
        float higher = 1.0;

        // Normalization factor for the CDF
        // This is equivarent to (G1 / 2)^{-1}
        static const float SQRT_PI_INV = 1.0f / std::sqrt(Pi);
        const float normalization = 1.0f / (1.0f + erfCotThetaI + SQRT_PI_INV * tanThetaI * std::exp(-cotThetaI * cotThetaI));

        // The following bisection method acquires "erf(x_m)"
        int it = 0;
        float samplex = std::max(U1, 1e-6f);
        while (std::abs(higher - lower) > 1.0e-6f) {
            // Bisection
            float mid = 0.5f * (lower + higher);

            // Evaluation for current estimation
            const float x_m = erfinv(mid);
            const float value = normalization * (1.0f + mid + SQRT_PI_INV * tanThetaI * std::exp(-x_m * x_m)) - samplex;

            /* Update bisection intervals */
            if (value > 0.0f)
            {
                higher = mid;
            }
            else {
                lower = mid;
            }
        }

        // Compute slopex from erf(x_m) given by above bisection method
        *slopex = erfinv(lower);

        // Sample y_m
        *slopey = erfinv(2.0f * std::max(U2, 1e-6f) - 1.0f);
    }
};


// ----------------------------------------------------------------------------
// GGX distribution
// ----------------------------------------------------------------------------
class GGXDistribution : public MicrofacetDistribution {
public:
    GGXDistribution(float alphax, float alphay, bool sampleVisible)
        : MicrofacetDistribution{ alphax, alphay, sampleVisible } {
    }

    Vec3 sampleWm(Sampler* sampler, const Vec3& wi) const override {
        const Vec2 uv = sampler->get2d();
        const float U1 = uv.x();
        const float U2 = uv.y();

        if (!sampleVisible_) {
            float tan2Theta, phi;
            if (alphax_ == alphay_) {
                // Isotropic case
                const float alpha = alphax_;
                tan2Theta = alpha * alpha * U1 / (1.0f - U1);
                phi = 2.0f * Pi * U2;
            }
            else {
                // Anisotropic case
                phi = std::atan(alphay_ / alphax_ * std::tan(2.0f * Pi * U2 + 0.5f * Pi));
                if (U2 > 0.5f) {
                    phi += Pi;
                }

                float sinPhi = std::sin(phi);
                float cosPhi = std::cos(phi);
                float alphax2 = alphax_ * alphax_;
                float alphay2 = alphay_ * alphay_;
                float alpha2 = 1.0f / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
                tan2Theta = U1 / (1.0f - U1) * alpha2;
            }

            // Compute normal direction from angle information sampled above
            const float cosTheta = 1.0f / std::sqrtf(1.0f + tan2Theta);
            const float sinTheta = std::sqrtf(std::max(0.0f, 1.0f - cosTheta * cosTheta));
            Vec3 wm = Vec3(sinTheta * std::cosf(phi), sinTheta * std::sinf(phi), cosTheta);

            if (wm.z() < 0.0) {
                wm = -wm;
            }

            return wm;
        }
        else {
            // Sample microfacet normals by considering only visible normals
            bool flip = wi.z() < 0.0;
            Vec3 wm = sampleGGX(sampler, flip ? -wi : wi, alphax_, alphay_);

            if (wm.z() < 0.0) {
                wm = -wm;
            }

            return wm;
        }
    }

    static Vec3 sampleGGX(Sampler* sampler, const Vec3& wi, float alphax, float alphay)
    {
        // 1. stretch wi
        Vec3 wiStretched = Vec3(alphax * wi.x(), alphay * wi.y(), wi.z()).normalize();

        // 2. simulate P22_{wi}(x_slope, y_slope, 1, 1)
        float slopex, slopey;
        sampleGGX_P22_11(sampler, cosTheta(wiStretched), &slopex, &slopey);

        // 3. rotate
        float tmp = cosPhi(wiStretched) * slopex - sinPhi(wiStretched) * slopey;
        slopey = sinPhi(wiStretched) * slopex + cosPhi(wiStretched) * slopey;
        slopex = tmp;

        // 4. unstretch
        slopex = alphax * slopex;
        slopey = alphay * slopey;

        // 5. compute normal
        return Vec3(-slopex, -slopey, 1.0).normalize();
    }

    static void sampleGGX_P22_11(Sampler* sampler, float cosThetaI, float *slopex, float *slopey) {
        const Vec2 uv = sampler->get2d();
        const float U1 = uv.x();
        float U2 = uv.y();

        // The special case where the ray comes from normal direction
        // The following sampling is equivarent to the sampling of
        // microfacet normals (not slopes) on isotropic rough surface
        if (cosThetaI > 0.9999f) {
            const float r = std::sqrt(U1 / (1.0f - U1));
            const float sinPhi = std::sinf(2 * Pi * U2);
            const float cosPhi = std::cosf(2 * Pi * U2);
            *slopex = r * cosPhi;
            *slopey = r * sinPhi;
            return;
        }

        const float sinThetaI = std::sqrt(std::max(0.0f, 1.0f - cosThetaI * cosThetaI));
        const float tanThetaI = sinThetaI / cosThetaI;
        const float a = 1.0f / tanThetaI;
        const float G1 = 2.0f / (1.0f + std::sqrt(1.0f + 1.0f / (a * a)));

        // Sample slopex
        const float A = 2.0f * U1 / G1 - 1.0f;
        const float B = tanThetaI;
        const float tmp = std::min(1.0f / (A * A - 1.0f), 1.0e12f);

        const float D = std::sqrt(B * B * tmp * tmp - (A * A - B * B) * tmp);
        const float slopex1 = B * tmp - D;
        const float slopex2 = B * tmp + D;
        *slopex = (A < 0.0f || slopex2 > 1.0f / tanThetaI) ? slopex1 : slopex2;

        // Sample slopey
        float S;
        if (U2 > 0.5f) {
            S = 1.0f;
            U2 = 2.0f * (U2 - 0.5f);
        }
        else {
            S = -1.0f;
            U2 = 2.0f * (0.5f - U2);
        }

        const float z = (U2 * (U2 * (U2 * 0.27385f - 0.73369f) + 0.46341f)) /
            (U2 * (U2 * (U2 * 0.093073f + 0.309420f) - 1.000000f) + 0.597999f);
        *slopey = S * z * std::sqrt(1.0f + (*slopex) * (*slopex));
    }

    float lambda(const Vec3 &wo) const override {
        float absTanThetaO = std::abs(tanTheta(wo));
        if (std::isinf(absTanThetaO)) return 0.;

        const float alpha =
            std::sqrtf(cos2Phi(wo) * alphax_ * alphax_ + sin2Phi(wo) * alphay_ * alphay_);
        //
        if (std::isinf(alpha))
        {
            return 0.0f;
        }
        const float alpha2Tan2Theta = (alpha * absTanThetaO) * (alpha * absTanThetaO);
        return (-1.0f + std::sqrtf(1.0f + alpha2Tan2Theta)) / 2.0f;
    }
};


void test2()
{
    //
    const float alphaX = 1.0f;
    const float alphaY = 1.0f;
    GGXDistribution ggx0(alphaX, alphaY, true);
    GGXDistribution ggx1(alphaX, alphaY, false);
    FloatStreamStats<float,FSS_MomentLevel::MuVar,true> fs0, fs1;
    SamplerHalton sampler;
    sampler.setHash(0x123);
    //
    for (int32_t sn=0;sn<1024*1024;++sn)
    {
        sampler.startSample(sn);
        //const Vec3 wiLocal = sampler.getHemisphere();
        const Vec3 wiLocal = Vec3(1.0,0.0f,0.01f).normalized();
        //
        {
            const Vec3 wm = ggx0.sampleWm(&sampler, wiLocal);
            const Vec3 woLocal = -wm.reflect(wiLocal);
            const float w = ggx0.weight(woLocal, wiLocal, wm);
            fs0.add(w);
        }
        //
        {
            const Vec3 wm = ggx1.sampleWm(&sampler, wiLocal);
            const Vec3 woLocal = -wm.reflect(wiLocal);
            const float w = ggx1.weight(woLocal, wiLocal, wm);
            fs1.add(w);
        }
    }
    printf("mu:%f sig:%f min:%f max:%f\n", fs0.mu(), fs0.sigma(), fs0.min(), fs0.max() );
    printf("mu:%f sig:%f min:%f max:%f\n", fs1.mu(), fs1.sigma(), fs1.min(), fs1.max() );
}

/*
-------------------------------------------------
main
-------------------------------------------------
*/
int32_t main(int32_t argc, char* argv[])
{
    test2();
    return 0;
    //
    Alhazen app;
    return app.run(argc, argv);
}
