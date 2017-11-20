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
    static bool maybeSameMean(const FloatStreamStats& lhs, const FloatStreamStats& rhs);
protected:
    float mean_ = 0.0f;
    float M_ = 0.0f;
    float n_ = 0.0f;
    float min_ = std::numeric_limits<float>::max();
    float max_ = std::numeric_limits<float>::min();
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class FloatStreamStats2 AL_FINAL
{
public:
    struct float2
    {
        float x;
        float y;
    };
public:
    FloatStreamStats2() = default;
    void add(float v0, float v1);
    int32_t size() const;
    float2 mean() const;
    float2 variance() const;
    float2 sigma() const;
    float cov() const;
protected:
    float means_[2] = { 0.0f,0.0f };
    float Ms_[2] = { 0.0f,0.0f };
    float cov_ = 0.0f;
    float n_ = 0.0f;
};

/*
-------------------------------------------------
TODO: Reservoir Samplingも行う。P2も行うようにする。
-------------------------------------------------
*/
class FloatStreamStatsEx
    :public FloatStreamStats
{
public:
    FloatStreamStatsEx(int32_t sampleSize);
    virtual void add(float v) override;
private:
    std::vector<float> samples_;
    RNG rng_;
};
