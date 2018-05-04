#pragma once

/*
-------------------------------------------------
uint32_t -> float
-------------------------------------------------
*/
INLINE float
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
INLINE uint32_t
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
INLINE float
nextFloatUp(float v)
{
    // inf -> inf
    if (std::isinf(v) && v > 0.0f)
    {
        return v;
    }
    //
    if (v == -0.0f)
    {
        v = +0.0f;
    }
    //
    uint32_t u = float2uint(v);
    if (v >= 0.0f)
    {
        ++u;
    }
    else
    {
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
INLINE float
nextFloatDown(float v)
{
    // -inf -> -inf
    if (std::isinf(v) && v < 0.0f)
    {
        return v;
    }
    //
    if (v == +0.0f)
    {
        v = -0.0f;
    }
    //
    uint32_t u = float2uint(v);
    if (v >= 0.0f)
    {
        --u;
    }
    else
    {
        ++u;
    }
    const float nv = uint2float(u);
    return nv;
}


/*
-------------------------------------------------
誤差範囲付き浮動小数点
-------------------------------------------------
*/
class ErrFloat
{
public:
    ErrFloat() = default;
    ErrFloat(float v);
    ErrFloat(float v, float err);
    ErrFloat(const ErrFloat& other);
    float low() const;
    float high() const;
    ErrFloat sqrt() const;
    ErrFloat operator+(ErrFloat other) const;
    ErrFloat operator-(ErrFloat other) const;
    ErrFloat operator*(ErrFloat other) const;
    ErrFloat operator/(ErrFloat other) const;
    ErrFloat operator-() const;
    ErrFloat& operator=(const ErrFloat& other);
    bool operator==(ErrFloat other) const;
    explicit operator float() const;
    //
private:
    float v_, low_, high_;
};
ErrFloat operator*(float f, ErrFloat ef);
ErrFloat operator/(float f, ErrFloat ef);
ErrFloat operator+(float f, ErrFloat ef);
ErrFloat operator-(float f, ErrFloat ef);

//
#include "core/errfloat.inl"