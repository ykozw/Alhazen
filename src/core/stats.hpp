#pragma once

#include "pch.hpp"
#include "core/logging.hpp"
#include "core/threadid.hpp"

#if 0
/*
-------------------------------------------------
-------------------------------------------------
*/
struct IncrimentCounterValue
{
    // x64ビルド前提
    std::atomic<uint64_t> value;
    int8_t unused[120];
public:
    IncrimentCounterValue& operator =(const IncrimentCounterValue& other)
    {
        value.store(other.value.load());
        return *this;
    }
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class IncrimentCounter
{
public:
    enum { NUM_COUNTERS = 128 };
public:
    IncrimentCounter(const std::string& name);
    uint64_t operator ++ ();
    static void print();
private:
    // new経由では生成しないようにする
    void* operator new(size_t size) = delete;
    void operator delete(void* p) = delete;
private:
    std::string name_;
    std::array<IncrimentCounterValue, NUM_COUNTERS> values_;
    static std::vector<IncrimentCounter*> counters;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE uint64_t IncrimentCounter::operator++ ()
{
    std::this_thread::get_id();
    const int32_t threadNo = Job::threadId;
    AL_ASSERT_DEBUG(threadNo < NUM_COUNTERS);
    std::atomic<uint64_t>& v = values_[threadNo].value;
    const uint64_t oldValue = v.fetch_add(1, std::memory_order_relaxed);
    return oldValue;
}
#endif

/*
-------------------------------------------------
-------------------------------------------------
*/
struct StatCounterValue
{
    // x64ビルド前提
    double average;   // 8byte
    double m;         // 8byte
    double count;     // 8byte
    double minValue;  // 8byte
    double maxValue;  // 8byte
    int8_t unused[88];
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class StatCounter
{
public:
    enum { NUM_COUNTERS = 128 };
public:
    StatCounter(const std::string& name);
    void add(double newValue);
    static void print();
private:
    void* operator new(size_t size);
    void operator delete(void* p);
private:
    std::string name_;
    std::array<StatCounterValue, NUM_COUNTERS> values_;
    static std::vector<StatCounter*> counters;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void StatCounter::add(double newValue)
{
    // 参照: http://qpp.bitbucket.org/post/variance/
    const int32_t thredNo = Job::threadId;
    AL_ASSERT_DEBUG(thredNo < NUM_COUNTERS);
    StatCounterValue& value = values_[thredNo];
    const double oldAve = value.average;
    const double newAve = (1.0 / (1.0 + value.count)) * (newValue - oldAve) + oldAve;
    const double oldM = value.m;
    const double newM = (newValue - newAve) * (newValue - oldAve) + oldM;
    value.average = newAve;
    value.m = newM;
    // 最小最大の更新
    value.maxValue = std::max(value.maxValue, newValue);
    value.minValue = std::min(value.minValue, newValue);
}

