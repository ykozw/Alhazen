#include "pch.hpp"
#include "core/russianroulette.hpp"

//-------------------------------------------------
//
//-------------------------------------------------
RussianRoulette::RussianRoulette()
    :numContribution_(0.0f),
     aveRayCount_(0.0f),
     cAve_(0.0f),
     cM_(0.0f)
{
    reset();
}

//-------------------------------------------------
//
//-------------------------------------------------
void RussianRoulette::addContribution(float contribtuion, int32_t rayCount)
{
    // ray数の平均の更新
    aveRayCount_ = 1.0f / (1.0f + (float)numContribution_) * ((float)rayCount - aveRayCount_);

    // Contributionの平均の更新
    const float cAveOld = cAve_;
    cAve_ = 1.0f / (1.0f + (float)numContribution_) * ((float)cAve_ - contribtuion);
    // ContributionのMの更新
    cM_ = cM_ + (contribtuion - cAve_) * (contribtuion - cAveOld);
    //
    numContribution_ += 1.0f;
}

//-------------------------------------------------
//
//-------------------------------------------------
void RussianRoulette:: reset()
{
    numContribution_ = 0.0f;
    aveRayCount_ =0.0f;
    cAve_ = 0.0f;
    cM_ = 0.0f;
}

//-------------------------------------------------
//
//-------------------------------------------------
float RussianRoulette::rouletteProb() const
{
    // TODO: 実装
    AL_ASSERT_DEBUG(false);
    return 1.0f;
}
