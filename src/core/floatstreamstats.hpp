#pragma once

#include "core/rng.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
enum class FSS_MomentLevel
{
    // 平均
    Mu,
    // 平均+分散
    MuVar,
    // 平均+分散+歪度
    MuVarSkew,
    // 平均+分散+歪度+尖度
    MuVarSkewKurt
};

/*
-------------------------------------------------
FloatStreamStats
-------------------------------------------------
*/
template<typename Float = float, FSS_MomentLevel ML = FSS_MomentLevel::MuVar, bool ENABLE_MINMAX = false>
class FloatStreamStats
{
public:
    void add(Float v)
    {
        //
        ++n_;
        // モーメントの更新
        updateMoment<ML>(v);
        // 最大最高の更新
        updateMinMax<ENABLE_MINMAX>(v);
        // 
    }
    // 相加平均
    Float mu() const
    {
        return mb_.m0;
    }
    // 不偏分散
    Float var() const
    {
        return mb_.m1 / Float(n_ - 1);
    }
    // 不偏標準偏差
    Float sigma() const
    {
        return std::sqrt(var());
    }
    Float skew() const
    {
        return std::sqrt(Float(n_)) * mb_.m3 / std::pow(mb_.m2, 1.5f);
    }
    Float kurt() const
    {
        return (float(n_) * m4_) / (m2_ * m2_);
    }
    Float min() const
    {
        return mmb_.mn;
    }
    Float max() const
    {
        return mmb_.mx;
    }

private:
    // 最大最高の更新
    template<bool ENABLE_MINMAX>
    typename std::enable_if<ENABLE_MINMAX>::type updateMinMax(Float v)
    {
        mmb_.mn = std::min(v, mmb_.mn);
        mmb_.mx = std::max(v, mmb_.mx);
    }
    template<bool ENABLE_MINMAX>
    typename std::enable_if<!ENABLE_MINMAX>::type updateMinMax(Float v)
    {}
    /*
    モーメントの更新
    Simpler Online Updates for Arbitrary-Order Central Moments
    https://arxiv.org/pdf/1510.04923.pdf
    */
    template<FSS_MomentLevel ML>
    typename std::enable_if<ML == FSS_MomentLevel::Mu>::type updateMoment(Float v)
    {
        mb_.m0 = (v - mb_.m0) / Float(n_) + mb_.m0;
    }
    template<FSS_MomentLevel ML>
    typename std::enable_if<ML == FSS_MomentLevel::MuVar>::type updateMoment(Float v)
    {
        const float nmu = (v - mb_.m0) / Float(n_) + mb_.m0;
        const float newM = (v - mb_.m0) * (v - nmu) + mb_.m1;
        mb_.m1 = newM;
        mb_.m0 = nmu;
    }
    template<FSS_MomentLevel ML>
    typename std::enable_if<ML == FSS_MomentLevel::MuVarSkew>::type updateMoment(Float v)
    {
        const Float delta = v - mb_.m0;
        const Float delta_n = delta / Float(n_);
        mb_.m0 += delta_n;
        mb_.m1 += delta * (delta - delta_n);
        const Float delta_2 = delta * delta;
        const Float delta_n_2 = delta_n * delta_n;
        mb_.m2 += Float(-3.0) * delta_n * mb_.m2 + delta * (delta_2 - delta_n_2);
    }
    template<FSS_MomentLevel ML>
    typename std::enable_if<ML == FSS_MomentLevel::MuVarSkewKurt>::type updateMoment(Float v)
    {
        const Float delta = v - mb_.m0;
        const Float delta_n = delta / Float(n_);
        mb_.m0 += delta_n;
        mb_.m1 += delta * (delta - delta_n);
        const Float delta_2 = delta * delta;
        const Float delta_n_2 = delta_n * delta_n;
        mb_.m2 += Float(-3.0) * delta_n * mb_.m2 + delta * (delta_2 - delta_n_2);
        mb_.m3 += Float(-4.0) * delta_n * mb_.m3 - Float(6.0) * delta_n_2 * mb_.m2 + delta * (delta * delta_2 - delta_n * delta_n_2);
    }
private:
    //
    int32_t n_ = 0;

    // モーメント用のバッファ
    template<FSS_MomentLevel T>
    struct MomentBuffer {};
    template<>
    struct MomentBuffer<FSS_MomentLevel::Mu>
    {
        MomentBuffer<FSS_MomentLevel::Mu>()
            : m0(0.0f)
        {}
        Float m0 = 0.0f;
    };
    template<>
    struct MomentBuffer<FSS_MomentLevel::MuVar>
    {
        MomentBuffer<FSS_MomentLevel::MuVar>()
            : m0(0.0f), m1(0.0f)
        {}
        Float m0;
        Float m1;
    };
    template<>
    struct MomentBuffer<FSS_MomentLevel::MuVarSkew>
    {
        MomentBuffer<FSS_MomentLevel::MuVarSkew>()
            : m0(0.0f), m1(0.0f), m2(0.0f)
        {}
        Float m0;
        Float m1;
        Float m2;
    };
    template<>
    struct MomentBuffer<FSS_MomentLevel::MuVarSkewKurt>
    {
        MomentBuffer<FSS_MomentLevel::MuVarSkewKurt>()
            : m0(0.0f), m1(0.0f), m2(0.0f), m3(0.0f)
        {}
        Float m0;
        Float m1;
        Float m2;
        Float m3;
    };
    MomentBuffer<ML> mb_;

    // 最大最小用のバッファ
    template<bool T>
    struct MinMaxBuffer {};
    template<>
    struct MinMaxBuffer<true>
    {
        Float mn = std::numeric_limits<Float>::max();
        Float mx = std::numeric_limits<Float>::lowest();
    };
    MinMaxBuffer<ENABLE_MINMAX> mmb_;
};

/*
-------------------------------------------------
 https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online
-------------------------------------------------
*/
class FloatStreamStats2 AL_FINAL
{
public:
    FloatStreamStats2() = default;
    void add(float x, float y);
    int32_t size() const { return n_; }
    float varX() const { return mx_ / (n_ - 1.0f); }
    float varY() const { return my_ / (n_ - 1.0f); }
    float sigmaX() const { return std::sqrtf(varX()); }
    float sigmaY() const { return std::sqrtf(varY()); }
    float cov() const { return C_ / float(n_ - 1); }
    float R2() const { return cov()/(sigmaX()*sigmaY()); }
protected:
    float mux_ = 0.0f;
    float muy_ = 0.0f;
    float mx_ = 0.0f;
    float my_ = 0.0f;
    float C_ = 0.0f;
    int32_t n_ = 0;
};

#if 0
/*
-------------------------------------------------
TODO: Reservoir Samplingも行う。P2も行うようにする。
-------------------------------------------------
*/
class FloatStreamStatsEx : public FloatStreamStats
{
public:
    FloatStreamStatsEx(int32_t sampleSize);
    virtual void add(float v) override;

private:
    std::vector<float> samples_;
    RNG rng_;
};
#endif