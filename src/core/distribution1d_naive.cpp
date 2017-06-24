#include "pch.hpp"
#include "core/distribution1d.hpp"
#include "core/math.hpp"
#include "core/logging.hpp"
#include "core/rng.hpp"
#include "core/unittest.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive()
{
    construct({ {} });
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive(const std::vector<float>& values)
{
    construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive(const std::initializer_list<float>& values)
{
    construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive(const float* values, int32_t num)
{
    construct(std::vector<float>(values, values + num));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive(std::function<float(float)> genFunc, int32_t split)
{
    std::vector<float> values;
    values.resize(split);
    for (int32_t i = 0; i < split; ++i)
    {
        const float samplePoint = (0.5f + static_cast<float>(i)) / static_cast<float>(split);
        values[i] = genFunc(samplePoint);
    }
    construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Distribution1D_Naive::construct(const std::vector<float>& values)
{
    uint32_t cdfSize = (uint32_t)values.size();
    if (!alIsPowerOfTwo(cdfSize + 1))
    {
        const uint32_t x = (uint32_t)cdfSize;
        const uint32_t y = (x - ((x & (~x + 1)))) << 1;
        cdfSize = y;
    }
    //
    cdf_.resize(values.size() + 1);
    cdf_[0] = 0.0f;
    for (int32_t i = 0; i < values.size(); ++i)
    {
        AL_ASSERT_DEBUG(values[i] >= 0.0f);
        AL_ASSERT_DEBUG(!isnan(values[i]));
        cdf_[i + 1] = cdf_[i] + values[i];
    }
    if (cdf_.back() == 0.0f)
    {
        cdf_.back() = 1.0f;
    }
    const float scale = 1.0f / cdf_.back();
    for (auto& v : cdf_)
    {
        v *= scale;
    }
    numValue_ = (float)(values.size());
    invNumValue_ = 1.0f / numValue_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution1D_Naive::sample(float u, _Out_ float* pdf, _Out_ int32_t* offset) const
{
    const auto upperIte = std::lower_bound(cdf_.begin(), cdf_.end(), u);
    const int32_t index = alMax((int32_t)(upperIte - cdf_.begin() - 1), 0);
    //
    if (offset)
    {
        *offset = index;
    }
    const float t = (u - cdf_[index]) / (cdf_[index + 1] - cdf_[index]);
    *pdf = (cdf_[index + 1] - cdf_[index]) * numValue_;
    const float samplePoint = ((float)index + t) * invNumValue_;
    return samplePoint;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution1D_Naive::pdf(int32_t index) const
{
    return (cdf_[index + 1] - cdf_[index]) * numValue_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(Distribution, OneDimention)
{
    // 推定が正しいかチェック
    {
        XorShift128 rng;
        const std::vector<float> samples = { { 0.1f, 0.4f } };
        Distribution1D distribution1d(samples);
        const int32_t NUM_SAMPLE = 1000;
        float total = 0.0f;
        for (int32_t i = 0; i < NUM_SAMPLE + 1; ++i)
        {
            float pdf;
            const float samplePoint = distribution1d.sample(rng.nextFloat(), &pdf);
            const float value = alLerp(samples[0], samples[1], samplePoint);
            total += value / pdf;
        }
        total /= (float)NUM_SAMPLE;
        if (fabsf(total / 0.25f - 1.0f) > 0.01f)
        {
            return;
        }
    }
    return;
}
