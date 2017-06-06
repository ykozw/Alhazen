#include "pch.hpp"
#include "core/stats.hpp"
#include "core/math.hpp"

std::vector<IncrimentCounter*> IncrimentCounter::counters;
std::vector<StatCounter*> StatCounter::counters;
//
IncrimentCounter g_intersectTriangle("INTERSECT_TRIANGLE");
IncrimentCounter g_intersectSphere("INTERSECT_SPHERE");
IncrimentCounter g_intersectPlane("INTERSECT_PLANE");
IncrimentCounter g_checkTriangle("CHECK_TRIANGLE");
IncrimentCounter g_checkSphere("CHECK_SPHERE");
IncrimentCounter g_checkPlane("CHECK_PLANE");
IncrimentCounter g_checkAABB("CHECK_AABB");

//-------------------------------------------------
// IncrimentCounter
//-------------------------------------------------
IncrimentCounter::IncrimentCounter(const std::string& name)
    :name_(name)
{
    const IncrimentCounterValue zero = { 0, 0 };
    values_.fill(zero);
    counters.push_back(this);
}

//-------------------------------------------------
// print
//-------------------------------------------------
void IncrimentCounter::print()
{
    logging("-------------------------------------------------");
    logging("IncrimentCounters");
    for (const auto& counter : counters)
    {
        uint64_t total = 0;
        for (const IncrimentCounterValue& value : counter->values_)
        {
            total += value.value;
        }
        logging("Counter[%s]: %" PRIu64, counter->name_.c_str(), total);
    }
}

//-------------------------------------------------
// StatCounter
//-------------------------------------------------
StatCounter::StatCounter(const std::string& name)
{
    static_cast<void>(name);

    for (auto& v : values_)
    {
        v.average = 0.0f;
        v.m = 0.0f;
        v.count = 0;
        v.minValue = std::numeric_limits<double>::max();
        v.maxValue = std::numeric_limits<double>::min();
    }
    counters.push_back(this);
}

//-------------------------------------------------
// print
//-------------------------------------------------
void StatCounter::print()
{
    logging("-------------------------------------------------");
    logging("StatCounter");
    for (const auto& counter : counters)
    {
        double ave = 0.0;
        double var = 0.0;
        double count = 0.0;
        double minv = std::numeric_limits<double>::max();
        double maxv = std::numeric_limits<double>::min();
        for (const StatCounterValue& value : counter->values_)
        {
            count += value.count;
            ave += value.average * value.count;
            // var(X)は加法性があることに注意
            var += (value.m / (value.count - 1.0)) * value.count;
            //
            minv = alMin(minv, value.minValue);
            maxv = alMin(maxv, value.maxValue);
        }
        ave /= count;
        var /= count;
        logging("Counter[%s]: Ave:%f\tVar:%f\tMin:%f\tMax:%f", counter->name_.c_str(), ave, var, minv, maxv);
    }
}
