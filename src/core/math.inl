#include "math.hpp"

//-------------------------------------------------
// _mm_rsqrt_ps_accurate()は単体では精度が非常に低いので
// ニュートンラフソンを挟み精度を上げる
// http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=1551
//-------------------------------------------------
static INLINE __m128 _mm_rsqrt_ps_accurate(const __m128 v)
{
    static const __m128 half = _mm_set1_ps(0.5f);
    static const __m128 three = _mm_set1_ps(3.0f);
    const __m128 approx = _mm_rsqrt_ps(v);
    const __m128 muls = _mm_mul_ps(_mm_mul_ps(v, approx), approx);
    return _mm_mul_ps(_mm_mul_ps(half, approx), _mm_sub_ps(three, muls));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D::Size2D(_In_reads_(2) int32_t* es)
	:width(es[0]), height(es[1])
{}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D::Size2D(int32_t awidth, int32_t aheight)
	: width(awidth), height(aheight)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE int32_t Size2D::index(int32_t x, int32_t y) const
{
	return x + y * width;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D operator + (const Size2D& lhs, const Size2D& rhs)
{
	return Size2D(lhs.width + rhs.width, lhs.height + rhs.height);
}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D operator - (const Size2D& lhs, const Size2D& rhs)
{
	return Size2D( lhs.width - rhs.width, lhs.height - rhs.height );
}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D operator - (const Size2D& v)
{
	return Size2D( -v.width, -v.height );
}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D& operator += (Size2D& lhs, const Size2D& rhs)
{
	lhs.width += rhs.width;
	lhs.height += rhs.height;
	return lhs;
}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D& operator -= (Size2D& lhs, const Size2D& rhs)
{
	lhs.width -= rhs.width;
	lhs.height -= rhs.height;
	return lhs;
}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool operator == (const Size2D& lhs, const Size2D& rhs)
{
	return
		lhs.width == rhs.width &&
		lhs.height == rhs.height;
}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D operator * (int32_t f, const Size2D& v)
{
	return Size2D( f * v.width, f * v.height );
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D operator * (const Size2D& v, int32_t f)
{
	return Size2D( f * v.width, f * v.height );
}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE Size2D& operator *= (Size2D& v, int32_t f)
{
	v.width *= f;
	v.height *= f;
	return v;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Region2D::Region2D(int32_t aLeft, int32_t aTop, int32_t aRight, int32_t aBottom)
	:left(aLeft),
	top(aTop),
	right(aRight),
	bottom(aBottom)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE int32_t Region2D::width() const
{
	return right - left;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE int32_t Region2D::height() const
{
	return bottom - top;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE int32_t Region2D::area() const
{
	return width() * height();
}


//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec2::Vec2(_In_reads_(2) float* es)
	:x(es[0]), y(es[1])
{

}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec2::Vec2(float ax, float ay)
	:x(ax), y(ay)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec2::Vec2(float e)
	: x(e), y(e)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Vec2::normalize()
{
	const float invLen = 1.0f / length();
	x /= invLen;
	y /= invLen;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool Vec2::isNormalized() const
{
	return fabsf(lengthSq() - 1.0f) < 1.0e-5; // HACK: この数値は適当
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float Vec2::length() const
{
	return sqrtf(lengthSq());
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float Vec2::lengthSq() const
{
	return x*x + y*y;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float& Vec2::operator[](int32_t index)
{
	AL_ASSERT_DEBUG(0 <= index && index <= 1);
	return *(&x + index);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float Vec2::operator[](int32_t index) const
{
	AL_ASSERT_DEBUG(0 <= index && index <= 1);
	return *(&x + index);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool Vec2::hasNan() const
{
	return
		isnan(x) ||
		isnan(y);

}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float Vec2::dot(const Vec2& lhs, const Vec2& rhs)
{
	return
		lhs.x * rhs.x +
		lhs.y * rhs.y;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec2 operator + (const Vec2& lhs, const Vec2& rhs)
{
	return
		Vec2(
			lhs.x + rhs.x,
			lhs.y + rhs.y);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec2 operator - (const Vec2& lhs, const Vec2& rhs)
{
	return
		Vec2(
			lhs.x - rhs.x,
			lhs.y - rhs.y);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec2 operator - (const Vec2& v)
{
	return  Vec2(-v.x, -v.y);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec2& operator += (Vec2& lhs, const Vec2& rhs)
{
	assert(false);
	//lhs = lhs + rhs;
	return lhs;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec2& operator -= (Vec2& lhs, const Vec2& rhs)
{
	assert(false);
	//lhs = lhs - rhs;
	return lhs;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static bool operator == (const Vec2& lhs, const Vec2& rhs)
{
	return
		(lhs.x == rhs.x) &&
		(lhs.y == rhs.y);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec2 operator * (float f, const Vec2& v)
{
	return
		Vec2(
			f * v.x,
			f * v.y);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec2 operator * (const Vec2& v, float f)
{
	return f*v;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec2& operator *= (Vec2& v, float factor)
{
	assert(false);
	//v = v * factor;
	return v;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec2 operator / (const Vec2& v, float factor)
{
	return
		Vec2(
			v.x / factor,
			v.y / factor);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec::FloatInVec(__m128 av)
	:v(av)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec::FloatInVec(float av)
	:v(_mm_set_ss(av))
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec::operator __m128 () const
{
	return v;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec::operator float() const
{
#if !defined(WINDOWS)
    return v[0];
#else
	return v.m128_f32[0];
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float FloatInVec::value() const
{
#if !defined(WINDOWS)
    return v[0];
#else
	return v.m128_f32[0];
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool operator < (FloatInVec lhs, FloatInVec rhs)
{
    return float(lhs) < float(rhs);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Float8::Float8(__m256 other)
	:v(other)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Float8::operator __m256()const
{
	return v;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Bool8::Bool8(__m256 other)
	:v(other)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool Bool8::at(int32_t index) const
{
#if defined(WINDOWS)
	return v.m256_f32[index] != 0;
#else
    return v[index];
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE BoolInVec::BoolInVec(__m128i av)
	:v(av)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE BoolInVec::BoolInVec(bool av)
	:v(_mm_set1_epi32(av ? 0xFFFFFFFF : 0x00000000))
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE BoolInVec::operator __m128i () const
{
	return v;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE BoolInVec::operator bool() const
{
#if defined(WINDOWS)
	return (v.m128i_i32[0] != 0x00000000);
#else
    AL_ASSERT_ALWAYS(false);
    return false;
    //return _mm_cmpeq_epi32(v,__mm_set1_epi32(0x00000000))[0];
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool BoolInVec::value() const
{
#if defined(WINDOWS)
    return (v.m128i_i32[0] != 0x00000000);
#else
    AL_ASSERT_ALWAYS(false);
    return false;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3::Vec3(_In_reads_(3) float* es)
{
#if defined(AL_MATH_USE_NO_SIMD)
	x = es[0];
	y = es[1];
	z = es[2];
#elif defined(AL_MATH_USE_AVX2)
	xyz = (_mm_set_ps(0.0f, es[2], es[1], es[0]));
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3::Vec3(float ax, float ay, float az)
{
#if defined(AL_MATH_USE_NO_SIMD)
	x = ax;
	y = ay;
	z = az;
#elif defined(AL_MATH_USE_AVX2)
	xyz = _mm_set_ps(0.0f, az, ay, ax);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3::Vec3(float e)
{
#if defined(AL_MATH_USE_NO_SIMD)
	x = e;
	y = e;
	z = e;
#elif defined(AL_MATH_USE_AVX2)
	xyz = (_mm_set_ps(0.0f, e, e, e));
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3::Vec3(__m128 other)
{
#if defined(AL_MATH_USE_NO_SIMD)
	x = other.m128_f32[0];
	y = other.m128_f32[1];
	z = other.m128_f32[2];
#elif defined(AL_MATH_USE_AVX2)
	xyz = other;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3::Vec3(const std::array<float, 3>& arr)
{
#if defined(AL_MATH_USE_NO_SIMD)
	x = arr[0];
	y = arr[1];
	z = arr[2];
#elif defined(AL_MATH_USE_AVX2)
	xyz = _mm_set_ps(0.0f, arr[2], arr[1], arr[0]);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3::Vec3(const Vec4& arr)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x = arr[0];
    y = arr[1];
    z = arr[2];
#elif defined(AL_MATH_USE_AVX2)
    xyz = _mm_set_ps(0.0f, arr[2], arr[1], arr[0]);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Vec3::zero()
{
#if defined(AL_MATH_USE_NO_SIMD)
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
#elif defined(AL_MATH_USE_AVX2)
	xyz = _mm_set_ps1(0.0f);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool Vec3::isZero() const
{
#if defined(AL_MATH_USE_NO_SIMD)
	const float d = 0.0001f;
	return
		fabsf(x) < d &&
		fabsf(y) < d &&
		fabsf(z) < d;
#elif defined(AL_MATH_USE_AVX2)
	const __m128 zero = _mm_setzero_ps();
	const __m128 mask = _mm_cmpeq_ps(zero, xyz);
	const int isZeroMask = _mm_movemask_ps(mask);
	return (isZeroMask == 0);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool Vec3::hasNan() const
{
#if defined(AL_MATH_USE_NO_SIMD)
	return
		isnan(x) ||
		isnan(y) ||
		isnan(z);
#elif defined(AL_MATH_USE_AVX2)
	const __m128 mask = _mm_cmpeq_ps(xyz, xyz);
	return (_mm_movemask_ps(mask) == 0);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3& Vec3::normalize()
{
#if defined(AL_MATH_USE_NO_SIMD)
	const float invLen = 1.0f / length();
	x *= invLen;
	y *= invLen;
	z *= invLen;
	return *this;
#elif defined(AL_MATH_USE_AVX2)
	const __m128 dp = _mm_dp_ps(xyz, xyz, 0x77);
	const __m128 idp = _mm_rsqrt_ps_accurate(dp);
	xyz = _mm_mul_ps(xyz, idp);
	return *this;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::normalized() const
{
#if defined(AL_MATH_USE_NO_SIMD)
	const float invLen = 1.0f / length();
	return Vec3(x * invLen, y * invLen, z * invLen);
#elif defined(AL_MATH_USE_AVX2)
	const __m128 dp = _mm_dp_ps(xyz, xyz, 0x7F);
	const __m128 idp = _mm_rsqrt_ps_accurate(dp);
	return _mm_mul_ps(xyz, idp);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Vec3::scale(float scale)
{
#if defined(AL_MATH_USE_NO_SIMD)
	x *= scale;
	y *= scale;
	z *= scale;
#elif defined(AL_MATH_USE_AVX2)
	const __m128 s = _mm_set1_ps(scale);
	xyz = _mm_mul_ps(xyz, s);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool Vec3::isNormalized() const
{
    // この数値は適当
    const float eps = 1.0e-3f;
    return isNormalized(eps);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool Vec3::isNormalized(float eps) const
{
#if defined(AL_MATH_USE_NO_SIMD)
	return fabsf(lengthSq() - 1.0f) < eps;
#elif defined(AL_MATH_USE_AVX2)
	return (std::fabsf(lengthSq() - 1.0f) < eps);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec Vec3::length() const
{
#if defined(AL_MATH_USE_NO_SIMD)
	return sqrtf(lengthSq());
#elif defined(AL_MATH_USE_AVX2)
	return length(xyz);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec Vec3::lengthSq() const
{
#if defined(AL_MATH_USE_NO_SIMD)
	return x*x + y*y + z*z;
#elif defined(AL_MATH_USE_AVX2)
	return lengthSq(xyz);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE bool Vec3::isAllZero() const
{
#if defined(AL_MATH_USE_NO_SIMD)
	return
		(x == 0.0f) &&
		(y == 0.0f) &&
		(z == 0.0f);
#elif defined(AL_MATH_USE_AVX2)
    return (_mm_movemask_ps(_mm_cmpeq_ps(xyz, _mm_setzero_ps())) & 0x07) != 0;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::inverted() const
{
#if defined(AL_MATH_USE_NO_SIMD)
	return Vec3(1.0f / x, 1.0f / y, 1.0f / z);
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	return _mm_rsqrt_ps_accurate(xyz);
#endif

}

//-------------------------------------------------
// 0になっているレーンだけdefaultValueを設定する
//-------------------------------------------------
INLINE Vec3 Vec3::invertedSafe(const float defaultValue) const
{
#if defined(AL_MATH_USE_NO_SIMD)
	auto inv = [&defaultValue](float v)
	{
		if (v == 0.0f)
		{
			return defaultValue;
		}
		return 1.0f / v;
	};
	return Vec3(inv(x), inv(y), inv(z));
#elif defined(AL_MATH_USE_AVX2)
	//const __m128 dv = _mm_set_ps1(defaultValue);
	const __m128 zero = _mm_setzero_ps();
	const __m128 mask = _mm_cmpeq_ps(zero, xyz);
	const __m128 rcp = _mm_rcp_ps(xyz);
	return _mm_andnot_ps(mask, rcp);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::reflect(const Vec3& v) const
{
#if defined(AL_MATH_USE_NO_SIMD)
	const float factor = -2.0f * dot(*this, v);
	return v + factor * (*this);
#elif defined(AL_MATH_USE_AVX2)
    // TODO: SIMDnize
    const float factor = dot(*this, v) * -2.0f ;
    return v + factor * (*this);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float& Vec3::operator[](int32_t index)
{
	AL_ASSERT_DEBUG(0 <= index && index <= 2);
    
#if !defined(WINDOWS)
    AL_ASSERT_ALWAYS(false);
    return (float&)xyz[index];
#else
    
#if defined(AL_MATH_USE_NO_SIMD)
	AL_ASSERT_DEBUG(0 <= index && index <= 2);
	return *(&x + index);
#elif defined(AL_MATH_USE_AVX2)
	return xyz.m128_f32[index];
#endif
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float Vec3::operator[](int32_t index) const
{
	AL_ASSERT_DEBUG(0 <= index && index <= 2);
    
#if !defined(WINDOWS)
    AL_ASSERT_ALWAYS(false);
    return (float&)xyz[index];
#else
    
#if defined(AL_MATH_USE_NO_SIMD)
	AL_ASSERT_DEBUG(0 <= index && index <= 2);
	return *(&x + index);
#elif defined(AL_MATH_USE_AVX2)
	return xyz.m128_f32[index];
#endif
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec Vec3::length(const Vec3& v)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	return _mm_sqrt_ps(lengthSq(v));
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec Vec3::lengthSq(const Vec3& v)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	return dot(v, v);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec Vec3::distance(const Vec3& lhs, const Vec3& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	const Vec3 tmp = lhs - rhs;
	return tmp.length();
#elif defined(AL_MATH_USE_AVX2)
	const __m128 dif = _mm_sub_ps(lhs.xyz, rhs.xyz);
	return length(dif);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec Vec3::distanceSq(const Vec3& lhs, const Vec3& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	const Vec3 tmp = lhs - rhs;
	return tmp.lengthSq();
#elif defined(AL_MATH_USE_AVX2)
	const __m128 dif = _mm_sub_ps(lhs.xyz, rhs.xyz);
	return lengthSq(dif);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE FloatInVec Vec3::dot(const Vec3& lhs, const Vec3& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	return
		lhs.x * rhs.x +
		lhs.y * rhs.y +
		lhs.z * rhs.z;
#elif defined(AL_MATH_USE_AVX2)
	return _mm_dp_ps(lhs.xyz, rhs.xyz, 0x7F);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::cross(const Vec3& xyz, const Vec3& abc)
{
#if defined(AL_MATH_USE_NO_SIMD)
	return
		Vec3(
			xyz.y * abc.z - xyz.z * abc.y,
            xyz.z * abc.x - xyz.x * abc.z,
            xyz.x * abc.y - xyz.y * abc.x);
#elif defined(AL_MATH_USE_AVX2)
	const __m128 tmp0 = _mm_shuffle_ps(xyz.xyz, xyz.xyz, _MM_SHUFFLE(3, 0, 2, 1));
	const __m128 tmp1 = _mm_shuffle_ps(abc.xyz, abc.xyz, _MM_SHUFFLE(3, 1, 0, 2));
	const __m128 tmp2 = _mm_shuffle_ps(xyz.xyz, xyz.xyz, _MM_SHUFFLE(3, 1, 0, 2));
	const __m128 tmp3 = _mm_shuffle_ps(abc.xyz, abc.xyz, _MM_SHUFFLE(3, 0, 2, 1));
	return _mm_sub_ps(_mm_mul_ps(tmp0, tmp1), _mm_mul_ps(tmp2, tmp3));
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Vec3::mul(const Vec3& lhs, const Vec3& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec3(
            xyz.x * abc.x,
            xyz.y * abc.y,
            xyz.z * abc.z);
#elif defined(AL_MATH_USE_AVX2)
    return _mm_mul_ps(lhs.xyz, rhs.xyz);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3 operator + (const Vec3& lhs, const Vec3& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	return
		Vec3(
			lhs.x + rhs.x,
			lhs.y + rhs.y,
			lhs.z + rhs.z);
#elif defined(AL_MATH_USE_AVX2)
	return _mm_add_ps(lhs.xyz, rhs.xyz);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3 operator - (const Vec3& lhs, const Vec3& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	return
		Vec3(
			lhs.x - rhs.x,
			lhs.y - rhs.y,
			lhs.z - rhs.z);
#elif defined(AL_MATH_USE_AVX2)
	return _mm_sub_ps(lhs.xyz, rhs.xyz);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3 operator - (const Vec3& v)
{
#if defined(AL_MATH_USE_NO_SIMD)
	return  Vec3(-v.x, -v.y, -v.z);
#elif defined(AL_MATH_USE_AVX2)
	return _mm_sub_ps(_mm_setzero_ps(), v.xyz);
#endif
}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3& operator += (Vec3& lhs, const Vec3& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	lhs = lhs + rhs;
	return lhs;
#elif defined(AL_MATH_USE_AVX2)
	lhs.xyz = _mm_add_ps(lhs.xyz, rhs.xyz);
	return lhs;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3& operator -= (Vec3& lhs, const Vec3& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	lhs = lhs - rhs;
	return lhs;
#elif defined(AL_MATH_USE_AVX2)
	lhs.xyz = _mm_sub_ps(lhs.xyz, rhs.xyz);
	return lhs;
#endif
}
//-------------------------------------------------
//
//-------------------------------------------------
INLINE static BoolInVec operator == (const Vec3& lhs, const Vec3& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	return
		(lhs.x == rhs.x) &&
		(lhs.y == rhs.y) &&
		(lhs.z == rhs.z);
#elif defined(AL_MATH_USE_AVX2)
	const __m128 mask = _mm_cmpeq_ps(lhs.xyz, rhs.xyz);
	const int32_t maskPacked = _mm_movemask_ps(mask);
	return (maskPacked == 0x0F);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static BoolInVec operator != (const Vec3& lhs, const Vec3& rhs)
{
    return !(lhs == rhs);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3 operator * (float f, const Vec3& v)
{
#if defined(AL_MATH_USE_NO_SIMD)
	return
		Vec3(
			f * v.x,
			f * v.y,
			f * v.z);
#elif defined(AL_MATH_USE_AVX2)
	const __m128 s = _mm_set1_ps(f);
	return _mm_mul_ps(s, v.xyz);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3 operator * (const Vec3& v, float f)
{
#if defined(AL_MATH_USE_NO_SIMD)
	return f*v;
#elif defined(AL_MATH_USE_AVX2)
	const __m128 s = _mm_set1_ps(f);
	return _mm_mul_ps(v.xyz, s);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3& operator *= (Vec3& v, float f)
{
#if defined(AL_MATH_USE_NO_SIMD)
	v = v * f;
	return v;
#elif defined(AL_MATH_USE_AVX2)
	const __m128 s = _mm_set1_ps(f);
	v.xyz = _mm_mul_ps(s, v.xyz);
	return v;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3 operator / (const Vec3& v, float f)
{
#if defined(AL_MATH_USE_NO_SIMD)
	const float inv = 1.0f / f;
	return Vec3(v.x * inv, v.y * inv, v.z * inv);
#elif defined(AL_MATH_USE_AVX2)
	const __m128 s = _mm_rcp_ps(_mm_set1_ps(f));
	return _mm_mul_ps(v.xyz, s);
#endif
}



//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3Pack8::Vec3Pack8(
	__m256 axs,
	__m256 ays,
	__m256 azs)
	:xs(axs), ys(ays), zs(azs)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static __m256 combine128(__m128 a, __m128 b)
{
	const __m256 tmp = _mm256_castps128_ps256(a);
	return _mm256_insertf128_ps(tmp, b, 1);
}

//-------------------------------------------------
// xyzが8回連続で続くfloat配列からの構築
// 参考: http://www.isus.jp/specials/3d-vector-normalization/
//-------------------------------------------------
INLINE void Vec3Pack8::set(
	// xyzが8回
	float* xyz8)
{
	const __m128 x0y0z0x1 = _mm_load_ps(xyz8 + 0);
	const __m128 y1z1x2y2 = _mm_load_ps(xyz8 + 4);
	const __m128 z2x3y3z3 = _mm_load_ps(xyz8 + 8);
	const __m128 x4y4z4x5 = _mm_load_ps(xyz8 + 12);
	const __m128 y5z5x6y6 = _mm_load_ps(xyz8 + 16);
	const __m128 z6x7y7z7 = _mm_load_ps(xyz8 + 20);
	const __m256 x0y0z0x1x4y4z4x5 = combine128(x0y0z0x1, x4y4z4x5);
	const __m256 y1z1x2y2y5z5x6y6 = combine128(y1z1x2y2, y5z5x6y6);
	const __m256 z2x3y3z3z6x7y7z7 = combine128(z2x3y3z3, z6x7y7z7);
	const __m256 x2y2x3y3x6y6x7y7 = _mm256_shuffle_ps(y1z1x2y2y5z5x6y6, z2x3y3z3z6x7y7z7, _MM_SHUFFLE(2, 1, 3, 2));
	const __m256 y0z0y1z1y4z4y5z5 = _mm256_shuffle_ps(x0y0z0x1x4y4z4x5, y1z1x2y2y5z5x6y6, _MM_SHUFFLE(1, 0, 2, 1));
	xs = _mm256_shuffle_ps(x0y0z0x1x4y4z4x5, x2y2x3y3x6y6x7y7, _MM_SHUFFLE(2, 0, 3, 0));
	ys = _mm256_shuffle_ps(y0z0y1z1y4z4y5z5, x2y2x3y3x6y6x7y7, _MM_SHUFFLE(3, 1, 2, 0));
	zs = _mm256_shuffle_ps(y0z0y1z1y4z4y5z5, z2x3y3z3z6x7y7z7, _MM_SHUFFLE(3, 0, 3, 1));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Vec3Pack8::set(
	const Vec3& v0,
	const Vec3& v1,
	const Vec3& v2,
	const Vec3& v3,
	const Vec3& v4,
	const Vec3& v5,
	const Vec3& v6,
	const Vec3& v7)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 x0y0z0w0_x4y4z4w4 = combine128(v0.xyz, v4.xyz);
	const __m256 x1y1z1w1_x5y5z5w5 = combine128(v1.xyz, v5.xyz);
	const __m256 x2y2z2w2_x6y6z6w6 = combine128(v2.xyz, v6.xyz);
	const __m256 x3y3z3w3_x7y7z7w7 = combine128(v3.xyz, v7.xyz);
	const __m256 x0x2y0y2_x4x6y4y6 = _mm256_unpacklo_ps(x0y0z0w0_x4y4z4w4, x2y2z2w2_x6y6z6w6);
	const __m256 z0z2w0w2_z4z6w4w6 = _mm256_unpackhi_ps(x0y0z0w0_x4y4z4w4, x2y2z2w2_x6y6z6w6);
	const __m256 x1x3y1y3_x5x7x5y7 = _mm256_unpacklo_ps(x1y1z1w1_x5y5z5w5, x3y3z3w3_x7y7z7w7);
	const __m256 z1z3w1w3_z5z7w5w7 = _mm256_unpackhi_ps(x1y1z1w1_x5y5z5w5, x3y3z3w3_x7y7z7w7);
	const __m256 x0x1x2x3_x4x5x6x7 = _mm256_unpacklo_ps(x0x2y0y2_x4x6y4y6, x1x3y1y3_x5x7x5y7);
	const __m256 y0y1y2y3_y4y5y6y7 = _mm256_unpackhi_ps(x0x2y0y2_x4x6y4y6, x1x3y1y3_x5x7x5y7);
	const __m256 z0z1z2z3_z4z5z6z7 = _mm256_unpacklo_ps(z0z2w0w2_z4z6w4w6, z1z3w1w3_z5z7w5w7);
	xs = x0x1x2x3_x4x5x6x7;
	ys = y0y1y2y3_y4y5y6y7;
	zs = z0z1z2z3_z4z5z6z7;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Vec3Pack8::setZero()
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	xs = _mm256_setzero_ps();
	ys = _mm256_setzero_ps();
	zs = _mm256_setzero_ps();
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Bool8 Vec3Pack8::isZero() const
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 zero = _mm256_setzero_ps();
	const __m256 eqx = _mm256_cmp_ps(xs, zero, _CMP_EQ_OQ);
	const __m256 eqy = _mm256_cmp_ps(xs, zero, _CMP_EQ_OQ);
	const __m256 eqz = _mm256_cmp_ps(xs, zero, _CMP_EQ_OQ);
	const __m256 tmp0 = _mm256_or_ps(eqx, eqy);
	const __m256 tmp1 = _mm256_or_ps(tmp0, eqz);
	return tmp1;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Bool8 Vec3Pack8::hasNan() const
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 maskx = _mm256_cmp_ps(xs, xs, _CMP_NEQ_OQ);
	const __m256 masky = _mm256_cmp_ps(ys, ys, _CMP_NEQ_OQ);
	const __m256 maskz = _mm256_cmp_ps(zs, zs, _CMP_NEQ_OQ);
	const __m256 tmp0 = _mm256_or_ps(maskx, masky);
	const __m256 tmp1 = _mm256_or_ps(tmp0, maskz);
	return tmp1;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3Pack8& Vec3Pack8::normalize()
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
    AL_ASSERT_ALWAYS(false);
#if 0
	const __m256 len = lengthSq();
	const __m256 inv = _mm256_rsqrt_ps(len);
	xs = _mm256_mul_ps(xs, inv);
	ys = _mm256_mul_ps(ys, inv);
    zs = _mm256_mul_ps(zs, inv);
#endif
#endif
	return *this;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Vec3Pack8::scale(float v)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 vs = _mm256_set1_ps(v);
	xs = _mm256_mul_ps(xs, vs);
	ys = _mm256_mul_ps(ys, vs);
	zs = _mm256_mul_ps(zs, vs);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Bool8 Vec3Pack8::isNormalized() const
{
	return isNormalized(1.0e-5f);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Bool8 Vec3Pack8::isNormalized(float eps) const
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
    AL_ASSERT_ALWAYS(false);
#if 0
	const __m256 one = _mm256_set1_ps(1.0f);
	const __m256 epss = _mm256_set1_ps(eps);
	const __m256 dif = _mm256_sub_ps(length(), one);
	const __m256 mask = _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF));
	const __m256 difAbs = _mm256_and_ps(mask, dif);
	const __m256 tmp = _mm256_cmp_ps(difAbs, epss, _CMP_LT_OQ);
	return tmp;
#endif
    return Bool8();
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Float8 Vec3Pack8::length(const Vec3Pack8& v)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	return _mm256_sqrt_ps(lengthSq(v));
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Float8 Vec3Pack8::lengthSq(const Vec3Pack8& v)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 xs = v.xs;
	const __m256 ys = v.ys;
	const __m256 zs = v.zs;
	const __m256 xs2 = _mm256_dp_ps(xs, xs, 0xFF);
	const __m256 ys2 = _mm256_dp_ps(ys, ys, 0xFF);
	const __m256 zs2 = _mm256_dp_ps(zs, zs, 0xFF);
	return _mm256_add_ps(_mm256_add_ps(xs2, ys2), zs2);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Float8 Vec3Pack8::distance(const Vec3Pack8& lhs, const Vec3Pack8& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	return _mm256_sqrt_ps(distanceSq(lhs, rhs));
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Float8 Vec3Pack8::distanceSq(
	const Vec3Pack8& lhs,
	const Vec3Pack8& rhs)
{
#if !defined(WINDOWS)
    AL_ASSERT_ALWAYS(false);
    return Float8();
#else
    
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 dx = _mm256_sub_ps(lhs.xs, rhs.xs);
	const __m256 dy = _mm256_sub_ps(lhs.ys, rhs.ys);
	const __m256 dz = _mm256_sub_ps(lhs.zs, rhs.zs);
	return lengthSq({ dx, dy, dz });
#endif
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Float8 Vec3Pack8::dot(
	const Vec3Pack8& lhs,
	const Vec3Pack8& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 tmp0 = _mm256_mul_ps(lhs.xs, rhs.xs);
	const __m256 tmp1 = _mm256_mul_ps(lhs.ys, rhs.ys);
	const __m256 tmp2 = _mm256_mul_ps(lhs.zs, rhs.zs);
	const __m256 tmp3 = _mm256_add_ps(tmp0, tmp1);
	return _mm256_add_ps(tmp2, tmp3);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8 cross(
	const Vec3Pack8& lhs,
	const Vec3Pack8& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	Vec3Pack8 ret;
	ret.xs = _mm256_sub_ps(_mm256_mul_ps(lhs.ys, rhs.zs), _mm256_mul_ps(lhs.zs, rhs.ys));
	ret.ys = _mm256_sub_ps(_mm256_mul_ps(lhs.zs, rhs.xs), _mm256_mul_ps(lhs.xs, rhs.zs));
	ret.zs = _mm256_sub_ps(_mm256_mul_ps(lhs.xs, rhs.ys), _mm256_mul_ps(lhs.ys, rhs.xs));
	return ret;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8 operator + (const Vec3Pack8& lhs, const Vec3Pack8& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	Vec3Pack8 ret;
	ret.xs = _mm256_add_ps(lhs.xs, rhs.xs);
	ret.ys = _mm256_add_ps(lhs.ys, rhs.ys);
	ret.zs = _mm256_add_ps(lhs.zs, rhs.zs);
	return ret;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8 operator - (const Vec3Pack8& lhs, const Vec3Pack8& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	Vec3Pack8 ret;
	ret.xs = _mm256_sub_ps(lhs.xs, rhs.xs);
	ret.ys = _mm256_sub_ps(lhs.ys, rhs.ys);
	ret.zs = _mm256_sub_ps(lhs.zs, rhs.zs);
	return ret;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8 operator - (const Vec3Pack8& v)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	Vec3Pack8 ret;
	const __m256 zero = _mm256_setzero_ps();
	ret.xs = _mm256_sub_ps(zero, v.xs);
	ret.ys = _mm256_sub_ps(zero, v.ys);
	ret.zs = _mm256_sub_ps(zero, v.zs);
	return ret;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8& operator += (Vec3Pack8& lhs, const Vec3Pack8& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	lhs.xs = _mm256_add_ps(lhs.xs, rhs.xs);
	lhs.ys = _mm256_add_ps(lhs.ys, rhs.ys);
	lhs.zs = _mm256_add_ps(lhs.zs, rhs.zs);
	return lhs;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8& operator -= (Vec3Pack8& lhs, const Vec3Pack8& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	lhs.xs = _mm256_sub_ps(lhs.xs, rhs.xs);
	lhs.ys = _mm256_sub_ps(lhs.ys, rhs.ys);
	lhs.zs = _mm256_sub_ps(lhs.zs, rhs.zs);
	return lhs;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Bool8 operator == (const Vec3Pack8& lhs, const Vec3Pack8& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 tmp0 = _mm256_cmp_ps(lhs.xs, rhs.xs, _CMP_NEQ_OQ);
	const __m256 tmp1 = _mm256_cmp_ps(lhs.ys, rhs.ys, _CMP_NEQ_OQ);
	const __m256 tmp2 = _mm256_cmp_ps(lhs.zs, rhs.zs, _CMP_NEQ_OQ);
	const __m256 tmp3 = _mm256_or_ps(tmp0, tmp1);
	const __m256 tmp4 = _mm256_or_ps(tmp2, tmp3);
	return tmp4;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8 operator * (float f, const Vec3Pack8& v)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 s = _mm256_set1_ps(f);
	Vec3Pack8 ret;
	ret.xs = _mm256_mul_ps(s, v.xs);
	ret.ys = _mm256_mul_ps(s, v.ys);
	ret.zs = _mm256_mul_ps(s, v.zs);
	return ret;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8 operator * (const Vec3Pack8& v, float f)
{
	return f * v;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8& operator *= (Vec3Pack8& v, float f)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 s = _mm256_set1_ps(f);
	v.xs = _mm256_mul_ps(s, v.xs);
	v.ys = _mm256_mul_ps(s, v.ys);
	v.zs = _mm256_mul_ps(s, v.zs);
	return v;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Vec3Pack8 operator / (const Vec3Pack8& v, float f)
{
#if defined(AL_MATH_USE_NO_SIMD)
	assert(false);
#elif defined(AL_MATH_USE_AVX2)
	const __m256 s = _mm256_set1_ps(1.0f / f);
	Vec3Pack8 ret;
	ret.xs = _mm256_mul_ps(s, v.xs);
	ret.ys = _mm256_mul_ps(s, v.ys);
	ret.zs = _mm256_mul_ps(s, v.zs);
	return ret;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec4::Vec4(_In_reads_(4) const float* es)
	:x(es[0]), y(es[1]), z(es[2]), w(es[3])
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec4::Vec4(float ax, float ay, float az, float aw)
	:x(ax), y(ay), z(az), w(aw)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec4::Vec4(float e)
	:x(e), y(e), z(e), w(e)
{
#if defined(AL_MATH_USE_NO_SIMD)
	x = e;
	y = e;
	z = e;
	w = e;
#elif defined(AL_MATH_USE_AVX2)
	assert(false);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec4::Vec4(__m128 other)
{
#if defined(AL_MATH_USE_NO_SIMD)
	x = other.m128_f32[0];
	y = other.m128_f32[1];
	z = other.m128_f32[2];
	w = other.m128_f32[3];
#elif defined(AL_MATH_USE_AVX2)
	assert(false);
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float Vec4::operator[](int32_t index) const
{
    AL_ASSERT_DEBUG(0 <= index && index <= 3);
    return *(&x+ index);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float Vec4::dot(const Vec4& lhs, const Vec4& rhs)
{
	return
		lhs.x * rhs.x +
		lhs.y * rhs.y +
		lhs.z * rhs.z +
		lhs.w * rhs.w;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix3x3::Matrix3x3(_In_reads_(9) const float* es)
{
    e[0] = es[0];
    e[1] = es[1];
    e[2] = es[2];
    e[3] = es[3];
    e[4] = es[4];
    e[5] = es[5];
    e[6] = es[6];
    e[7] = es[7];
    e[8] = es[8];
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix3x3::Matrix3x3(
    float ae11, float ae12, float ae13,
    float ae21, float ae22, float ae23,
    float ae31, float ae32, float ae33)
{
    e11 = ae11;
    e12 = ae12;
    e13 = ae13;
    e21 = ae21;
    e22 = ae22;
    e23 = ae23;
    e31 = ae31;
    e32 = ae32;
    e33 = ae33;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float Matrix3x3::det() const
{
    const float d = 
        (e11 * e22 * e33 + e21 * e32 * e13 + e31 * e12 * e23) -
        (e11 * e32 * e23 + e31 * e22 * e13 + e21 * e12 * e33);
    return d;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix3x3::inverse()
{
    *this = inversed();
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix3x3::transpose()
{
    std::swap(e12, e21);
    std::swap(e13, e31);
    std::swap(e23, e32);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix3x3 Matrix3x3::inversed() const
{
    Matrix3x3 m;
    const float id = 1.0f / det();
    m.e11 = (e22 * e33 - e23 * e32) * id;
    m.e12 = (e13 * e32 - e12 * e33) * id;
    m.e13 = (e12 * e23 - e13 * e22) * id;
    m.e21 = (e23 * e31 - e21 * e33) * id;
    m.e22 = (e11 * e33 - e13 * e31) * id;
    m.e23 = (e13 * e21 - e11 * e23) * id;
    m.e31 = (e21 * e32 - e22 * e31) * id;
    m.e32 = (e12 * e31 - e11 * e32) * id;
    m.e33 = (e11 * e22 - e12 * e21) * id;
    return m;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix3x3 Matrix3x3::transposed() const
{
    Matrix3x3 m = *this;
    m.transpose();
    return m;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix4x4::Matrix4x4(const Matrix3x3& m33)
{
    e11 = m33.e11; e12 = m33.e12; e13 = m33.e13; e14 = 0.0f;
    e21 = m33.e21; e22 = m33.e22; e23 = m33.e23; e24 = 0.0f;
    e31 = m33.e31; e32 = m33.e32; e33 = m33.e33; e34 = 0.0f;    
    e41 = 0.0f;    e42 = 0.0f;    e43 = 0.0f;    e44 = 1.0f;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix4x4::Matrix4x4(_In_reads_(16) const float* es)
{
	e[0] = es[0];
	e[1] = es[1];
	e[2] = es[2];
	e[3] = es[3];
	e[4] = es[4];
	e[5] = es[5];
	e[6] = es[6];
	e[7] = es[7];
	e[8] = es[8];
	e[9] = es[9];
	e[10] = es[10];
	e[11] = es[11];
	e[12] = es[12];
	e[13] = es[13];
	e[14] = es[14];
	e[15] = es[15];
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix4x4::constructAsProjectionLH()
{
	// TODO: 実装
	assert(false);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix4x4::constructAsRotationAxis()
{
	// TODO: 実装
	assert(false);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix4x4::constructAsTranslation(const Vec3& v)
{
	const float x = v.x;
	const float y = v.y;
	const float z = v.z;
    e11 = 1.0f; e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
    e21 = 0.0f; e22 = 1.0f; e23 = 0.0f; e24 = 0.0f;
    e31 = 0.0f; e32 = 0.0f; e33 = 1.0f; e34 = 0.0f;
    e41 = x;    e42 = y;    e43 = z;    e44 = 1.0f;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix4x4::constructAsScale(const Vec3& scale)
{
	const float sx = scale.x;
	const float sy = scale.y;
	const float sz = scale.z;
	e11 = sx;   e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
	e21 = 0.0f; e22 = sy;   e23 = 0.0f; e24 = 0.0f;
	e31 = 0.0f; e32 = 0.0f; e33 = sz;   e34 = 0.0f;
	e41 = 0.0f; e42 = 0.0f; e43 = 0.0f; e44 = 1.0f;
}

//-------------------------------------------------
// http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/
//-------------------------------------------------
INLINE void Matrix4x4::constructAsRotation(const Vec3& xyz, float angle)
{
	//
	const float s = std::sinf(angle);
	const float c = std::cosf(angle);
	const float t = 1.0f - c;
	const float x = xyz.x;
	const float y = xyz.y;
	const float z = xyz.z;
	const float tx = t * x;
	const float ty = t * y;
	const float tz = t * z;
	const float txx = tx * x;
	const float txy = tx * y;
	const float txz = tx * z;
	const float tyy = ty * y;
	const float tyz = ty * z;
	const float tzz = tz * z;
	const float sx = s * x;
	const float sy = s * y;
	const float sz = s * z;
	//
	e11 = c + txx;  e12 = txy - sz;  e13 = txy + sy; e14 = 0.0f;
	e21 = txy + sz; e22 = c + tyy;   e23 = tyz - sx; e24 = 0.0f;
	e31 = txz - sy; e32 = tyz + sx;  e33 = c + tzz;  e34 = 0.0f;
	e41 = 0.0f;     e42 = 0.0f;      e43 = 0.0f;     e44 = 1.0f;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix4x4::constructAsViewMatrix(const Vec3& origin, const Vec3& target, const Vec3& up)
{
#if !defined(WINDOWS)
    AL_ASSERT_ALWAYS(false);
#else
	const Vec3 zaxis = (target - origin).normalized();
	const Vec3 xaxis = Vec3::cross(up, zaxis).normalized();
	const Vec3 yaxis = Vec3::cross(zaxis, xaxis);
	e11 = xaxis.x; e12 = yaxis.x; e13 = zaxis.x; e14 = 0.0f;
	e21 = xaxis.y; e22 = yaxis.y; e23 = zaxis.y; e24 = 0.0f;
	e31 = xaxis.z; e32 = yaxis.z; e33 = zaxis.z; e34 = 0.0f;
	e41 = -Vec3::dot(xaxis, origin);
	e42 = -Vec3::dot(yaxis, origin);
	e43 = -Vec3::dot(zaxis, origin);
	e44 = 1.0f;
#endif
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix4x4::fillZero()
{
	e11 = 0.0f; e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
	e21 = 0.0f; e22 = 0.0f; e23 = 0.0f; e24 = 0.0f;
	e31 = 0.0f; e32 = 0.0f; e33 = 0.0f; e34 = 0.0f;
	e41 = 0.0f; e42 = 0.0f; e43 = 0.0f; e44 = 1.0f;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix4x4::identity()
{
	e11 = 1.0f; e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
	e21 = 0.0f; e22 = 1.0f; e23 = 0.0f; e24 = 0.0f;
	e31 = 0.0f; e32 = 0.0f; e33 = 1.0f; e34 = 0.0f;
	e41 = 0.0f; e42 = 0.0f; e43 = 0.0f; e44 = 1.0f;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 Matrix4x4::transform(const Vec3& v) const
{
	const Vec4 nv(v.x, v.y, v.z, 1.0f);
	return
		Vec3(
			Vec4::dot(columnVector(0), nv),
			Vec4::dot(columnVector(1), nv),
			Vec4::dot(columnVector(2), nv));
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec4 Matrix4x4::transform(const Vec4& v) const
{
	return
		Vec4(
			Vec4::dot(columnVector(0), v),
			Vec4::dot(columnVector(1), v),
			Vec4::dot(columnVector(2), v),
			Vec4::dot(columnVector(3), v));
}

//-------------------------------------------------
// operator[]
// RawVectorへのアクセス
//-------------------------------------------------
INLINE const Vec4& Matrix4x4::operator [] (int32_t index) const
{
	AL_ASSERT_DEBUG(0 <= index && index <= 3);
	return *(Vec4*)(e + index * 4);
}

//-------------------------------------------------
// operator[]
// RawVectorへのアクセス
//-------------------------------------------------
INLINE Vec4& Matrix4x4::operator [] (int32_t index)
{
	AL_ASSERT_DEBUG(0 <= index && index <= 3);
	return *(Vec4*)(e + index * 4);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec4 Matrix4x4::rowVector(int32_t index) const
{
    AL_ASSERT_DEBUG(0 <= index && index <= 3);
    return Vec4(e + index * 4);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec4 Matrix4x4::columnVector(int32_t index) const
{
	AL_ASSERT_DEBUG(0 <= index && index <= 3);
	return Vec4(
		*(&e11 + index),
		*(&e21 + index),
		*(&e31 + index),
		*(&e41 + index)
	);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE float Matrix4x4::det() const
{
    // 一列目を余因子展開
    const float d1 =
        Matrix3x3(
            e22, e23, e24,
            e32, e33, e34,
            e42, e43, e44).det();
    const float d2 =
        Matrix3x3(
            e21, e23, e24,
            e31, e33, e34,
            e41, e43, e44).det();
    const float d3 =
        Matrix3x3(
            e21, e22, e24,
            e31, e32, e34,
            e41, e42, e44).det();
    const float d4 =
        Matrix3x3(
            e21, e22, e23,
            e31, e32, e33,
            e41, e42, e43).det();
    const float d = 
        + e11 * d1
        - e12 * d2
        + e13 * d3
        - e14 * d4;
	return d;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix4x4::inverse()
{
    const float d = det();
    const float id = 1.0f / d;
    // 小行列(1行目)
    const Matrix3x3 m11 =
        Matrix3x3(
            e22, e23, e24,
            e32, e33, e34,
            e42, e43, e44);
    const Matrix3x3 m12 =
        Matrix3x3(
            e21, e23, e24,
            e31, e33, e34,
            e41, e43, e44);
    const Matrix3x3 m13 =
        Matrix3x3(
            e21, e22, e24,
            e31, e32, e34,
            e41, e42, e44);
    const Matrix3x3 m14 =
        Matrix3x3(
            e21, e22, e23,
            e31, e32, e33,
            e41, e42, e43);

    // 小行列(2行目)
    const Matrix3x3 m21 =
        Matrix3x3(
            e12, e13, e14,
            e32, e33, e34,
            e42, e43, e44);
    const Matrix3x3 m22 =
        Matrix3x3(
            e11, e13, e14,
            e31, e33, e34,
            e41, e43, e44);
    const Matrix3x3 m23 =
        Matrix3x3(
            e11, e12, e14,
            e31, e32, e34,
            e41, e42, e44);
    const Matrix3x3 m24 =
        Matrix3x3(
            e11, e12, e13,
            e31, e32, e33,
            e41, e42, e43);

    // 小行列(3行目)
    const Matrix3x3 m31 =
        Matrix3x3(
            e12, e13, e14,
            e22, e23, e24,
            e42, e43, e44);
    const Matrix3x3 m32 =
        Matrix3x3(
            e11, e13, e14,
            e21, e23, e24,
            e41, e43, e44);
    const Matrix3x3 m33 =
        Matrix3x3(
            e11, e12, e14,
            e21, e22, e24,
            e41, e42, e44);
    const Matrix3x3 m34 =
        Matrix3x3(
            e11, e12, e13,
            e21, e22, e23,
            e41, e42, e43);

    // 小行列(4行目)
    const Matrix3x3 m41 =
        Matrix3x3(
            e12, e13, e14,
            e22, e23, e24,
            e32, e33, e34);
    const Matrix3x3 m42 =
        Matrix3x3(
            e11, e13, e14,
            e21, e23, e24,
            e31, e33, e34);
    const Matrix3x3 m43 =
        Matrix3x3(
            e11, e12, e14,
            e21, e22, e24,
            e31, e32, e34);
    const Matrix3x3 m44 =
        Matrix3x3(
            e11, e12, e13,
            e21, e22, e23,
            e31, e32, e33);
    //
    Matrix4x4 r;
    r.e11 = id * m11.det() * (+1.0f);
    r.e12 = id * m12.det() * (-1.0f);
    r.e13 = id * m13.det() * (+1.0f);
    r.e14 = id * m14.det() * (-1.0f);
    r.e21 = id * m21.det() * (-1.0f);
    r.e22 = id * m22.det() * (+1.0f);
    r.e23 = id * m23.det() * (-1.0f);
    r.e24 = id * m24.det() * (+1.0f);
    r.e31 = id * m31.det() * (+1.0f);
    r.e32 = id * m32.det() * (-1.0f);
    r.e33 = id * m33.det() * (+1.0f);
    r.e34 = id * m34.det() * (-1.0f);
    r.e41 = id * m41.det() * (-1.0f);
    r.e42 = id * m42.det() * (+1.0f);
    r.e43 = id * m43.det() * (-1.0f);
    r.e44 = id * m44.det() * (+1.0f);
    // TODO: transposeが無駄なので最初からひっくり返しておく
    *this = r.transposed();
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE void Matrix4x4::transpose()
{
    std::swap(e12, e21);
    std::swap(e13, e31);
    std::swap(e14, e41);
    std::swap(e23, e32);
    std::swap(e24, e42);
    std::swap(e34, e43);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix4x4 Matrix4x4::inversed() const
{
    Matrix4x4 m = *this;
    m.inverse();
    return m;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix4x4 Matrix4x4::transposed() const
{
    Matrix4x4 m = *this;
    m.transpose();
    return m;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix3x3 Matrix4x4::extract3x3() const
{
    Matrix3x3 m;
    m.e11 = e11;
    m.e12 = e12;
    m.e13 = e13;
    m.e21 = e21;
    m.e22 = e22;
    m.e23 = e23;
    m.e31 = e31;
    m.e32 = e32;
    m.e33 = e33;
    return m;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Matrix4x4 Matrix4x4::mul(const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	Matrix4x4 r;
	r.e11 = lhs.e11 * rhs.e11 + lhs.e12 * rhs.e21 + lhs.e13 * rhs.e31 + lhs.e14 * rhs.e41;
	r.e12 = lhs.e11 * rhs.e12 + lhs.e12 * rhs.e22 + lhs.e13 * rhs.e32 + lhs.e14 * rhs.e42;
	r.e13 = lhs.e11 * rhs.e13 + lhs.e12 * rhs.e23 + lhs.e13 * rhs.e33 + lhs.e14 * rhs.e43;
	r.e14 = lhs.e11 * rhs.e14 + lhs.e12 * rhs.e24 + lhs.e13 * rhs.e34 + lhs.e14 * rhs.e44;
	//
	r.e21 = lhs.e21 * rhs.e11 + lhs.e22 * rhs.e21 + lhs.e23 * rhs.e31 + lhs.e24 * rhs.e41;
	r.e22 = lhs.e21 * rhs.e12 + lhs.e22 * rhs.e22 + lhs.e23 * rhs.e32 + lhs.e24 * rhs.e42;
	r.e23 = lhs.e21 * rhs.e13 + lhs.e22 * rhs.e23 + lhs.e23 * rhs.e33 + lhs.e24 * rhs.e43;
	r.e24 = lhs.e21 * rhs.e14 + lhs.e22 * rhs.e24 + lhs.e23 * rhs.e34 + lhs.e24 * rhs.e44;
	//
	r.e31 = lhs.e31 * rhs.e11 + lhs.e32 * rhs.e21 + lhs.e33 * rhs.e31 + lhs.e34 * rhs.e41;
	r.e32 = lhs.e31 * rhs.e12 + lhs.e32 * rhs.e22 + lhs.e33 * rhs.e32 + lhs.e34 * rhs.e42;
	r.e33 = lhs.e31 * rhs.e13 + lhs.e32 * rhs.e23 + lhs.e33 * rhs.e33 + lhs.e34 * rhs.e43;
	r.e34 = lhs.e31 * rhs.e14 + lhs.e32 * rhs.e24 + lhs.e33 * rhs.e34 + lhs.e34 * rhs.e44;
	//
	r.e41 = lhs.e41 * rhs.e11 + lhs.e42 * rhs.e21 + lhs.e43 * rhs.e31 + lhs.e44 * rhs.e41;
	r.e42 = lhs.e41 * rhs.e12 + lhs.e42 * rhs.e22 + lhs.e43 * rhs.e32 + lhs.e44 * rhs.e42;
	r.e43 = lhs.e41 * rhs.e13 + lhs.e42 * rhs.e23 + lhs.e43 * rhs.e33 + lhs.e44 * rhs.e43;
	r.e44 = lhs.e41 * rhs.e14 + lhs.e42 * rhs.e24 + lhs.e43 * rhs.e34 + lhs.e44 * rhs.e44;
	//
	return r;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec4 Matrix4x4::mul(const Vec4& v, const Matrix4x4& m)
{
	Vec4 r;
	r.x = v.x * m.e11 + v.y * m.e12 + v.z * m.e13 + v.w * m.e14;
	r.y = v.x * m.e21 + v.y * m.e22 + v.z * m.e23 + v.w * m.e24;
	r.z = v.x * m.e31 + v.y * m.e32 + v.z * m.e33 + v.w * m.e34;
	r.w = v.x * m.e41 + v.y * m.e42 + v.z * m.e43 + v.w * m.e44;
	return r;
}

//-------------------------------------------------
//
//-------------------------------------------------
inline Matrix4x4& Matrix4x4::operator=(const Matrix4x4 & other)
{
	const float* oe = other.e;
	e[0] = oe[0];
	e[1] = oe[1];
	e[2] = oe[2];
	e[3] = oe[3];
	e[4] = oe[4];
	e[5] = oe[5];
	e[6] = oe[6];
	e[7] = oe[7];
	e[8] = oe[8];
	e[9] = oe[9];
	e[10] = oe[10];
	e[11] = oe[11];
	e[12] = oe[12];
	e[13] = oe[13];
	e[14] = oe[14];
	e[15] = oe[15];
	return *this;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE static Matrix4x4 operator + (const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	const float* e0 = lhs.e;
	const float* e1 = rhs.e;
	const float e[16] =
	{
		e0[0] + e1[0],
		e0[1] + e1[1],
		e0[2] + e1[2],
		e0[3] + e1[3],
		e0[4] + e1[4],
		e0[5] + e1[5],
		e0[6] + e1[6],
		e0[7] + e1[7],
		e0[8] + e1[8],
		e0[9] + e1[9],
		e0[10] + e1[10],
		e0[11] + e1[11],
		e0[12] + e1[12],
		e0[13] + e1[13],
		e0[14] + e1[14],
		e0[15] + e1[15],
	};
	return Matrix4x4(e);
}
