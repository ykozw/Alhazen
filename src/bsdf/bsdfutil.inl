#pragma once

#include "bsdfutil.hpp"
#include "core/image.hpp"
#include "core/math.hpp"
#include "core/rng.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool sameHemisphere(Vec3 localWi, Vec3 localWo)
{
    //
    AL_ASSERT_DEBUG(localWi.isNormalized(0.03f));
    AL_ASSERT_DEBUG(localWo.isNormalized(0.03f));
    //
    return localWi.z() * localWo.z() > 0.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float cosTheta(Vec3 v)
{
    AL_ASSERT_DEBUG(v.isNormalized());
    // 理屈の上では[-1,1]になるはずだが計算誤差がある場合があるのでclamp
    return alClamp(v.z(), -1.0f, 1.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float absCosTheta(Vec3 v)
{
    AL_ASSERT_DEBUG(v.isNormalized());
    // 理屈の上では[0,1]になるはずだが計算誤差がある場合があるのでclamp
    const float tmp = fabsf(v.z());
    return alClamp(tmp, 0.0f, 1.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float sinThetaSq(Vec3 v)
{
    AL_ASSERT_DEBUG(v.isNormalized());
    const float ct = cosTheta(v);
    // 理屈の上では[0,1]になるはずだが計算誤差がある場合があるのでclamp
    const float tmp = 1.0f - ct * ct;
    return alClamp(tmp, 0.0f, 1.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float sinTheta(Vec3 v)
{
    AL_ASSERT_DEBUG(v.isNormalized());
    // 理屈の上では[0,1]になるはずだが計算誤差がある場合があるのでclamp
    const float tmp = sqrtf(sinThetaSq(v));
    return alClamp(tmp, 0.0f, 1.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float cosPhi(Vec3 v)
{
    AL_ASSERT_DEBUG(v.isNormalized());
    const float st = sinTheta(v);
    if (st == 0.0f)
    {
        return 0.0f;
    }
    float cp = v.x() / st;
    // 理屈の上では[-1,1]になるはずだが計算誤差がある場合があるのでclamp
    cp = alClamp(cp, -1.0f, 1.0f);
    return cp;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float sinPhi(Vec3 v)
{
    AL_ASSERT_DEBUG(v.isNormalized());
    const float st = sinTheta(v);
    if (st == 0.0f)
    {
        return 1.0f;
    }
    float sp = v.y() / st;
    // 理屈の上では[-1,1]になるはずだが計算誤差がある場合があるのでclamp
    sp = alClamp(sp, -1.0f, 1.0f);
    return sp;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void sincosPhi(Vec3 v, float* sinPhi, float* cosPhi)
{
    AL_ASSERT_DEBUG(v.isNormalized());
    const float st = sinTheta(v);
    if (st == 0.0f)
    {
        *sinPhi = 1.0f;
        *cosPhi = 0.0f;
        return;
    }
    float& sp = *sinPhi;
    float& cp = *cosPhi;
    sp = v.y() / st;
    cp = v.x() / st;
    // 理屈の上では[-1,1]になるはずだが計算誤差がある場合があるのでclamp
    sp = alClamp(sp, -1.0f, 1.0f);
    cp = alClamp(cp, -1.0f, 1.0f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Spectrum str2spectrum(const std::string& str)
{
    float r, g, b;
    if (sscanf(str.c_str(), "%f %f %f", &r, &g, &b) == 3)
    {
        return Spectrum::createFromRGB({ { r, g, b } }, false);
    }
    else
    {
        return Spectrum::createFromRGB({ { 0.5f, 0.5f, 0.5f } }, false);
    }
}
