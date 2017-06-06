#pragma once
#include "pch.hpp"

// VectorMathでSIMDを使うか否か。AVX2まで仮定する。
//#define AL_MATH_USE_NO_SIMD
#define AL_MATH_USE_AVX2

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename Type>
FORCE_INLINE Type alLerp(const Type& lhs, const Type& rhs, float factor)
{
    const float t0 = 1.0f - factor;
    const float t1 = factor;
    return t0 * lhs + t1 * rhs;
}

template<typename Type>
FORCE_INLINE Type alLerp2(const Type lhs, const Type rhs, float factor)
{
    const float t0 = 1.0f - factor;
    const float t1 = factor;
    return t0 * lhs + t1 * rhs;
}

/*
-------------------------------------------------
isPowerOfTwo
-------------------------------------------------
*/
FORCE_INLINE static bool alIsPowerOfTwo(uint32_t x)
{
    return ((x != 0) && ((x & (~x + 1)) == x));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename T>
FORCE_INLINE static T alMin(T v0, T v1)
{
    return v0 < v1 ? v0 : v1;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<>
FORCE_INLINE float alMin(float v0, float v1)
{
#if 1
    _mm_store_ss(&v0, _mm_min_ss(_mm_set_ss(v0), _mm_set_ss(v1)));
    return v0;
#else
    return v0 < v1 ? v0 : v1;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename T>
FORCE_INLINE static T alMax(T v0, T v1)
{
    return v0 > v1 ? v0 : v1;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<>
FORCE_INLINE float alMax(float v0, float v1)
{
#if 1
    _mm_store_ss(&v0, _mm_max_ss(_mm_set_ss(v0), _mm_set_ss(v1)));
    return v0;
#else
    return v0 > v1 ? v0 : v1;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename T>
FORCE_INLINE static T alClamp(T v, T mn, T mx)
{
    return v > mx ? mx : (v < mn ? mn : v);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<>
FORCE_INLINE float alClamp(float v, float mn, float mx)
{
#if 1
    _mm_store_ss(&v, _mm_min_ss(_mm_max_ss(_mm_set_ss(v), _mm_set_ss(mn)), _mm_set_ss(mx)));
    return v;
#else
    return v > mx ? mx : (v < mn ? mn : v);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename T>
FORCE_INLINE static T alFabsf(T v)
{
    return v > -v ? v : -v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<>
FORCE_INLINE float alFabsf(float v)
{
    return std::fabsf(v);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
FORCE_INLINE static float alFmod(float x, float y)
{
    return std::fmodf(x, y);
}

/*
-------------------------------------------------
Size2D
-------------------------------------------------
*/
struct Size2D
{
public:
	int32_t width = 0;
	int32_t height = 0;
public:
	Size2D() = default;
	Size2D(const Size2D& other) = default;
	Size2D(Size2D&& other) = default;
	Size2D(_In_reads_(2) int32_t* es);
	Size2D(int32_t awidth, int32_t aheight);
	int32_t index(int32_t x, int32_t y) const;
};

INLINE static Size2D operator + (const Size2D& lhs, const Size2D& rhs);
INLINE static Size2D operator - (const Size2D& lhs, const Size2D& rhs);
INLINE static Size2D operator - (const Size2D& v);
INLINE static Size2D& operator += (Size2D& lhs, const Size2D& rhs);
INLINE static Size2D& operator -= (Size2D& lhs, const Size2D& rhs);
INLINE static bool operator == (const Size2D& lhs, const Size2D& rhs);
INLINE static Size2D operator * (int32_t f, const Size2D& v);
INLINE static Size2D operator * (const Size2D& v, int32_t f);
INLINE static Size2D& operator *= (Size2D& v, int32_t f);

/*
-------------------------------------------------
Region2D
-------------------------------------------------
*/
class Region2D
{
public:
	int32_t left = 0;
	int32_t top = 0;
	int32_t right = 0;
	int32_t bottom = 0;
public:
	Region2D() = default;
	Region2D(const Region2D& other) = default;
	Region2D(Region2D&& other) = default;
	Region2D(int32_t left, int32_t top, int32_t right, int32_t bottom);
	int32_t width() const;
	int32_t height() const;
	int32_t area() const;
	Region2D& operator = (const Region2D& other) = default;
};

/*
-------------------------------------------------
Vec2
SIMD化してもあまり実行速度が向上しないのと、
ランタイムであまり利用されていないのでSIMDは使わない
-------------------------------------------------
*/
struct Vec2
{
public:
	float x;
	float y;
public:
	Vec2() = default;
	Vec2(const Vec2& other) = default;
	Vec2(Vec2&& other) = default;
	Vec2(_In_reads_(2) float* es);
	Vec2(float ax, float ay);
	explicit Vec2(float e);
	void normalize();
	bool isNormalized() const;
	float length() const;
	float lengthSq() const;
	float& operator[](int32_t index);
	float operator[](int32_t index) const;
	bool hasNan() const;
	INLINE Vec2& operator = (const Vec2& other) = default;
	static float dot(const Vec2& lhs, const Vec2& rhs);
};

INLINE static Vec2 operator + (const Vec2& lhs, const Vec2& rhs);
INLINE static Vec2 operator - (const Vec2& lhs, const Vec2& rhs);
INLINE static Vec2 operator - (const Vec2& v);
INLINE static Vec2& operator += (Vec2& lhs, const Vec2& rhs);
INLINE static Vec2& operator -= (Vec2& lhs, const Vec2& rhs);
INLINE static bool operator == (const Vec2& lhs, const Vec2& rhs);
INLINE static Vec2 operator * (float f, const Vec2& v);
INLINE static Vec2 operator * (const Vec2& v, float f);
INLINE static Vec2& operator *= (Vec2& v, float factor);
INLINE static Vec2 operator / (const Vec2& v, float f);

/*
-------------------------------------------------
FloatInVec
0レーンにデータが入っていることのみを保証したデータ
-------------------------------------------------
*/
struct FloatInVec
{
public:
	__m128 v;
public:
	INLINE FloatInVec() = default;
	INLINE FloatInVec(const FloatInVec& other) = default;
	INLINE FloatInVec(FloatInVec&& other) = default;
	INLINE FloatInVec(__m128 v);
	INLINE FloatInVec(float v);
	INLINE operator __m128 () const;
	INLINE operator float() const;
	INLINE float value() const;
};

/*
-------------------------------------------------
Float8
-------------------------------------------------
*/
struct Float8
{
public:
	union
	{
		__m256 v;
		float e[8];
	};
	INLINE Float8() = default;
	INLINE Float8(const Float8& other) = default;
	INLINE Float8(Float8&& other) = default;
	INLINE Float8(__m256 other);
	operator __m256()const;
};

/*
-------------------------------------------------
Bool8
0xFFFFFFFFか0x00000000が各レーンに入っている
-------------------------------------------------
*/
struct Bool8
{
public:
	union
	{
		__m256 v;
		bool b[8];
	};
	INLINE Bool8() = default;
	INLINE Bool8(const Bool8& other) = default;
	INLINE Bool8(Bool8&& other) = default;
	INLINE Bool8(__m256 other);
	INLINE bool at(int32_t index) const;
};

/*
-------------------------------------------------
BoolInVec
0レーンにデータが入っていることのみを保証したデータ
-------------------------------------------------
*/
struct BoolInVec
{
public:
	__m128i v;
public:
	INLINE BoolInVec() = default;
	INLINE BoolInVec(const BoolInVec& other) = default;
	INLINE BoolInVec(BoolInVec&& other) = default;
	INLINE BoolInVec(__m128i v);
	INLINE BoolInVec(bool v);
	INLINE operator __m128i () const;
	INLINE operator bool() const;
	INLINE bool value() const;
};


/*
-------------------------------------------------
Vec3
-------------------------------------------------
*/
struct Vec4;
struct Vec3
{
public:
	union
	{
        struct
        {
            float x;
            float y;
            float z;
        };
		__m128 xyz;
	};
public:
	INLINE Vec3() = default;
	INLINE Vec3(const Vec3& other) = default;
	INLINE Vec3(Vec3&& other) = default;
	INLINE Vec3(_In_reads_(3) float* es);
	INLINE Vec3(float ax, float ay, float az);
	INLINE Vec3(float e);
	INLINE Vec3(__m128 other);
	INLINE Vec3(const std::array<float, 3>& arr);
    INLINE Vec3(const Vec4& arr);
	INLINE void zero();
	INLINE bool isZero() const;
	INLINE bool hasNan() const;
	INLINE Vec3& normalize();
	INLINE Vec3 normalized() const;
	INLINE void scale(float scale);
	INLINE bool isNormalized() const;
	INLINE bool isNormalized(float eps) const;
	INLINE FloatInVec length() const;
	INLINE FloatInVec lengthSq() const;
	INLINE bool isAllZero() const;
	INLINE Vec3 inverted() const;
	INLINE Vec3 invertedSafe(float defaultValue) const;
	INLINE Vec3 reflect(const Vec3& v) const;
    //
	INLINE float& operator[](int32_t index);
	INLINE float operator[](int32_t index) const;
	INLINE Vec3& operator = (const Vec3& other) = default;
    // swizzle
    INLINE Vec3 xxx() const;
    INLINE Vec3 xxy() const;
    INLINE Vec3 xxz() const;
    INLINE Vec3 xyx() const;
    INLINE Vec3 xyy() const;
    //INLINE Vec3 xyz() const;
    INLINE Vec3 xzx() const;
    INLINE Vec3 xzy() const;
    INLINE Vec3 xzz() const;
    INLINE Vec3 yxx() const;
    INLINE Vec3 yxy() const;
    INLINE Vec3 yxz() const;
    INLINE Vec3 yyx() const;
    INLINE Vec3 yyy() const;
    INLINE Vec3 yyz() const;
    INLINE Vec3 yzx() const;
    INLINE Vec3 yzy() const;
    INLINE Vec3 yzz() const;
    INLINE Vec3 zxx() const;
    INLINE Vec3 zxy() const;
    INLINE Vec3 zxz() const;
    INLINE Vec3 zyx() const;
    INLINE Vec3 zyy() const;
    INLINE Vec3 zyz() const;
    INLINE Vec3 zzx() const;
    INLINE Vec3 zzy() const;
    INLINE Vec3 zzz() const;
    //
	static FloatInVec length(const Vec3& v);
	static FloatInVec lengthSq(const Vec3& v);
	static FloatInVec distance(const Vec3& lhs, const Vec3& rhs);
	static FloatInVec distanceSq(const Vec3& lhs, const Vec3& rhs);
	static FloatInVec dot(const Vec3& lhs, const Vec3& rhs);
	static Vec3 cross(const Vec3& lhs, const Vec3& rhs);
    static Vec3 mul(const Vec3& lhs, const Vec3& rhs);
};

INLINE static Vec3 operator + (const Vec3& lhs, const Vec3& rhs);
INLINE static Vec3 operator - (const Vec3& lhs, const Vec3& rhs);
INLINE static Vec3 operator - (const Vec3& v);
INLINE static Vec3& operator += (Vec3& lhs, const Vec3& rhs);
INLINE static Vec3& operator -= (Vec3& lhs, const Vec3& rhs);
INLINE static BoolInVec operator == (const Vec3& lhs, const Vec3& rhs);
INLINE static BoolInVec operator != (const Vec3& lhs, const Vec3& rhs);
INLINE static Vec3 operator * (float f, const Vec3& v);
INLINE static Vec3 operator * (const Vec3& v, float f);
INLINE static Vec3& operator *= (Vec3& v, float factor);
INLINE static Vec3 operator / (const Vec3& v, float f);

/*
-------------------------------------------------
Vec3x8のデータ構造
-------------------------------------------------
*/
struct Vec3Pack8
{
public:
	union
	{
		struct
		{
			__m256 xs; // xxxxxxxx
			__m256 ys; // yyyyyyyy
			__m256 zs; // zzzzzzzz
		};
		struct
		{
			float x8[8];
			float y8[8];
			float z8[8];
		};
	};
public:
	Vec3Pack8() = default;
	Vec3Pack8(const Vec3Pack8& other) = default;
	Vec3Pack8(Vec3Pack8&& other) = default;
	Vec3Pack8(
		__m256 v0,
		__m256 v1,
		__m256 v2);
	INLINE void set(
		// xyzが8回
		float* xyzs);
	INLINE void set(
		const Vec3& v0,
		const Vec3& v1,
		const Vec3& v2,
		const Vec3& v3,
		const Vec3& v4,
		const Vec3& v5,
		const Vec3& v6,
		const Vec3& v7);
	INLINE void setZero();
	INLINE Bool8 isZero() const;
	INLINE Bool8 hasNan() const;
	INLINE Vec3Pack8& normalize();
	INLINE void scale(float scale);
	INLINE Bool8 isNormalized() const;
	INLINE Bool8 isNormalized(float eps) const;
	INLINE Float8 length() const;
	INLINE Float8 lengthSq() const;
	INLINE Bool8 isAllZero() const;
	INLINE Vec3Pack8 inverted() const;
	INLINE Vec3Pack8 invertedSafe(float defaultValue) const;
	INLINE Vec3Pack8 reflect(const Vec3& v) const;
	INLINE Float8 operator[](int32_t index) const;

	INLINE static Float8 length(const Vec3Pack8& v);
	INLINE static Float8 lengthSq(const Vec3Pack8& v);
	INLINE static Float8 distance(const Vec3Pack8& lhs, const Vec3Pack8& rhs);
	INLINE static Float8 distanceSq(const Vec3Pack8& lhs, const Vec3Pack8& rhs);
	INLINE static Float8 dot(const Vec3Pack8& lhs, const Vec3Pack8& rhs);
	INLINE static Vec3Pack8 cross(const Vec3Pack8& lhs, const Vec3Pack8& rhs);
};
INLINE static Vec3Pack8 operator + (const Vec3Pack8& lhs, const Vec3Pack8& rhs);
INLINE static Vec3Pack8 operator - (const Vec3Pack8& lhs, const Vec3Pack8& rhs);
INLINE static Vec3Pack8 operator - (const Vec3Pack8& v);
INLINE static Vec3Pack8& operator += (Vec3Pack8& lhs, const Vec3Pack8& rhs);
INLINE static Vec3Pack8& operator -= (Vec3Pack8& lhs, const Vec3Pack8& rhs);
INLINE static Bool8 operator == (const Vec3Pack8& lhs, const Vec3Pack8& rhs);
INLINE static Vec3Pack8 operator * (float f, const Vec3Pack8& v);
INLINE static Vec3Pack8 operator * (const Vec3Pack8& v, float f);
INLINE static Vec3Pack8& operator *= (Vec3Pack8& v, float factor);
INLINE static Vec3Pack8 operator / (const Vec3Pack8& v, float f);

/*
-------------------------------------------------
Vec4
-------------------------------------------------
*/
struct Vec4
{
public:
	union
	{
		__m128 xyzw;
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
	};
public:
	Vec4() = default;
	Vec4(const Vec4& other) = default;
	Vec4(Vec4&& other) = default;
	Vec4(_In_reads_(4) const float* es);
	Vec4(float x, float y, float z, float w);
	explicit Vec4(float e);
	Vec4(__m128 other);
    float operator[](int32_t index) const;
    Vec4& operator=(const Vec4& other)  = default;
	static float dot(const Vec4& lhs, const Vec4& rhs);
};


/*
-------------------------------------------------
Matrix3x3
Row-Major/Row-Vectorの4x4行列
-------------------------------------------------
*/
class Matrix3x3
{
public:
    union
    {
        float e[9];
        struct
        {
            float e11, e12, e13;
            float e21, e22, e23;
            float e31, e32, e33;
        };
    };
public:
    Matrix3x3() = default;
    Matrix3x3(const Matrix3x3& other) = default;
    Matrix3x3(Matrix3x3&& other) = default;
    Matrix3x3(_In_reads_(9) const float* es);
    Matrix3x3(
        float e11, float e12, float e13,
        float e21, float e22, float e23,
        float e31, float e32, float e33);
    float det() const;
    void inverse();
    void transpose();
    Matrix3x3 inversed() const;
    Matrix3x3 transposed() const;
    Matrix3x3& operator = (const Matrix3x3& other) = default;
};

/*
-------------------------------------------------
Matrix4x4
Row-Major/Row-Vectorの4x4行列
-------------------------------------------------
*/
class Matrix4x4
{
public:
	union
	{
		float e[16];
		struct
		{
			float e11, e12, e13, e14;
			float e21, e22, e23, e24;
			float e31, e32, e33, e34;
			float e41, e42, e43, e44;
		};
	};
public:
	Matrix4x4() = default;
	Matrix4x4(const Matrix4x4& other) = default;
	Matrix4x4(Matrix4x4&& other) = default;
    Matrix4x4(const Matrix3x3& m33);
	Matrix4x4(_In_reads_(16) const float* es);
	void constructAsProjectionLH();
	void constructAsRotationAxis();
	void constructAsTranslation(const Vec3& v);
	void constructAsScale(const Vec3& scale);
	void constructAsRotation(const Vec3& xyz, float angle);
	void constructAsViewMatrix(const Vec3& origin, const Vec3& target, const Vec3& up);
	void fillZero();
	void identity();
	Vec3 transform(const Vec3& v) const;
	Vec4 transform(const Vec4& v) const;
	const Vec4& operator [] (int32_t index) const;
	Vec4& operator [] (int32_t index);
    Vec4 rowVector(int32_t index) const;
	Vec4 columnVector(int32_t index) const;
	float det() const;
    void inverse();
    void transpose();
    Matrix4x4 inversed() const;
    Matrix4x4 transposed() const;
    Matrix3x3 extract3x3() const;
	static Matrix4x4 mul(const Matrix4x4& lhs, const Matrix4x4& rhs);
	static Vec4 mul(const Vec4& v, const Matrix4x4& m);
	Matrix4x4& operator = (const Matrix4x4& other);
};
INLINE static Matrix4x4 operator + (const Matrix4x4& lhs, const Matrix4x4& rhs);

#include "math.inl"
#include "swizzle.inl"
