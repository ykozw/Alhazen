#pragma once

#include "pch.hpp"
#include "core/math.hpp"
#include "sampler/sampler.hpp"

//-------------------------------------------------
//
//-------------------------------------------------
bool sameHemisphere(const Vec3& localWi, const Vec3& localWo);
float cosTheta(const Vec3& v);
float absCosTheta(const Vec3& v);
float sinThetaSq(const Vec3& v);
float sinTheta(const Vec3& v);
float cosPhi(const Vec3& v);
float sinPhi(const Vec3& v);
void sincosPhi(const Vec3& v, _Out_ float* sinPhi, _Out_ float* cosPhi);

//-------------------------------------------------
// str2spectrum()
//-------------------------------------------------
Spectrum str2spectrum(const std::string& str);


#include "bsdfutil.inl"
