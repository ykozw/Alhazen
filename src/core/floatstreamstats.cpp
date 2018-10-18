#include "core/floatstreamstats.hpp"

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

