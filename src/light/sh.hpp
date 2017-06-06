#pragma once

#include "pch.hpp"
#include "core/math.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
template<int32_t l,int32_t m>
float sphericalHarmonics(float theta, float phi);

/*
-------------------------------------------------
spherical harmonics
-------------------------------------------------
*/
template<int32_t ORDER>
struct SphericalHarmonicsCoeff
{
public:
    static_assert(2 <= ORDER && ORDER <= 3, "");
    std::array<float, ORDER*ORDER> values;

public:
    void clear()
    {
        values.fill(0.0f);
    }
    SphericalHarmonicsCoeff<ORDER>& operator = (const SphericalHarmonicsCoeff<ORDER>& other)
    {
        values = other.values;
        return *this;
    }
    
public:
    //
    static SphericalHarmonicsCoeff<ORDER> evalDirection(const Vec3& dir);
    static SphericalHarmonicsCoeff<ORDER> evalDirectionalLight(const Vec3& dir);
    //
    static SphericalHarmonicsCoeff<ORDER> scale(
        const SphericalHarmonicsCoeff<ORDER>& v,
        float s);
    static SphericalHarmonicsCoeff<ORDER> add(
        const SphericalHarmonicsCoeff<ORDER>& lhs,
        const SphericalHarmonicsCoeff<ORDER>& rhs);
    //
    static float dot(
        const SphericalHarmonicsCoeff<ORDER>& lhs,
        const SphericalHarmonicsCoeff<ORDER>& rhs);
};

#include "sh.inl"

