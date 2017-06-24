#pragma once

#include "pch.hpp"

/*
-------------------------------------------------
RussianRoulette
Efficiency-optimized Russian rouletteの実装
将来的にBPTを実装したときに使う。
-------------------------------------------------
*/
class RussianRoulette
{
public:
    RussianRoulette();
    void addContribution(float contribtuion, int32_t rayCount);
    void reset();
    float rouletteProb() const;
private:
    float numContribution_;
    float aveRayCount_;
    float cAve_;
    float cM_;
};
