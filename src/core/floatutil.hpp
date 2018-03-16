#pragma once

#include "pch.hpp"

/*
-------------------------------------------------
uint32_t -> float
-------------------------------------------------
*/
float
uint2float(uint32_t v)
{
    float r;
    memcpy(&r, &v, sizeof(uint32_t));
    return r;
}

/*
-------------------------------------------------
float -> uint32_t
-------------------------------------------------
*/
uint32_t
float2uint(float v)
{
    uint32_t r;
    memcpy(&r, &v, sizeof(float));
    return r;
}

/*
-------------------------------------------------
floatを表現可能な値で次に大きな値に上げる
-------------------------------------------------
*/
float
nextFloatUp(float v)
{
    // inf -> inf
    if (std::isinf(v) && v > 0.0f) {
        return v;
    }
    //
    if (v == -0.0f) {
        v = +0.0f;
    }
    //
    uint32_t u = float2uint(v);
    if (v >= 0.0f) {
        ++u;
    } else {
        --u;
    }
    const float nv = uint2float(u);
    return nv;
}

/*
-------------------------------------------------
floatを表現可能な値で次に小さな値に下げる
-------------------------------------------------
*/
float
nextFloatDown(float v)
{
    // -inf -> -inf
    if (std::isinf(v) && v < 0.0f) {
        return v;
    }
    //
    if (v == +0.0f) {
        v = -0.0f;
    }
    //
    uint32_t u = float2uint(v);
    if (v >= 0.0f) {
        --u;
    } else {
        ++u;
    }
    const float nv = uint2float(u);
    return nv;
}
