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
    FloatStreamStats() = default;
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
        return mb_.m1;
    }
    // 不偏分散
    Float var() const
    {
        return mb_.m2 / Float(n_ - 1);
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
        return (float(n_) * mb_.m4) / (mb_.m2 * mb_.m2) - 3.0f;
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
        const Float delta = v - mb_.m1;
        const Float delta_n = delta / Float(n_);
        mb_.m1 += delta_n;
    }
    template<FSS_MomentLevel ML>
    typename std::enable_if<ML == FSS_MomentLevel::MuVar>::type updateMoment(Float v)
    {
        const Float delta = v - mb_.m1;
        const Float delta_n = delta / Float(n_);
        const Float t = delta * delta_n * Float(n_ - 1);
        mb_.m2 += t;
        mb_.m1 += delta_n;
    }
    template<FSS_MomentLevel ML>
    typename std::enable_if<ML == FSS_MomentLevel::MuVarSkew>::type updateMoment(Float v)
    {
        const Float delta = v - mb_.m1;
        const Float delta_n = delta / Float(n_);
        const Float t = delta * delta_n * Float(n_ - 1);
        mb_.m3 += Float(-3.0) * delta_n * mb_.m2 + t * delta_n * (Float(n_) - Float(2.0));
        mb_.m2 += t;
        mb_.m1 += delta_n;
    }
    template<FSS_MomentLevel ML>
    typename std::enable_if<ML == FSS_MomentLevel::MuVarSkewKurt>::type updateMoment(Float v)
    {
        const Float delta = v - mb_.m1;
        const Float delta_n = delta / Float(n_);
        const Float delta_n2 = delta_n * delta_n;
        const Float t = delta * delta_n * Float(n_-1);
        mb_.m4 +=
            t * delta_n2 * (Float(n_)*Float(n_) - Float(3.0) * Float(n_) + Float(3.0))
            + Float(6.0) * delta_n2 * Float(mb_.m2)
            - Float(4.0) * delta_n * Float(mb_.m3);
        mb_.m3 += Float(-3.0) * delta_n * mb_.m2 + t * delta_n * (Float(n_) - Float(2.0));
        mb_.m2 += t;
        mb_.m1 += delta_n;
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
            : m1(0.0f)
        {}
        Float m1 = 0.0f;
    };
    template<>
    struct MomentBuffer<FSS_MomentLevel::MuVar>
    {
        MomentBuffer<FSS_MomentLevel::MuVar>()
            : m1(0.0f), m2(0.0f)
        {}
        Float m1;
        Float m2;
    };
    template<>
    struct MomentBuffer<FSS_MomentLevel::MuVarSkew>
    {
        MomentBuffer<FSS_MomentLevel::MuVarSkew>()
            : m1(0.0f), m2(0.0f), m3(0.0f)
        {}
        Float m1;
        Float m2;
        Float m3;
    };
    template<>
    struct MomentBuffer<FSS_MomentLevel::MuVarSkewKurt>
    {
        MomentBuffer<FSS_MomentLevel::MuVarSkewKurt>()
            : m1(0.0f), m2(0.0f), m3(0.0f), m4(0.0f)
        {}
        Float m1;
        Float m2;
        Float m3;
        Float m4;
    };
    MomentBuffer<ML> mb_;

    // 最大最小用のバッファ
    template<bool T>
    struct MinMaxBuffer {};
    template<>
    struct MinMaxBuffer<true>
    {
        MinMaxBuffer<true>()
        {
            mn = std::numeric_limits<Float>::max();
            mx = std::numeric_limits<Float>::lowest();
        }
        Float mn;
        Float mx;
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