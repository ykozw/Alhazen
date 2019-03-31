#include "core/distribution1d.hpp"
#include "core/logging.hpp"
#include "core/math.hpp"
#include "core/rng.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive() { construct({{}}); }

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive(const std::vector<float>& values)
{
    construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive(
    const std::initializer_list<float>& values)
{
    construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive(const float* values, int32_t num)
{
    construct(std::vector<float>(values, values + num));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_Naive::Distribution1D_Naive(std::function<float(float)> genFunc,
                                           int32_t split)
{
    std::vector<float> values;
    values.resize(split);
    for (int32_t i = 0; i < split; ++i)
    {
        const float samplePoint =
            (0.5f + static_cast<float>(i)) / static_cast<float>(split);
        values[i] = genFunc(samplePoint);
    }
    construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Distribution1D_Naive::construct(const std::vector<float>& values)
{
#if 0
    uint32_t cdfSize = (uint32_t)values.size();
    if (!alIsPowerOfTwo(cdfSize + 1))
    {
        const uint32_t x = (uint32_t)cdfSize;
        const uint32_t y = (x - ((x & (~x + 1)))) << 1;
        cdfSize = y;
    }
#endif
    //
    cdf_.resize(values.size() + 1);
    cdf_[0] = 0.0f;
    for (int32_t i = 0; i < values.size(); ++i)
    {
        AL_ASSERT_DEBUG(values[i] >= 0.0f);
        AL_ASSERT_DEBUG(!isnan(values[i]));
        cdf_[i + 1] = cdf_[i] + values[i];
    }
    if (cdf_.back() == 0.0f)
    {
        cdf_.back() = 1.0f;
    }
    const float scale = 1.0f / cdf_.back();
    for (auto& v : cdf_)
    {
        v *= scale;
    }
    numValue_ = (float)(values.size());
    invNumValue_ = 1.0f / numValue_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution1D_Naive::sample(float u, float* pdf, int32_t* offset) const
{
    const auto upperIte = std::lower_bound(cdf_.begin(), cdf_.end(), u);
    const int32_t index = alMax((int32_t)(upperIte - cdf_.begin() - 1), 0);
    //
    if (offset)
    {
        *offset = index;
    }
    const float t = (u - cdf_[index]) / (cdf_[index + 1] - cdf_[index]);
    *pdf = (cdf_[index + 1] - cdf_[index]) * numValue_;
    const float samplePoint = ((float)index + t) * invNumValue_;
    return samplePoint;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution1D_Naive::pdf(int32_t index) const
{
    return (cdf_[index + 1] - cdf_[index]) * numValue_;
}


/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution1D_AliasMethod::Distribution1D_AliasMethod()
{
    construct({});
}
Distribution1D_AliasMethod::Distribution1D_AliasMethod(const std::vector<float>& weights)
{
    construct(weights);
}
Distribution1D_AliasMethod::Distribution1D_AliasMethod(const std::initializer_list<float>& weights)
{
    construct(weights);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Distribution1D_AliasMethod::construct(const std::vector<float>& aWeights)
{
    if (aWeights.empty())
    {
        return;
    }
    //
    std::vector<float> weights = aWeights;
    for (float& w : weights)
    {
        // 負の値は0にしておく
        w = std::max(w, 0.0f);
    }
    //
    const size_t n = weights.size();
    aliasProbs_.resize(n);
    //
    struct Weight
    {
    public:
        int32_t idx;
        float prob;
    };
    //
    std::vector<Weight> smalls;
    std::vector<Weight> larges;
    const size_t half1 = n / 2 + 1;
    smalls.reserve(half1);
    larges.reserve(half1);
    //
    const float total = std::accumulate(weights.begin(), weights.end(), 0.0f);
    // 総和が0であれば全ての確率を同じにする
    if (total == 0.0f)
    {
        for (auto& ap : aliasProbs_)
        {
            ap.alias = -1;
            ap.pdf = 1.0f;
            ap.prob = 1.0f;
        }
        return;
    }
    const float scale = float(n) / total;
    //
    for (int32_t i = 0; i < n; ++i)
    {
        // 負の値は0にしておく
        const float w = std::max(weights[i], 0.0f);
        //
        const float ws = w * scale;
        if (ws < 1.0f)
        {
            smalls.push_back({i, ws});
        }
        else
        {
            larges.push_back({i, ws});
        }
        //
        aliasProbs_[i].pdf = ws;
    }
    //
    while (!smalls.empty() && !larges.empty())
    {
        auto& l = smalls.back(); 
        smalls.pop_back();
        auto& g = larges.back();
        auto& b = aliasProbs_[l.idx];
        b.prob = l.prob;
        b.alias = g.idx;
        g.prob -= (1.0f-l.prob);
        //
        if (g.prob < 1.0f)
        {
            larges.pop_back();
            smalls.push_back(g);
        }
    }
    //
    while (!larges.empty())
    {
        auto& g = larges.back();
        larges.pop_back();
        auto& apg = aliasProbs_[g.idx];
        apg.prob = 1.0f;
        apg.alias = -1;
    }
    //
    while (!smalls.empty())
    {
        auto& l = smalls.back();
        smalls.pop_back();
        auto& apl = aliasProbs_[l.idx];
        apl.prob = 1.0f;
        apl.alias = -1;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution1D_AliasMethod::sample(float u0, float* pdf, int32_t* offset) const
{
    //
    int32_t offsetDummy = 0;
    offset = (offset != nullptr) ? offset : &offsetDummy;
    //
    if (aliasProbs_.empty())
    {
        *pdf = 1.0f;
        *offset = 0;
        return 0.0f;
    }
    const int32_t idx = int32_t(u0 * aliasProbs_.size());
    const float u1 = std::fmodf(u0*float(aliasProbs_.size()), 1.0f);
    const AliasProb& aliasProb = aliasProbs_[idx];
    //
    if (u1 < aliasProb.prob)
    {
        *pdf = aliasProb.pdf;
        *offset = idx;
        const float u2 = u1 / aliasProb.prob;
        return float(idx) + u2;
    }
    else
    {
        const int32_t aliasIdx = aliasProb.alias;
        auto& alias = aliasProbs_[aliasIdx];
        *pdf = alias.pdf;
        *offset = aliasIdx;
        const float u2 = (u1 - aliasProb.prob)/(1.0f- aliasProb.prob);
        return float(aliasIdx) + u2;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution1D_AliasMethod::pdf(int32_t index) const
{
    return aliasProbs_[index].pdf;
}
