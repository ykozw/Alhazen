#pragma once
#include "pch.hpp"
#include "math.hpp"

// 可視光の範囲
const float SPECTRUM_LAMBDA_START = 400.0f;
const float SPECTRUM_LAMBDA_LAST = 700.0f;
const float SPECTRUM_LAMBDA_RANGE =
  SPECTRUM_LAMBDA_LAST - SPECTRUM_LAMBDA_START;
// SpectrumSampledクラスでのサンプル数
static const int32_t SPECTRUM_SAMPLED_NUM_SAMPLES = 30;

/*
-------------------------------------------------
-------------------------------------------------
*/
void
xyz2rgb(const std::array<float, 3>& xyz_, std::array<float, 3>& rgb);
void
rgb2xyz(const std::array<float, 3>& rgb, std::array<float, 3>& xyz_);
// Spectrum evalAsSpectrum(const std::string& str, bool asIllumination);

/*
-------------------------------------------------
-------------------------------------------------
*/
class SpectrumRGB AL_FINAL
{
public:
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

public:
    static SpectrumRGB Black;
    static SpectrumRGB White;

public:
    static SpectrumRGB createFromRGB(const std::array<float, 3>& rgb,
                                     bool asIllumination);
    static SpectrumRGB createFromColorTemp(const float kelvin);
    static float energyDif(const SpectrumRGB& lhs, const SpectrumRGB& rhs);
    SpectrumRGB() = default;
    SpectrumRGB(const SpectrumRGB& other) = default;
    SpectrumRGB(SpectrumRGB&& other) noexcept = default;
    explicit SpectrumRGB(float v);
    explicit SpectrumRGB(float r, float g, float b);
    void clear();
    bool isBlack() const;
    SpectrumRGB sqrt() const;
    float luminance() const;
    void toRGB(SpectrumRGB& rgb) const;
    void clamp(float mn, float mx);
    bool hasNaN() const;
    //
    SpectrumRGB& operator=(const SpectrumRGB& other);
};
SpectrumRGB
operator+(const SpectrumRGB& lhs, const SpectrumRGB& rhs);
SpectrumRGB
operator-(const SpectrumRGB& lhs, const SpectrumRGB& rhs);
SpectrumRGB operator*(float scale, const SpectrumRGB& spectrum);
SpectrumRGB operator*(const SpectrumRGB& spectrum, float scale);
SpectrumRGB operator*(const SpectrumRGB& lhs, const SpectrumRGB& rhs);
SpectrumRGB
operator/(const SpectrumRGB& spectrum, float v);
SpectrumRGB
operator/(const SpectrumRGB& lhs, const SpectrumRGB& rhs);
bool
operator==(const SpectrumRGB& lhs, const SpectrumRGB& rhs);
SpectrumRGB&
operator+=(SpectrumRGB& lhs, const SpectrumRGB& rhs);
SpectrumRGB&
operator-=(SpectrumRGB& lhs, const SpectrumRGB& rhs);
SpectrumRGB&
operator*=(SpectrumRGB& spectrum, float scale);
SpectrumRGB&
operator*=(SpectrumRGB& lhs, const SpectrumRGB& rhs);
SpectrumRGB&
operator/=(SpectrumRGB& spectrum, float scale);
SpectrumRGB
lerp(const SpectrumRGB& lhs, const SpectrumRGB& rhs, float factor);

/*
-------------------------------------------------
-------------------------------------------------
*/
class SpectrumSampled AL_FINAL
{
public:
    std::array<float, SPECTRUM_SAMPLED_NUM_SAMPLES> samples;

public:
    static SpectrumSampled Black;
    static SpectrumSampled White;
    static SpectrumSampled createFromRGB(const std::array<float, 3>& rgb,
                                         bool asIllumination);
    static SpectrumSampled createFromColorTemp(const float kelvin);
    static float energyDif(const SpectrumSampled& lhs,
                           const SpectrumSampled& rhs);
    SpectrumSampled();
    explicit SpectrumSampled(float v);
    explicit SpectrumSampled(
      const std::array<float, SPECTRUM_SAMPLED_NUM_SAMPLES>& samples);
    void clear();
    bool isBlack() const;
    SpectrumSampled sqrt() const;
    void toXYZ(std::array<float, 3>& xyz_) const;
    void toRGB(SpectrumRGB& rgb) const;
    void clamp(float mn, float mx);
    bool hasNaN() const;
    float maxValue() const;
    float sample(float wavelength) const;
    float accumrate() const;
    float y() const;
};
SpectrumSampled
operator+(const SpectrumSampled& lhs, const SpectrumSampled& rhs);
SpectrumSampled
operator-(const SpectrumSampled& lhs, const SpectrumSampled& rhs);
SpectrumSampled operator*(float scale, const SpectrumSampled& spectrum);
SpectrumSampled operator*(const SpectrumSampled& spectrum, float scale);
SpectrumSampled operator*(const SpectrumSampled& lhs,
                          const SpectrumSampled& rhs);
SpectrumSampled
operator/(const SpectrumSampled& spectrum, float v);
SpectrumSampled
operator/(const SpectrumSampled& lhs, const SpectrumSampled& rhs);
SpectrumSampled&
operator+=(SpectrumSampled& lhs, const SpectrumSampled& rhs);
SpectrumSampled&
operator-=(SpectrumSampled& lhs, const SpectrumSampled& rhs);
SpectrumSampled&
operator*=(SpectrumSampled& spectrum, float scale);
SpectrumSampled
lerp(const SpectrumSampled& lhs, const SpectrumSampled& rhs, float factor);

/*
-------------------------------------------------
-------------------------------------------------
*/
class SpectrumSingleWavelength AL_FINAL
{
public:
    float lambda;
    float intensity;

public:
    SpectrumSingleWavelength();
    SpectrumSingleWavelength(float lambda, float intensity);
    SpectrumRGB toRGB() const;

private:
    // HACK:
    // この関数はtoRGB()と「表色系が違う」だけでなくスケールも違うので外では使わないこと
    Vec3 toXYZ() const;
};

SpectrumSingleWavelength operator*(float scale,
                                   const SpectrumSingleWavelength& spectrum);
SpectrumSingleWavelength operator*(const SpectrumSampled& ss,
                                   const SpectrumSingleWavelength& single);

/*
-------------------------------------------------
-------------------------------------------------
*/
class SpectrumHerowavelength AL_FINAL
{
public:
    // TODO: 通しで完成次第、SIMD化をする
    std::array<float, 4> lambdas;
    std::array<float, 4> intensitys;

public:
    SpectrumHerowavelength();
    SpectrumHerowavelength(std::array<float, 4> lambdas,
                           std::array<float, 4> intensitys);
    SpectrumRGB toRGB() const;
    Vec3 toXYZ() const;
    static SpectrumHerowavelength createFromWavelength(float w0,
                                                       float w1,
                                                       float w2,
                                                       float w3);
    static SpectrumHerowavelength createFromHerowavelength(
      float w0,
      bool isImportanceSamplingXYZ);
    static SpectrumHerowavelength createFromXYZImportanceSampling(float w0);
};
SpectrumHerowavelength operator*(float scale,
                                 const SpectrumHerowavelength& spectrum);
SpectrumHerowavelength operator*(const SpectrumSampled& ss,
                                 const SpectrumHerowavelength& sh);

template<int32_t NUM_SAMPLE>
SpectrumSampled
sample2SpectrumSampled(const std::array<float, NUM_SAMPLE>& lambdas,
                       const std::array<float, NUM_SAMPLE>& intensity);

//
// typedef SpectrumSampled Spectrum;
typedef SpectrumRGB Spectrum;

#include "core/spectrumsampled.inl"
#include "core/spectrumsinglewavelength.inl"
#include "core/spectrumherowavelength.inl"
#include "core/spectrumrgb.inl"
