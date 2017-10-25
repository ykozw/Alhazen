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
Distribution1D_Optimized::Distribution1D_Optimized()
{
    construct({ {} });
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Optimized::Distribution1D_Optimized(const std::vector<float>& values)
{
    construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Optimized::Distribution1D_Optimized(const std::initializer_list<float>& values)
{
    construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Optimized::Distribution1D_Optimized(const float* values, int32_t num)
{
    construct(std::vector<float>(values, values + num));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Optimized::Distribution1D_Optimized(std::function<float(float)> genFunc, int32_t split)
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
static uint32_t roundupPowerOf2(uint32_t v)
{
    --v;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    return v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Distribution1D_Optimized::construct(const std::vector<float>& values)
{
    // CDFを2^n-1の形にしておくと、後段のオフセット計算が簡単になる
    const uint32_t cdfSize = roundupPowerOf2((uint32_t)(values.size() + 1));
    //
    // 昇順にソートされたCDF
    std::vector<float> cdf;
    cdf.resize(cdfSize);
    cdf[0] = 0.0f;
    for (int32_t i = 0; i < values.size(); ++i)
    {
        AL_ASSERT_DEBUG(values[i] >= 0.0f);
        AL_ASSERT_DEBUG(!isnan(values[i]));
        cdf[i + 1] = cdf[i] + values[i];
    }
    // valueで加算が行われたよりも後ろは全て同じ値にしておく
    for (uint32_t i = (uint32_t)values.size() + 1; i < cdfSize; ++i)
    {
        cdf[i] = cdf[values.size()];
    }
    //
    if (cdf.back() == 0.0f)
    {
        cdf.back() = 1.0f;
    }
    const float scale = 1.0f / cdf.back();
    for (auto& v : cdf)
    {
        v *= scale;
    }
    numValue_ = (float)(values.size());
    invNumValue_ = 1.0f / numValue_;
    cdfBreadthFirst_.resize(cdf.size());
    //
    class Local
    {
    public:
        static void insert(size_t i, size_t n, std::vector<float>::const_iterator first, std::vector<float>& cdfBreadthFirst)
        {
            auto root = [](size_t n) -> size_t
            {
                if (n <= 1)return 0;
                size_t i = 2;
                while (i <= n)
                {
                    i *= 2;
                }
                return std::min(i / 2 - 1, n - i / 4);
            };
            if (n)
            {
                const size_t h = root(n);
                cdfBreadthFirst[i] = *(first + h);
                insert(2 * i + 1, h, first, cdfBreadthFirst);
                insert(2 * i + 2, n - h - 1, first + h + 1, cdfBreadthFirst);
            }
        };
    };
    // CDFを幅優先にして置き換えたものも生成
    Local::insert(0, cdf.size() - 1, cdf.begin() + 1, cdfBreadthFirst_); // TODO:ここの-1があるせいで最後が0になってるのを直す
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution1D_Optimized::sample(float u, _Out_ float* pdf, _Out_ int32_t* aOffset) const
{
    //
    uint32_t n = (uint32_t)cdfBreadthFirst_.size() - 1;
    uint32_t j = 0;
    uint32_t offset = 0;
    uint32_t left = -1;
    uint32_t right = 0;
    float v0;
    float v1;
    for (;;)
    {
        offset <<= 1;
        //
        if (cdfBreadthFirst_[j] < u)
        {
            //
            offset |= 1;
            //
            left = j;
            const uint32_t prej = j;
            j = 2 * j + 2;
            //
            if (j >= n)
            {
                v0 = cdfBreadthFirst_[prej];
                v1 = cdfBreadthFirst_[right];
                break;
            }
        }
        else
        {
            //
            right = j;
            const uint32_t prej = j;
            j = 2 * j + 1;
            //
            if (j >= n)
            {
                v0 = (left == -1) ? 0.0f : cdfBreadthFirst_[left];
                v1 = cdfBreadthFirst_[prej];
                break;
            }
        }
    }
    //
    *pdf = (v1 - v0) * numValue_;
    const float t = (u - v0) / (v1 - v0);
    const float samplePoint = ((float)offset + t) * invNumValue_;
    //
    if (aOffset)
    {
        *aOffset = offset;
    }
    return samplePoint;

#if 0 // テスト
    const auto ite = std::lower_bound(cdf_.begin(), cdf_.end(), u);
    const int32_t indexCheck = alMax((int32_t)(ite - cdf_.begin() - 1), 0);
    AL_ASSERT_DEBUG(offset == indexCheck);
    const float t2 = (u - cdf_[indexCheck]) / (cdf_[indexCheck + 1] - cdf_[indexCheck]);
    const float pdf2 = (cdf_[indexCheck + 1] - cdf_[indexCheck]) * numValue_;
    const float samplePoint2 = ((float)indexCheck + t2) * invNumValue_;
    return samplePoint2;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution1D_Optimized::pdf(int32_t index) const
{
    // TODO: 本来のindexから専用cdf用のindexを逆算する
    // TODO: 実装
    AL_ASSERT_DEBUG(false);
    return 0.0f;
}

/*
-------------------------------------------------
TestNaiveAndOptimized
Distribution1D_Optimizedが、Naive版と同じ挙動になっているかのチェック
-------------------------------------------------
*/
AL_TEST(Distribution, Optimized)
{
    // データの作成
    std::vector<float> datas;
    std::vector<float> samples;
    RNG rng;
    for (int32_t i = 0; i < 57; ++i)
    {
        const float v = (float)rand() / (float)RAND_MAX * 100;
        datas.push_back(v);
    }
    for (int32_t i = 0; i < 1000; ++i)
    {
        samples.push_back(rng.nextFloat());
    }
    //
    Distribution1D_Naive naive(datas);
    Distribution1D_Optimized optimized(datas);
    //
    for (size_t i = 0, n = samples.size(); i < n; ++i)
    {
#if 0
        float pdf0;
        float pdf1;
        volatile float v0 = naive.sample(samples[i], &pdf0);
        volatile float v1 = optimized.sample(samples[i], &pdf1);
        AL_ASSERT_DEBUG(v0 == v1);
        AL_ASSERT_DEBUG(pdf0 == pdf1);
#endif
    }
    return;
}
