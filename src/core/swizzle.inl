﻿#include "math.hpp"

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::xxx() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 0, 0));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::xxy() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 0, 0));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::xxz() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 2, 0, 0));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::xyx() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 1, 0));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::xyy() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 1, 0));
}

#if 0
//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::xyz() const
{
    return *this;
}
#endif

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::xzx() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 2, 0));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::xzy() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 2, 0));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::xzz() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 2, 2, 0));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::yxx() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 0, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::yxy() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 0, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::yxz() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 2, 0, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::yyx() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 1, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::yyy() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 1, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::yyz() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 2, 1, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::yzx() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 2, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::yzy() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 2, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::yzz() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 2, 2, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::zxx() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 0, 2));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::zxy() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 0, 2));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::zxz() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 2, 0, 2));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::zyx() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 1, 2));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::zyy() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 1, 2));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::zyz() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 2, 1, 2));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::zzx() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 2, 2));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::zzy() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 2, 2));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::zzz() const
{
    return _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 2, 2, 2));
}
