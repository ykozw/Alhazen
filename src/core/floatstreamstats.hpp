#pragma once

#include "core/rng.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class FloatStreamStats /*AL_FINAL*/
{
public:
    FloatStreamStats() = default;
    virtual void add(float v);
    int32_t size() const;
    float mean() const;
    float variance() const;
    float sigma() const;
    float max() const;
    float min() const;
    std::tuple<float, float> meanConfidenceInterval() const;
    static bool maybeSameMean(const FloatStreamStats& lhs,
                              const FloatStreamStats& rhs);

protected:
    float mean_ = 0.0f;
    float M_ = 0.0f;
    float n_ = 0.0f;
    float min_ = std::numeric_limits<float>::max();
    float max_ = std::numeric_limits<float>::min();
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
