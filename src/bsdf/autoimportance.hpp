#pragma once

#include "pch.hpp"
#include "bsdf.hpp"
#include "core/distribution2d.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class AutoImportance
{
public:
    AutoImportance();
    void setBRDF(const BSDF* bsdf);
    void sample(
        Vec3 localWo, 
        XorShift128& rng, 
        _Out_ Vec3* localWi, 
        _Out_ float* pdf) const;
    float pdf(
        Vec3 localWo,
        Vec3 localWi ) const;
private:
    void genDistribution(
        float coso, 
        const BSDF* bsdf,
        const XorShift128& rng,
        Distribution2D* distribution);
private:
    // (cos(w0),distribuion)のペアのリスト。cos(w0)でソートされている。
    typedef std::pair<float, Distribution2D> DistributionWo;
    std::vector<DistributionWo> distributions_;
};
