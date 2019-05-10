#pragma once

#include "core/errfloat.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat::ErrFloat(float v)
    :v_(v), low_(v), high_(v)
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat::ErrFloat(float v, float err)
{
    if (err == 0.0f)
    {
        v_ = v;
        low_ = v;
        high_ = v;
    }
    else
    {
        v_ = v;
        low_ = nextFloatUp(v - err);
        high_ = nextFloatDown(v + err);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat::ErrFloat(const ErrFloat& other)
{
    v_ = other.v_;
    low_ = other.low_;
    high_ = other.high_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float ErrFloat::low() const
{
    return low_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float ErrFloat::high() const
{
    return high_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat ErrFloat::sqrt() const
{
    ErrFloat ret;
    ret.v_ = std::sqrtf(v_);
    ret.low_ = nextFloatDown(std::sqrtf(low_));
    ret.high_ = nextFloatDown(std::sqrtf(high_));
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat ErrFloat::operator+(ErrFloat other) const
{
    ErrFloat ret;
    ret.v_ = v_ + other.v_;
    ret.low_ = nextFloatDown(low_ + other.low_);
    ret.high_ = nextFloatUp(high_ + other.high_);
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat ErrFloat::operator-(ErrFloat other) const
{
    ErrFloat ret;
    ret.v_ = v_ - other.v_;
    ret.low_ = nextFloatDown(low_ - other.high_);
    ret.high_ = nextFloatUp(high_ - other.low_);
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat ErrFloat::operator*(ErrFloat other) const
{
    ErrFloat ret;
    ret.v_ = v_ * other.v_;
    /*
    low,highが0を跨ぐ場合を考慮すると
    どの組み合わせがlow,highになるか自明ではない
    複雑な条件分岐よりもmin(),max()を使うことにする
    */
    const float p0 = low_ * other.low_;
    const float p1 = low_ * other.high_;
    const float p2 = high_ * other.low_;
    const float p3 = high_ * other.high_;
    // NOTE: initializer listで全体を囲ってしまうと最適化されなくなるので手動
    ret.low_ = std::min(std::min(p0, p1), std::min(p2, p3));
    ret.high_ = std::max(std::max(p0, p1), std::max(p2, p3));
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat ErrFloat::operator/(ErrFloat other) const
{
    ErrFloat ret;
    ret.v_ = v_ / other.v_; 
    // intervalが0を跨いでいる場合、もはや何も確かに言えなくなるのでinfにする
    if ((other.low_ < 0.0f) && (0.0f < other.high_))
    {
        ret.low_ = -std::numeric_limits<float>::infinity();
        ret.high_ = std::numeric_limits<float>::infinity();
    }
    else
    {
        // operator * 同様に全ての場合を見る必要がある
        const float p0 = low_ / other.low_;
        const float p1 = low_ / other.high_;
        const float p2 = high_ / other.low_;
        const float p3 = high_ / other.high_;
        ret.low_ = std::min(std::min(p0, p1), std::min(p2, p3));
        ret.high_ = std::max(std::max(p0, p1), std::max(p2, p3));
    }
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat& ErrFloat::operator=(const ErrFloat& other)
{
    if (this != &other)
    {
        v_ = other.v_;
        low_ = other.low_;
        high_ = other.high_;
    }
    return *this;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat ErrFloat::operator-() const
{
    ErrFloat ret;
    ret.v_ = -v_;
    ret.low_ = -high_;
    ret.high_ = -low_;
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool ErrFloat::operator==(ErrFloat other) const
{
    return (v_ == other.v_);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat::operator float() const
{
    return v_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat operator*(float f, ErrFloat ef)
{
    return ErrFloat(f) * ef;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat operator/(float f, ErrFloat ef)
{
    return ErrFloat(f) / ef;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat operator+(float f, ErrFloat ef)
{
    return ErrFloat(f) + ef;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ErrFloat operator-(float f, ErrFloat ef)
{
    return ErrFloat(f) - ef;
}