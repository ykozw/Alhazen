#include "core/floatstreamstats.hpp"
#include "core/unittest.hpp"

#if 0 // TODO: 以下を移植する
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
bool FloatStreamStats::maybeSameMean(const FloatStreamStats& lhs,
                                     const FloatStreamStats& rhs)
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
#endif

/*
-------------------------------------------------
-------------------------------------------------
*/
void FloatStreamStats2::add(float x, float y)
{
    n_ += 1;
    const float invn = 1.0f / float(n_);
    const float dx = x - mux_;
    //
    const float oldmux = mux_;
    const float oldmuy = muy_;
    mux_ += dx * invn;
    muy_ += (y - muy_) * invn;
    C_ += dx * (y - muy_);
    //
    mx_ = (x - oldmux) * (x - mux_) + mx_;
    my_ = (y - oldmuy) * (y - muy_) + my_;
}
#if 0
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
#endif

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(testFloatStreamStats, 0)
{
    {
        FloatStreamStats<> stats;
        stats.add(13.0f);
        stats.add(23.0f);
        stats.add(12.0f);
        stats.add(44.0f);
        stats.add(55.0f);
        AL_ASSERT_ALWAYS(fabsf(stats.mu() / 29.4f - 1.0f) < 0.1f);
        AL_ASSERT_ALWAYS(fabsf(stats.var() / 370.3f - 1.0f) < 0.1f);
        AL_ASSERT_ALWAYS(fabsf(stats.sigma() / 19.2f - 1.0f) < 0.1f);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(testFloatStreamStats, 1)
{
    FloatStreamStats<float, FSS_MomentLevel::MuVarSkewKurt> stats;
    stats.add(2.0f);
    stats.add(30.0f);
    stats.add(51.0f);
    stats.add(72.0f);
    const float t0 = stats.mu();
    const float t1 = stats.var();
    const float t2 = stats.skew();
    const float t3 = stats.kurt();

    AL_ASSERT_ALWAYS(fabsf((t0 / +38.75f) - 1.0f) < 0.1f);
    AL_ASSERT_ALWAYS(fabsf((t1 / +894.25f) - 1.0f) < 0.1f);
    AL_ASSERT_ALWAYS(fabsf((t2 / -0.1685f) - 1.0f) < 0.1f);
    AL_ASSERT_ALWAYS(fabsf((t3 / -1.29f) - 1.0f) < 0.1f);

}
   