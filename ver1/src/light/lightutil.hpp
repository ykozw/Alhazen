#pragma once

#include "core/math.hpp"

/*
-------------------------------------------------
フォームファクターとPDFの算出
-------------------------------------------------
*/
template<bool twosided0 = false, bool twosided1 = false>
void
calcFormFactor(const Vec3& p0,
               const Vec3& n0,
               const Vec3& p1,
               const Vec3& n1,
               float* formFactor,
               float* pdf)
{
    const Vec3 dir = p0 - p1;
    const Vec3 dirNormalized = dir.normalized();
    const float distanceSq = dir.lengthSq();
    const float f0 = Vec3::dot(n0, dirNormalized);
    const float f1 = Vec3::dot(n1, dirNormalized);
    const float fa0 = twosided0 ? std::fabsf(f0) : f0;
    const float fa1 = twosided1 ? std::fabsf(f1) : f1;
    const float ff = (fa0 * fa1) / (distanceSq * PI);
    *formFactor = ff;
    *pdf = (4.0f * PI) / ff;
}
