#include "pch.hpp"
#include "core/floatstreamstats.hpp"
#include "core/unittest.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
void FloatStreamStats::add(float v)
{
    //
    n_ += 1.0f;
    //
    const float newMean = (v - mean_) / n_ + mean_;
    const float newM = (v - mean_) * (v - newMean) + M_;
    //
    M_ = newM;
    mean_ = newMean;
    //
    min_ = std::min(v, min_);
    max_ = std::max(v, max_);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t FloatStreamStats::size() const { return int32_t(n_); }

/*
-------------------------------------------------
-------------------------------------------------
*/
float FloatStreamStats::mean() const { return mean_; }

/*
-------------------------------------------------
標本不偏分散
-------------------------------------------------
*/
float FloatStreamStats::variance() const { return M_ / (n_ - 1.0f); }

/*
-------------------------------------------------
標本不偏標準偏差
-------------------------------------------------
*/
float FloatStreamStats::sigma() const { return sqrtf(variance()); }

/*
-------------------------------------------------
-------------------------------------------------
*/
float FloatStreamStats::max() const { return max_; }

/*
-------------------------------------------------
-------------------------------------------------
*/
float FloatStreamStats::min() const { return min_; }

/*
-------------------------------------------------
自由度無限(サンプル数が十分)、信頼係数0.95のときの平均値の信頼区間
参照: 統計学入門 11.5.1
-------------------------------------------------
*/
std::tuple<float, float> FloatStreamStats::meanConfidenceInterval() const
{
    // 自由度無限のt分布の両側5パーセント点
    // TOOD: 自由度と信頼係数を外部から指定できるようにする
    const float t = 1.960f;
    const float d = t * sigma() / std::sqrtf(float(size()));
    const float m = mean();
    return std::tuple<float, float>(m - d, m + d);
}

/*
-------------------------------------------------
恐らく真値は同じであることの確認
-------------------------------------------------
*/
bool FloatStreamStats::maybeSameMean(const FloatStreamStats &lhs,
                                     const FloatStreamStats &rhs)
{
    auto i0 = lhs.meanConfidenceInterval();
    auto i1 = rhs.meanConfidenceInterval();
    const float i0l = std::get<0>(i0);
    const float i0h = std::get<1>(i0);
    const float i1l = std::get<0>(i1);
    const float i1h = std::get<1>(i1);
    // 領域が重ならない場合は真値は違う判定
    return !((i0h < i1l) || (i1h < i0l));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void FloatStreamStats2::add(float v0, float v1)
{
    //　TODO: 検算とコードの整理
    const float nm0 = (v0 - means_[0]) / float(n_ + 1.0f) + means_[0];
    const float nm1 = (v1 - means_[1]) / float(n_ + 1.0f) + means_[1];
    const float covDif =
        ((v0 - nm0) * (v1 - nm1) +
         ((nm0 * nm1 - means_[0] * means_[1]) + (means_[1] - nm1) * means_[0] +
          (means_[0] - nm0) * means_[1]) *
             n_) /
            float(n_ + 1.0f) +
        cov_ * float(n_) / float(n_ + 1.0f);
    cov_ = (int32_t(n_) >= 1) ? covDif : 0;
    means_[0] = nm0;
    means_[1] = nm1;

    // TODO: varianceを計算する
}

/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t FloatStreamStats2::size() const { return int32_t(n_); }

/*
-------------------------------------------------
-------------------------------------------------
*/
FloatStreamStats2::float2 FloatStreamStats2::mean() const
{
    return {means_[0], means_[1]};
}

/*
-------------------------------------------------
-------------------------------------------------
*/
FloatStreamStats2::float2 FloatStreamStats2::variance() const
{
    return {Ms_[0] / n_, Ms_[1] / n_};
}

/*
-------------------------------------------------
-------------------------------------------------
*/
FloatStreamStats2::float2 FloatStreamStats2::sigma() const
{
    // TODO: 実装
    return {0.0f, 0.0f};
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float FloatStreamStats2::cov() const { return cov_; }

/*
-------------------------------------------------
-------------------------------------------------
*/
FloatStreamStatsEx::FloatStreamStatsEx(int32_t sampleSize)
{
    samples_.resize(sampleSize);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void FloatStreamStatsEx::add(float v)
{
    FloatStreamStats::add(v);
    // TODO: このあたりの実装はちゃんとテストする必要がある
    // 充填されるまでは無条件に入れる
    if (n_ < samples_.size())
    {
        samples_[int32_t(n_)] = v;
    }
    // 充填されたら既定の確率以下にならないと入らないようになる
    else
    {
        const float idx = rng_.nextFloat() * n_;
        if (idx < float(samples_.size()))
        {
            samples_[int32_t(idx)] = v;
        }
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(testFloatStreamStats, 0)
{
    {
        FloatStreamStats stats;
        stats.add(13.0f);
        stats.add(23.0f);
        stats.add(12.0f);
        stats.add(44.0f);
        stats.add(55.0f);
        AL_ASSERT_ALWAYS(fabsf(stats.mean() / 29.4f - 1.0f) < 0.1f);
        AL_ASSERT_ALWAYS(fabsf(stats.variance() / 370.3f - 1.0f) < 0.1f);
        AL_ASSERT_ALWAYS(fabsf(stats.sigma() / 19.2f - 1.0f) < 0.1f);
    }
}
