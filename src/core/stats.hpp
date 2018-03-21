#pragma once

/*
-------------------------------------------------
-------------------------------------------------
*/
class CounterStats
{
public:
    typedef void (*PreFun)(void);
    typedef void (*PostFun)(void);

public:
    static void preParallel();
    static void postParallel();
    static void addCounter(const std::string& title,
                           const std::string& unitNames,
                           std::atomic<int64_t>* counter,
                           PreFun preFun,
                           PostFun postFun);
    static void printStats(bool clearStats);
};

#define STATS_COUNTER(TITLE, COUNTER, UNIT_NAME)                               \
    __thread int64_t COUNTER;                                                  \
    std::atomic<int64_t> COUNTER##_master;                                     \
    class RegisterStats##COUNTER                                               \
    {                                                                          \
    public:                                                                    \
        RegisterStats##COUNTER()                                               \
        {                                                                      \
            CounterStats::addCounter(TITLE,                                    \
                                     UNIT_NAME,                                \
                                     &COUNTER##_master,                        \
                                     []() { COUNTER = 0; },                    \
                                     []() { COUNTER##_master += COUNTER; });   \
        }                                                                      \
    } registerStats##COUNTER;
