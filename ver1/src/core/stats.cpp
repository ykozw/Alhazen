#include "core/stats.hpp"
#include "core/logging.hpp"

//
static std::chrono::time_point<std::chrono::system_clock> g_start;

class StatStart
{
public:
    StatStart()
    {
        // 時間計測を開始する
        g_start = std::chrono::system_clock::now();
    }
} statStart;

/*
-------------------------------------------------
-------------------------------------------------
*/
struct CounterInfo
{
    std::string title;
    std::string unitName;
    std::atomic<int64_t>* counter;
    CounterStats::PreFun preFun;
    CounterStats::PostFun postFun;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
std::vector<CounterInfo>& getCounterInfo()
{
    static std::vector<CounterInfo> counterInfo;
    return counterInfo;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void CounterStats::preParallel()
{
    for (auto& ci : getCounterInfo())
    {
        ci.preFun();
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void CounterStats::postParallel()
{
    for (auto& ci : getCounterInfo())
    {
        ci.postFun();
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void CounterStats::addCounter(const std::string& title,
                              const std::string& unitName,
                              std::atomic<int64_t>* counter,
                              PreFun preFun,
                              PostFun postFun)
{
    CounterInfo newCI;
    newCI.title = title;
    newCI.unitName = unitName;
    newCI.counter = counter;
    newCI.preFun = preFun;
    newCI.postFun = postFun;
    // タイトル順でソート
    auto& vec = getCounterInfo();
    auto ite =
        std::lower_bound(vec.begin(),
                         vec.end(),
                         title,
                         [](const CounterInfo& ci, const std::string& title) {
                             return ci.title < title;
                         });
    vec.insert(ite, newCI);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void CounterStats::printStats(bool clearStats)
{
    auto elapased = std::chrono::system_clock::now() - g_start;
    const int64_t elapseTimeInMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(elapased).count();
    //
    logging("Counters");
    for (auto& ci : getCounterInfo())
    {
        // そのまま表示する場合
        const int64_t count = ci.counter->load();
        const double counterPerSec =
            double(count) / double(elapseTimeInMs) * 1000.0;
        const double MCountPerSec = double(counterPerSec) / double(1000000);
        const double KCountPerSec = double(counterPerSec) / double(1000);
        if (MCountPerSec > 0.1)
        {
            logging(" %-10s: %2.1f M%s/sec",
                    ci.title.c_str(),
                    MCountPerSec,
                    ci.unitName.c_str());
        }
        else if (KCountPerSec > 1.0)
        {
            logging(" %-10s: %2.1f K%s/sec",
                    ci.title.c_str(),
                    KCountPerSec,
                    ci.unitName.c_str());
        }
        else
        {
            logging(" %-10s: %lld %s/sec",
                    ci.title.c_str(),
                    count,
                    ci.unitName.c_str());
        }
    }

    if (clearStats)
    {
        for (auto& ci : getCounterInfo())
        {
            *ci.counter = 0;
        }
        // 時間計測を開始する
        g_start = std::chrono::system_clock::now();
    }
}
