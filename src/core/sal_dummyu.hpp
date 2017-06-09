#pragma once

#include "pch.hpp"
#include "core/logging.hpp"
#include "core/threadid.hpp"

//-------------------------------------------------
// IncrimentCounterValue
//-------------------------------------------------
struct IncrimentCounterValue
{
    // x64ビルド前提
    uint64_t value;
    int8_t unused[120];
};

//-------------------------------------------------
// IncrimentCounter
//-------------------------------------------------
class IncrimentCounter
{
public:
    enum { NUM_COUNTERS = 128};
public:
    IncrimentCounter( const std::string& name );
    uint64_t operator ++ ();
    static void print();
private:
    // new経由では生成しないようにする
    void* operator new(size_t size);
    void operator delete(void* p);
private:
    std::string name_;
    std::array<IncrimentCounterValue, NUM_COUNTERS> values_;    
    static std::vector<IncrimentCounter*> counters;
};

//-------------------------------------------------
// operator++
//-------------------------------------------------
INLINE uint64_t IncrimentCounter::operator++ ()
{
    std::this_thread::get_id();
    const int32_t thredNo = Job::threadId;
    AL_ASSERT_DEBUG(thredNo < NUM_COUNTERS);
    _InterlockedExchangeAdd64(reinterpret_cast<__int64 volatile *>(&values_[thredNo].value), 1);
    return values_[thredNo].value;
}

//-------------------------------------------------
// StatCounterValue
//-------------------------------------------------
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

//-------------------------------------------------
// StatCounter
//-------------------------------------------------
class StatCounter
{
public:
    enum { NUM_COUNTERS = 128 };
public:
    StatCounter(const std::string& name);
    void add( double newValue );
    static void print();
private:
    void* operator new(size_t size);
    void operator delete(void* p);
private:
    std::string name_;
    std::array<StatCounterValue, NUM_COUNTERS> values_;
    static std::vector<StatCounter*> counters;
};

//-------------------------------------------------
// add
//-------------------------------------------------
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

