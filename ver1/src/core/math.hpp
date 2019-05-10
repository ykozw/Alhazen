#pragma once

// VectorMathでSIMDを使うか否か。AVX2まで仮定する。
//#define AL_MATH_USE_NO_SIMD

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename Type>
FORCE_INLINE Type
alLerp(const Type& lhs, const Type& rhs, float factor)
{
    const float t0 = 1.0f - factor;
    const float t1 = factor;
    return t0 * lhs + t1 * rhs;
}

template<typename Type>
FORCE_INLINE Type
alLerp2(const Type lhs, const Type rhs, float factor)
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
FORCE_INLINE static bool
alIsPowerOfTwo(uint32_t x)
{
    return ((x != 0) && ((x & (~x + 1)) == x));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename T>
FORCE_INLINE static T
alMin(T v0, T v1)
{
    return v0 < v1 ? v0 : v1;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<>
FORCE_INLINE float
alMin(float v0, float v1)
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
FORCE_INLINE static T
alMax(T v0, T v1)
{
    return v0 > v1 ? v0 : v1;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<>
FORCE_INLINE float
alMax(float v0, float v1)
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
FORCE_INLINE static T
alClamp(T v, T mn, T mx)
{
    return v > mx ? mx : (v < mn ? mn : v);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<>
FORCE_INLINE float
alClamp(float v, float mn, float mx)
{
#if 1
    _mm_store_ss(
      &v,
      _mm_min_ss(_mm_max_ss(_mm_set_ss(v), _mm_set_ss(mn)), _mm_set_ss(mx)));
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
FORCE_INLINE static T
alFabsf(T v)
{
    return v > -v ? v : -v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<>
FORCE_INLINE float
alFabsf(float v)
{
    return std::fabsf(v);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
FORCE_INLINE static float
alFmod(float x, float y)
{
    return std::fmodf(x, y);
}

/*
-------------------------------------------------
Size2D
-------------------------------------------------
*/
struct ALIGN16 Size2D AL_FINAL
{
public:
    int32_t width = 0;
    int32_t height = 0;

public:
    Size2D() = default;
    Size2D(const Size2D& other) = default;
    Size2D(Size2D&& other) = default;
    Size2D(int32_t* es);
    Size2D(int32_t awidth, int32_t aheight);
    int32_t index(int32_t x, int32_t y) const;
};

INLINE static Size2D
operator+(const Size2D& lhs, const Size2D& rhs);
INLINE static Size2D
operator-(const Size2D& lhs, const Size2D& rhs);
INLINE static Size2D
operator-(const Size2D& v);
INLINE static Size2D&
operator+=(Size2D& lhs, const Size2D& rhs);
INLINE static Size2D&
operator-=(Size2D& lhs, const Size2D& rhs);
INLINE static bool
operator==(const Size2D& lhs, const Size2D& rhs);
INLINE static Size2D operator*(int32_t f, const Size2D& v);
INLINE static Size2D operator*(const Size2D& v, int32_t f);
INLINE static Size2D&
operator*=(Size2D& v, int32_t f);

/*
-------------------------------------------------
Region2D
-------------------------------------------------
*/
struct ALIGN16 Region2D AL_FINAL
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
    Region2D& operator=(const Region2D& other) = default;
};

/*
 -------------------------------------------------
 BoolInVec
 0レーンにデータが入っていることのみを保証したデータ
 -------------------------------------------------
 */
struct ALIGN16 BoolInVec AL_FINAL
{
public:
#if defined(AL_MATH_USE_NO_SIMD)
    bool v;
#else
    __m128i v;
#endif
public:
    INLINE BoolInVec() = default;
    INLINE BoolInVec(const BoolInVec& other) = default;
    INLINE BoolInVec(BoolInVec&& other) = default;
#if !defined(AL_MATH_USE_NO_SIMD)
    INLINE BoolInVec(__m128i v);
#endif
    INLINE BoolInVec(bool v);
#if !defined(AL_MATH_USE_NO_SIMD)
    INLINE operator __m128i() const;
#endif
    INLINE operator bool() const;
    INLINE bool value() const;
};

/*
 -------------------------------------------------
 FloatInVec
 0レーンにデータが入っていることのみを保証したデータ
 -------------------------------------------------
 */
struct ALIGN16 FloatInVec AL_FINAL
{
public:
#if defined(AL_MATH_USE_NO_SIMD)
    float v;
#else
    __m128 v;
#endif
public:
    // TODO: no simd版を作成する
    INLINE FloatInVec() = default;
    INLINE FloatInVec(const FloatInVec& other) = default;
    INLINE FloatInVec(FloatInVec&& other) = default;
    INLINE explicit FloatInVec(__m128 v);
    INLINE explicit FloatInVec(float v);
    INLINE operator __m128() const;
    INLINE operator float() const;
    INLINE FloatInVec operator-() const;
    INLINE bool isNan() const;
};
INLINE FloatInVec
operator-(FloatInVec lhs, FloatInVec rhs);
INLINE FloatInVec
operator*(FloatInVec lhs, FloatInVec rhs);
INLINE bool
operator<(FloatInVec lhs, FloatInVec rhs);
INLINE bool
operator>(FloatInVec lhs, FloatInVec rhs);
INLINE bool
operator <= (FloatInVec lhs, FloatInVec rhs);
INLINE bool
operator>=(FloatInVec lhs, FloatInVec rhs);

/*
-------------------------------------------------
Vec2
-------------------------------------------------
*/
struct ALIGN16 Vec2 AL_FINAL
{
public:
    Vec2() = default;
    Vec2(const Vec2& other) = default;
    Vec2(Vec2&& other) = default;
    Vec2(const float* es);
    Vec2(float ax, float ay);
    Vec2(__m128 other);
    explicit Vec2(float e);
    //
    INLINE float x() const;
    INLINE float y() const;
    INLINE FloatInVec vx() const;
    INLINE FloatInVec vy() const;
    //
    INLINE void setX(float x);
    INLINE void setY(float y);
    //
    void setZero();
    BoolInVec isZero() const;
    BoolInVec hasNaN() const;
    BoolInVec any() const;
    BoolInVec all() const;
    Vec2& normalize();
    Vec2& normalizeFast();
    Vec2& normalizeAccurate();
    Vec2 normalized() const;
    Vec2 normalizedFast() const;
    Vec2 normalizedAccurate() const;
    bool isNormalized() const;
    bool isNormalized(float eps) const;
    FloatInVec length() const;
    FloatInVec lengthSq() const;
    float operator[](int32_t index) const;
    INLINE Vec2& operator=(const Vec2& other) = default;
#if !defined(AL_MATH_USE_NO_SIMD)
    INLINE operator __m128() const { return xy_; }
#endif
    // static
    static Vec2 zero();
    static Vec2 one();
    static FloatInVec length(Vec2 v);
    static FloatInVec lengthSq(Vec2 v);
    static FloatInVec dot(Vec2 lhs, Vec2 rhs);
    static Vec2 min(Vec2 lhs, Vec2 rhs);
    static Vec2 max(Vec2 lhs, Vec2 rhs);

private:
#if defined(AL_MATH_USE_NO_SIMD)
    float x_;
    float y_;
#else
    __m128 xy_;
#endif
};

INLINE static Vec2
operator+(Vec2 lhs, Vec2 rhs);
INLINE static Vec2
operator-(Vec2 lhs, Vec2 rhs);
INLINE static Vec2
operator-(Vec2 v);
INLINE static Vec2&
operator+=(Vec2& lhs, Vec2 rhs);
INLINE static Vec2&
operator-=(Vec2& lhs, Vec2 rhs);
INLINE static BoolInVec
operator==(Vec2 lhs, Vec2 rhs);
INLINE static BoolInVec
operator!=(Vec2 lhs, Vec2 rhs);
INLINE static Vec2 operator*(float f, Vec2 v);
INLINE static Vec2 operator*(Vec2 v, float f);
INLINE static Vec2&
operator*=(Vec2& v, float factor);
INLINE static Vec2
operator/(Vec2 v, float f);

/*
-------------------------------------------------
Vec3
-------------------------------------------------
*/
struct ALIGN16 Vec3 /* AL_FINAL */
{
public:
    INLINE Vec3() = default;
    INLINE Vec3(const Vec3& other) = default;
    INLINE Vec3(Vec3&& other) = default;
    INLINE Vec3(const float* es);
    INLINE Vec3(float ax, float ay, float az);
    INLINE Vec3(float e);
    INLINE Vec3(__m128 other);
    INLINE Vec3(const std::array<float, 3>& arr);
    INLINE Vec3(Vec4 arr);
    INLINE void setZero();
    INLINE bool isZero() const; // TODO: boolinvecにする
    INLINE bool hasNan() const;
    INLINE bool any() const;
    INLINE bool all() const;
    INLINE Vec3& normalize();
    INLINE Vec3& normalizeFast();
    INLINE Vec3& normalizeAccurate();
    INLINE Vec3 normalized() const;
    INLINE Vec3 normalizedFast() const;
    INLINE Vec3 normalizedAccurate() const;
    INLINE bool isNormalized() const;
    INLINE bool isNormalized(float eps) const;
    INLINE void scale(float scale);
    INLINE FloatInVec length() const;
    INLINE FloatInVec lengthSq() const;
    INLINE Vec3& invert();
    INLINE Vec3 inverted() const;
    INLINE Vec3 invertedSafe(float defaultValue) const;
    INLINE Vec3 reflect(Vec3 v) const;
    //
    INLINE float operator[](int32_t index) const; // TODO: 削除する
    INLINE Vec3& operator=(const Vec3& other) = default;
#if !defined(AL_MATH_USE_NO_SIMD)
    INLINE operator __m128() const { return xyz_; }
#endif
    // アクセッサ
    INLINE float x() const;
    INLINE float y() const;
    INLINE float z() const;
    INLINE FloatInVec vx() const;
    INLINE FloatInVec vy() const;
    INLINE FloatInVec vz() const;
    // セッター
    INLINE void setX(float x);
    INLINE void setY(float y);
    INLINE void setZ(float z);
    INLINE void setVX(FloatInVec x);
    INLINE void setVY(FloatInVec y);
    INLINE void setVZ(FloatInVec z);
    // swizzle
#include "swizzle_vec3.inl"
    //
    static FloatInVec length(Vec3 v);
    static FloatInVec lengthSq(Vec3 v);
    static FloatInVec distance(Vec3 lhs, Vec3 rhs);
    static FloatInVec distanceSq(Vec3 lhs, Vec3 rhs);
    static FloatInVec dot(Vec3 lhs, Vec3 rhs);
    static Vec3 cross(Vec3 lhs, Vec3 rhs);
    static Vec3 mul(Vec3 lhs, Vec3 rhs);
    static Vec3 min(Vec3 lhs, Vec3 rhs);
    static Vec3 max(Vec3 lhs, Vec3 rhs);
    static Vec3 hmin(Vec3 v);
    static Vec3 hmax(Vec3 v);

private:
#if defined(AL_MATH_USE_NO_SIMD)
    float x_;
    float y_;
    float z_;
#else
    __m128 xyz_;
#endif
};

INLINE static Vec3
operator+(Vec3 lhs, Vec3 rhs);
INLINE static Vec3
operator-(Vec3 lhs, Vec3 rhs);
INLINE static Vec3
operator-(Vec3 v);
INLINE static Vec3&
operator+=(Vec3& lhs, Vec3 rhs);
INLINE static Vec3&
operator-=(Vec3& lhs, Vec3 rhs);
INLINE static BoolInVec
operator==(Vec3 lhs, Vec3 rhs);
INLINE static BoolInVec
operator!=(Vec3 lhs, Vec3 rhs);
INLINE static Vec3 operator*(float f, Vec3 v);
INLINE static Vec3 operator*(Vec3 v, float f);
INLINE static Vec3&
operator*=(Vec3& v, float factor);
INLINE static Vec3
operator/(Vec3 v, float f);

/*
-------------------------------------------------
Vec4
-------------------------------------------------
*/
struct ALIGN16 Vec4 AL_FINAL
{
public:
    INLINE Vec4() = default;
    INLINE Vec4(const Vec4& other) = default;
    INLINE Vec4(Vec4&& other) = default;
    INLINE Vec4(const float* es);
    INLINE Vec4(float x, float y, float z, float w);
    INLINE explicit Vec4(float e);
    INLINE Vec4(__m128 other);
    // アクセッサ
    INLINE float x() const;
    INLINE float y() const;
    INLINE float z() const;
    INLINE float w() const;
    INLINE FloatInVec vx() const;
    INLINE FloatInVec vy() const;
    INLINE FloatInVec vz() const;
    INLINE FloatInVec vw() const;
    //
    INLINE void zero();
    INLINE bool isZero() const;
    INLINE bool hasNan() const;
    INLINE bool any() const;
    INLINE bool all() const;
    // swizzle
#include "swizzle_vec4.inl"
    //
    INLINE FloatInVec length() const;
    INLINE FloatInVec lengthSq() const;
    INLINE Vec4& normalize();
    INLINE Vec4& normalizeFast();
    INLINE Vec4& normalizeAccurate();
    INLINE Vec4 normalized() const;
    INLINE Vec4 normalizedFast() const;
    INLINE Vec4 normalizedAccurate() const;
    //
    INLINE float operator[](int32_t index) const;
    INLINE Vec4& operator=(const Vec4& other) = default;
#if !defined(AL_MATH_USE_NO_SIMD)
    INLINE operator __m128() const { return xyzw_; }
#endif
    static FloatInVec dot(Vec4 lhs, Vec4 rhs);
    static FloatInVec length(Vec4 v);
    static FloatInVec lengthSq(Vec4 v);

private:
#if defined(AL_MATH_USE_NO_SIMD)
    float x_;
    float y_;
    float z_;
    float w_;
#else
    __m128 xyzw_;
#endif
};

/*
-------------------------------------------------
Matrix3x3
Row-Major/Row-Vectorの4x4行列
-------------------------------------------------
*/
struct ALIGN16 Matrix3x3 AL_FINAL
{
public:
    Matrix3x3() = default;
    Matrix3x3(const Matrix3x3& other) = default;
    Matrix3x3(Matrix3x3&& other) = default;
    Matrix3x3(const float* es);
    Matrix3x3(float e11,
              float e12,
              float e13,
              float e21,
              float e22,
              float e23,
              float e31,
              float e32,
              float e33);
    Matrix3x3(Vec3 row0, Vec3 row1, Vec3 row2);
    void set(Vec3 row0, Vec3 row1, Vec3 row2);
    void identity();
    void zero();
    bool isZero() const;
    bool hasNan() const;
    bool any() const;
    bool all() const;
    FloatInVec det() const;
    void inverse();
    void transpose();
    Vec3 transform(Vec3 v) const;
    Matrix3x3 inversed() const;
    Matrix3x3 transposed() const;
    template<int32_t index>
    Vec3 row() const;
    template<int32_t index>
    void setRow(Vec3 v);
    //
    Matrix3x3& operator=(const Matrix3x3& other) = default;

public:
#if defined(AL_MATH_USE_NO_SIMD)
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
#else
    Vec3 row0;
    Vec3 row1;
    Vec3 row2;
#endif
};

/*
-------------------------------------------------
Matrix4x4
Row-Major/Row-Vectorの4x4行列
-------------------------------------------------
*/
struct ALIGN16 Matrix4x4 AL_FINAL
{
public:
#if defined(AL_MATH_USE_NO_SIMD)
    union
    {
        ALIGN16 float e[16];
        ALIGN16 struct
        {
            float e11, e12, e13, e14;
            float e21, e22, e23, e24;
            float e31, e32, e33, e34;
            float e41, e42, e43, e44;
        };
    };
#else
    Vec4 row0;
    Vec4 row1;
    Vec4 row2;
    Vec4 row3;
#endif

public:
    Matrix4x4() = default;
    Matrix4x4(const Matrix4x4& other) = default;
    Matrix4x4(Matrix4x4&& other) = default;
    Matrix4x4(const Matrix3x3& m33);
    void constructAsTranslation(Vec3 v);
    void constructAsScale(Vec3 scale);
    void constructAsRotation(Vec3 xyz_, float angle);
    void constructAsViewMatrix(Vec3 origin, Vec3 target, Vec3 up);
    void zero();
    bool isZero() const;
    bool hasNan() const;
    bool any() const;
    bool all() const;
    void identity();
    Vec3 transform(Vec3 v) const;
    Vec4 transform(Vec4 v) const;
    float det() const;
    void inverse();
    void transpose();
    Matrix4x4 inversed() const;
    Matrix4x4 transposed() const;
    Matrix3x3 extract3x3() const;
    static Matrix4x4 mul(const Matrix4x4& lhs, const Matrix4x4& rhs);
    static Vec4 mul(const Vec4& v, const Matrix4x4& m);
    Matrix4x4& operator=(const Matrix4x4& other);

    // ViewMatrixからの取り出し
    Vec3 extractViewmatRight() const;
    Vec3 extractViewmatUp() const;
    Vec3 extractViewmatDir() const;
};
INLINE static Matrix4x4
operator+(const Matrix4x4& lhs, const Matrix4x4& rhs);

/*
-------------------------------------------------
クオータニオン
-------------------------------------------------
*/
class ALIGN16 Quat
{
public:
    Quat() = default;
    Quat(Vec3 xyz, float s);
    Quat(float x, float y, float z, float w);
    Vec3 qv() const;
    float qs() const;
    void identity();
    Quat conjugated() const;
    Vec3 rotate(Vec3 v) const;
    Matrix3x3 toMatrix() const;
    Quat normalized() const;
    //
    static Quat fromAxisRot(Vec3 axis, float theta);
    static Quat lerp(Quat a, Quat b, float t);
    static Quat slerp(Quat p, Quat q, float t);
    static float dot(Quat p, Quat q);
    //
    Quat operator*(float s) const;
    Quat operator+(Quat other) const;
    Quat operator*(Quat q) const;

public:
    Vec3 qv_;
    float qs_;
};

#include "math.inl"
