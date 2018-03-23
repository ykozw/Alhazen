#include "pch.hpp"
#include "bsdf/bsdf.hpp"

REGISTER_OBJECT(BSDF, BSDFs);

/*
-------------------------------------------------
-------------------------------------------------
*/
BSDFs::BSDFs(const ObjectProp& objectProp) : BSDF(objectProp), name_("default")
{
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void BSDFs::setName(const std::string& name) { name_ = name; }

/*
-------------------------------------------------
-------------------------------------------------
*/
const std::string& BSDFs::getName() const { return name_; }

/*
-------------------------------------------------
-------------------------------------------------
*/
void BSDFs::add(BSDFPtr bsdf) { bsdfs_.push_back(bsdf); }

/*
-------------------------------------------------
-------------------------------------------------
*/
float BSDFs::pdf(Vec3 localWo, Vec3 localWi) const
{
    //
    if (bsdfs_.size() == 0)
    {
        return 0.0f;
    }
    //
    float total = 0.0f;
    for (auto& bsdf : bsdfs_)
    {
        total += bsdf->pdf(localWo, localWi);
    }
    const float pdf = total / (float)bsdfs_.size();
    return pdf;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum BSDFs::bsdf(Vec3 localWo, Vec3 localWi) const
{
    Spectrum r(0.0f);
    for (auto& bsdf : bsdfs_)
    {
        r += bsdf->bsdf(localWo, localWi);
    }
    return r;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum BSDFs::bsdfSample(Vec3 localWo,
                           Sampler* sampler,
                           Vec3* localWi,
                           float* pdf) const
{
    //
    if (bsdfs_.size() == 0)
    {
        return Spectrum(0.0f);
    }
    // どのBSDFをサンプルするかを選択
    const int32_t bsdfIndex = sampler->getSize(int32_t(bsdfs_.size()));
    const auto& bsdf = bsdfs_[bsdfIndex];

    // bsdfをサンプル
    return bsdf->bsdfSample(localWo, sampler, localWi, pdf);
}
