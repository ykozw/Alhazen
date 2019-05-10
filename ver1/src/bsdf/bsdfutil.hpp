#pragma once

#include "core/math.hpp"
#include "sampler/sampler.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
bool
sameHemisphere(Vec3 localWi, Vec3 localWo);
float
cosTheta(Vec3 v);
float
absCosTheta(Vec3 v);
float
sinThetaSq(Vec3 v);
float
sinTheta(Vec3 v);
float
cosPhi(Vec3 v);
float
sinPhi(Vec3 v);
void
sincosPhi(Vec3 v, float* sinPhi, float* cosPhi);

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum
str2spectrum(const std::string& str);

#include "bsdfutil.inl"
