#include "math.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
static INLINE bool is_aligned(const void * ptr, std::uintptr_t alignment) noexcept
{
    auto iptr = reinterpret_cast<std::uintptr_t>(ptr);
    return !(iptr % alignment);
}

/*
-------------------------------------------------
_mm_rsqrt_ps()は単体では精度が非常に低いので
ニュートンラフソンを挟み精度を上げる
http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?f=9&t=1551
f(y) = y^{-2} - x = 0
となるxを求めるので
f'(y) = -2*y^{-3}
より次の漸化式を解くことになる
y_{n+1} = y_{n} * ( 1.5 - 0.5 * x * y_{n}^{3} )
-------------------------------------------------
*/
static INLINE __m128 _mm_rsqrt_ps_accurate(const __m128 v)
{
    static const __m128 half = _mm_set1_ps(0.5f);
    static const __m128 three = _mm_set1_ps(3.0f);
    const __m128 approx = _mm_rsqrt_ps(v);
    const __m128 muls = _mm_mul_ps(_mm_mul_ps(v, approx), approx);
    return _mm_mul_ps(_mm_mul_ps(half, approx), _mm_sub_ps(three, muls));
}

/*
 -------------------------------------------------
 _mm_rcp_ps()は精度が低いので、これもニュートンラフソン法を使って精度を上げる
 f(y) = y^{-1} - x = 0
 となるxを求めるので
 f'(y) = -y^{-2}
 より次の漸化式を解くことになる
 y_{n+1} = y_{n}*(2 - x * y_{n})
 -------------------------------------------------
 */
static INLINE __m128 _mm_rcp_ps_accurate(const __m128 v)
{
    static const __m128 two = _mm_set1_ps(2.0f);
    const __m128 approx = _mm_rcp_ps(v);
    const __m128 tmp0 = _mm_mul_ps(approx, v);
    const __m128 tmp1 = _mm_sub_ps(two, tmp0);
    const __m128 tmp2 = _mm_mul_ps(approx, tmp1);
    return tmp2;
}

/*
-------------------------------------------------
_mm_extract_ps()を使ってはいけない
cf. https://stackoverflow.com/a/17258448
-------------------------------------------------
*/
template<int32_t index>
INLINE float _mm_extract_ps_fast(__m128 v)
{
    return _mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, index)));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D::Size2D(_In_reads_(2) int32_t* es)
    :width(es[0]), height(es[1])
{}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D::Size2D(int32_t awidth, int32_t aheight)
    : width(awidth), height(aheight)
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t Size2D::index(int32_t x, int32_t y) const
{
    return x + y * width;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D operator + (const Size2D& lhs, const Size2D& rhs)
{
    return Size2D(lhs.width + rhs.width, lhs.height + rhs.height);
}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D operator - (const Size2D& lhs, const Size2D& rhs)
{
    return Size2D(lhs.width - rhs.width, lhs.height - rhs.height);
}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D operator - (const Size2D& v)
{
    return Size2D(-v.width, -v.height);
}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D& operator += (Size2D& lhs, const Size2D& rhs)
{
    lhs.width += rhs.width;
    lhs.height += rhs.height;
    return lhs;
}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D& operator -= (Size2D& lhs, const Size2D& rhs)
{
    lhs.width -= rhs.width;
    lhs.height -= rhs.height;
    return lhs;
}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool operator == (const Size2D& lhs, const Size2D& rhs)
{
    return
        lhs.width == rhs.width &&
        lhs.height == rhs.height;
}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D operator * (int32_t f, const Size2D& v)
{
    return Size2D(f * v.width, f * v.height);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D operator * (const Size2D& v, int32_t f)
{
    return Size2D(f * v.width, f * v.height);
}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Size2D& operator *= (Size2D& v, int32_t f)
{
    v.width *= f;
    v.height *= f;
    return v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Region2D::Region2D(int32_t aLeft, int32_t aTop, int32_t aRight, int32_t aBottom)
    :left(aLeft),
    top(aTop),
    right(aRight),
    bottom(aBottom)
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t Region2D::width() const
{
    return right - left;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t Region2D::height() const
{
    return bottom - top;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t Region2D::area() const
{
    return width() * height();
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec::FloatInVec(__m128 av)
{
#if defined(AL_MATH_USE_NO_SIMD)
    v = _mm_cvtss_f32(av);
#else
    v = av;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec::FloatInVec(float av)
{
#if defined(AL_MATH_USE_NO_SIMD)
    v = av;
#else
    v = _mm_set_ss(av);
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec::operator __m128 () const
{
#if defined(AL_MATH_USE_NO_SIMD)
    AL_ASSERT_ALWAYS(false);
    return _mm_set_ps1(0.0f);
#else
    return v;
#endif
    
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec::operator float() const
{
    return value();
}

/*
 -------------------------------------------------
 反転した値を返す
 -------------------------------------------------
 */
INLINE FloatInVec FloatInVec::operator -() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return FloatInVec(-v);
#else
    const __m128 neg = _mm_set_ss(-1.0f);
    const __m128 negv = _mm_mul_ps(neg,v);
    return negv;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE float FloatInVec::value() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return v;
#else
    return _mm_extract_ps_fast<0>(v);
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool FloatInVec::isNan() const
{
    return std::isnan(value());
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool operator < (FloatInVec lhs, FloatInVec rhs)
{
    return float(lhs) < float(rhs);
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Float8::Float8(__m256 other)
:v(other)
{}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Float8::operator __m256()const
{
    return v;
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Bool8::Bool8(__m256 other)
{
#if defined(AL_MATH_USE_NO_SIMD)
    AL_ASSERT_ALWAYS(false);
#else
    v = other;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool Bool8::at(int32_t index) const
{
#if defined(AL_MATH_USE_NO_SIMD)
    AL_ASSERT_ALWAYS(false);
    return false;
#else
    // TODO: SIMD化
    AL_ASSERT_ALWAYS(false);
    //return v[index];
    return 0.0f;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE BoolInVec::BoolInVec(__m128i av)
{
#if defined(AL_MATH_USE_NO_SIMD)
    v = (_mm_cvtsi128_si32(av) != 0);
#else
    v = av;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE BoolInVec::BoolInVec(bool av)
{
#if defined(AL_MATH_USE_NO_SIMD)
    v = av;
#else
    v = (_mm_set1_epi32(av ? 0xFFFFFFFF : 0x00000000));
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE BoolInVec::operator __m128i () const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return _mm_set1_epi32(v);
#else
    return v;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE BoolInVec::operator bool() const
{
    return value();
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool BoolInVec::value() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return v;
#else
    const __m128i zero = _mm_set1_epi32(0);
    const int32_t zeromask =
    _mm_movemask_epi8(_mm_cmpeq_epi32(v, zero)) & 0x000000FFFF;
    const int32_t ret = zeromask & 0x01;
    return !ret;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec2::Vec2(_In_reads_(4) const float* es)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = es[0];
    y_ = es[1];
#else
    // NOTE: アライメントは揃っている前提
    xy_ = _mm_load_ps(es);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec2::Vec2(float ax, float ay)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = ax;
    y_ = ay;
#else
    xy_ = (_mm_set_ps(0.0f, 0.0f, ay, ax));
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Vec2::Vec2(__m128 other)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = _mm_cvtss_f32(_mm_shuffle_ps(other, other, _MM_SHUFFLE(0, 0, 0, 0)));
    y_ = _mm_cvtss_f32(_mm_shuffle_ps(other, other, _MM_SHUFFLE(0, 0, 0, 1)));
#else
    xy_ = other;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec2::Vec2(float e)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = e;
    y_ = e;
#else
    xy_ = (_mm_set_ps(0.0f, 0.0f, e, e));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Vec2::x() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return x_;
#else
    return _mm_cvtss_f32(_mm_shuffle_ps(xy_, xy_, _MM_SHUFFLE(0, 0, 0, 0)));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Vec2::y() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return y_;
#else
    return _mm_cvtss_f32(_mm_shuffle_ps(xy_, xy_, _MM_SHUFFLE(1, 1, 1, 1)));
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE void Vec2::setX(float x)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = x;
#else
    // vvyy
    const __m128 tmp0 = _mm_shuffle_ps(_mm_set_ps1(x), xy_, _MM_SHUFFLE(1, 1, 0, 0));
    // vyz
    const __m128 tmp1 = _mm_shuffle_ps(tmp0, xy_, _MM_SHUFFLE(2, 2, 2, 0));
    xy_ = tmp1;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE void Vec2::setY(float y)
{
#if defined(AL_MATH_USE_NO_SIMD)
    y_ = y;
#else
    // xxvv
    const __m128 tmp0 = _mm_shuffle_ps(xy_, _mm_set_ps1(y), _MM_SHUFFLE(1, 1, 0, 0));
    // vyz
    const __m128 tmp1 = _mm_shuffle_ps(tmp0, xy_, _MM_SHUFFLE(2, 2, 2, 0));
    xy_ = tmp1;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE void Vec2::zero()
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = 0.0f;
    y_ = 0.0f;
#else
    xy_ = _mm_set1_ps(0.0f);
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE BoolInVec Vec2::isZero() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float d = 0.0001f;
    return
    fabsf(x_) < d &&
    fabsf(y_) < d;
#elif defined(AL_MATH_USE_AVX2)
    const __m128 zero = _mm_setzero_ps();
    const __m128 mask = _mm_cmpeq_ps(zero, xy_);
    return ((_mm_movemask_ps(mask) & 0x03) == 0x03);
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE BoolInVec Vec2::hasNaN() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
    std::isnan(x()) ||
    std::isnan(y());
#else
    const __m128 mask = _mm_cmpeq_ps(xy_, xy_);
    return (_mm_movemask_ps(mask) & 0x03) != 0x03;
#endif
}

/*
 -------------------------------------------------
 いずれかの要素が非0かを返す
 -------------------------------------------------
 */
INLINE BoolInVec Vec2::any() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return (x_ != 0.0f) || (y_ != 0.0f);
#else
    const __m128 zero = _mm_setzero_ps();
    const __m128 mask = _mm_cmpeq_ps(zero, xy_);
    return (_mm_movemask_ps(mask) & 0x03) != 0x03;
#endif
}

/*
 -------------------------------------------------
 全ての要素が非0かを返す
 -------------------------------------------------
 */
INLINE BoolInVec Vec2::all() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return (x_ != 0.0f) && (y_ != 0.0f);
#else
    const __m128 zero = _mm_setzero_ps();
    const __m128 mask = _mm_cmpeq_ps(zero, xy_);
    return (_mm_movemask_ps(mask) & 0x03) == 0x00;
#endif
    
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec2& Vec2::normalize()
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float invLen = 1.0f / length();
    x_ *= invLen;
    y_ *= invLen;
    return *this;
#else
    const __m128 dp = _mm_dp_ps(xy_, xy_, 0x33);
    const __m128 idp = _mm_rsqrt_ps_accurate(dp);
    xy_ = _mm_mul_ps(xy_, idp);
    return *this;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Vec2 Vec2::normalized() const
{
    Vec2 tmp = *this;
    tmp.normalize();
    return tmp;
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool Vec2::isNormalized() const
{
    // この数値は適当
    const float eps = 1.0e-3f;
    return isNormalized(eps);
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool Vec2::isNormalized(float eps) const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return fabsf(lengthSq() - 1.0f) < eps;
#elif defined(AL_MATH_USE_AVX2)
    // TODO: SIMDにしてboolinvecを返すようにする
    return (std::fabsf(lengthSq() - 1.0f) < eps);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec2::length() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return std::sqrtf(lengthSq());
#else
    return _mm_sqrt_ps(lengthSq(*this));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec2::lengthSq() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return x_ * x_ + y_ * y_;
#else
    return Vec2::dot(xy_, xy_);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Vec2::operator[](int32_t index) const
{
    AL_ASSERT_DEBUG(0 <= index && index <= 1);
    switch (index)
    {
        case 0: return x();
        case 1: return y();
        default: return 0.0f;
    }
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec Vec2::length(Vec2 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    AL_ASSERT_ALWAYS(false);
#elif defined(AL_MATH_USE_AVX2)
    return _mm_sqrt_ps(lengthSq(v));
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec Vec2::lengthSq(Vec2 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    AL_ASSERT_ALWAYS(false);
#else
    return Vec2::dot(v, v);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec2::dot(Vec2 lhs, Vec2 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
    lhs.x_ * rhs.x_ +
    lhs.y_ * rhs.y_;
#elif defined(AL_MATH_USE_AVX2)
    return _mm_dp_ps(lhs.xy_, rhs.xy_, 0x3F);
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Vec2 Vec2::min(Vec2 lhs, Vec2 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float x = std::min(lhs.x(), rhs.x());
    const float y = std::min(lhs.y(), rhs.y());
    return Vec2(x, y);
#else
    return _mm_min_ps(lhs.xy_, rhs.xy_);
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Vec2 Vec2::max(Vec2 lhs, Vec2 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float x = std::min(lhs.x(), rhs.x());
    const float y = std::min(lhs.y(), rhs.y());
    return Vec2(x, y);
#else
    return _mm_max_ps(lhs.xy_, rhs.xy_);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec2 operator + (Vec2 lhs, Vec2 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec2(
            lhs.x() + rhs.x(),
            lhs.y() + rhs.y());
#else
    return _mm_add_ps(lhs, rhs);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec2 operator - (Vec2 lhs, Vec2 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec2(
            lhs.x() - rhs.x(),
            lhs.y() - rhs.y());
#else
    AL_ASSERT_ALWAYS(false);
    return Vec2();
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec2 operator - (Vec2 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return  Vec2(-v.x(), -v.y());
#else
    return _mm_sub_ps(_mm_setzero_ps(), v);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec2& operator += (Vec2& lhs, Vec2 rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec2& operator -= (Vec2& lhs, Vec2 rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static BoolInVec operator == (Vec2 lhs, Vec2 rhs)
{
    return !(lhs != rhs);
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE static BoolInVec operator != (Vec2 lhs, Vec2 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
    (lhs.x() != rhs.x()) ||
    (lhs.y() != rhs.y());
#else
    const __m128 mask = _mm_cmpeq_ps(lhs, rhs);
    const int32_t maskPacked = _mm_movemask_ps(mask);
    return (maskPacked & 0x03) != 0x03;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec2 operator * (float f, Vec2 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec2(
            f * v.x(),
            f * v.y());
#else
    const __m128 s = _mm_set1_ps(f);
    return _mm_mul_ps(s, v);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec2 operator * (Vec2 v, float f)
{
    return f*v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec2& operator *= (Vec2& v, float f)
{
    v = v * f;
    return v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec2 operator / (Vec2 v, float f)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float inv = 1.0f / f;
    return Vec2(v.x() * inv, v.y() * inv);
#elif defined(AL_MATH_USE_AVX2)
    const __m128 s = _mm_rcp_ps_accurate(_mm_set1_ps(f));
    return _mm_mul_ps(v, s);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3::Vec3(_In_reads_(4) const float* es)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = es[0];
    y_ = es[1];
    z_ = es[2];
#elif defined(AL_MATH_USE_AVX2)
    // NOTE: 16byteアライメントでないといけない
    xyz_ = _mm_load_ps(es);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3::Vec3(float ax, float ay, float az)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = ax;
    y_ = ay;
    z_ = az;
#elif defined(AL_MATH_USE_AVX2)
    xyz_ = _mm_set_ps(0.0f, az, ay, ax);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3::Vec3(float e)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = e;
    y_ = e;
    z_ = e;
#elif defined(AL_MATH_USE_AVX2)
    xyz_ = _mm_set_ps1(e);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3::Vec3(__m128 other)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = _mm_cvtss_f32(_mm_shuffle_ps(other, other, _MM_SHUFFLE(0, 0, 0, 0)));
    y_ = _mm_cvtss_f32(_mm_shuffle_ps(other, other, _MM_SHUFFLE(0, 0, 0, 1)));
    z_ = _mm_cvtss_f32(_mm_shuffle_ps(other, other, _MM_SHUFFLE(0, 0, 0, 2)));
#elif defined(AL_MATH_USE_AVX2)
    xyz_ = other;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3::Vec3(const std::array<float, 3>& arr)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = arr[0];
    y_ = arr[1];
    z_ = arr[2];
#elif defined(AL_MATH_USE_AVX2)
    xyz_ = _mm_set_ps(0.0f, arr[2], arr[1], arr[0]);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3::Vec3(Vec4 arr)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = arr[0];
    y_ = arr[1];
    z_ = arr[2];
#elif defined(AL_MATH_USE_AVX2)
    xyz_ = arr;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Vec3::zero()
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = 0.0f;
    y_ = 0.0f;
    z_ = 0.0f;
#elif defined(AL_MATH_USE_AVX2)
    xyz_ = _mm_set_ps1(0.0f);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool Vec3::isZero() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float d = 0.0001f;
    return
        fabsf(x_) < d &&
        fabsf(y_) < d &&
        fabsf(z_) < d;
#elif defined(AL_MATH_USE_AVX2)
    const __m128 zero = _mm_setzero_ps();
    const __m128 mask = _mm_cmpeq_ps(zero, xyz_);
    return (_mm_movemask_ps(mask) & 0x07) == 0x07;
#endif
}
/*
-------------------------------------------------
TODO: 戻り値はBoolInVecの方がいいかもしれない
-------------------------------------------------
*/
INLINE bool Vec3::hasNan() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        isnan(x_) ||
        isnan(y_) ||
        isnan(z_);
#elif defined(AL_MATH_USE_AVX2)
    const __m128 mask = _mm_cmpeq_ps(xyz_, xyz_);
    return (_mm_movemask_ps(mask) & 0x07) != 0x07;
#endif
}

/*
-------------------------------------------------
いずれかの要素が非0かを返す
-------------------------------------------------
*/
INLINE bool Vec3::any() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return (x_ != 0.0f) || (y_ != 0.0f) || (z_ != 0.0f);
#else
    const __m128 zero = _mm_setzero_ps();
    const __m128 mask = _mm_cmpeq_ps(zero, xyz_);
    return (_mm_movemask_ps(mask) & 0x07) != 0x07;
#endif
}

/*
 -------------------------------------------------
 全ての要素が非0かを返す
 -------------------------------------------------
 */
INLINE bool Vec3::all() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return (x_ != 0.0f) && (y_ != 0.0f) && (z_ != 0.0f);
#else
    const __m128 zero = _mm_setzero_ps();
    const __m128 mask = _mm_cmpeq_ps(zero, xyz_);
    return (_mm_movemask_ps(mask) & 0x07) == 0x00;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3& Vec3::normalize()
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float invLen = 1.0f / length();
    x_ *= invLen;
    y_ *= invLen;
    z_ *= invLen;
    return *this;
#elif defined(AL_MATH_USE_AVX2)
    // NOTE: 長さ0のケアはしていない
    const __m128 dp = _mm_dp_ps(xyz_, xyz_, 0x7F); // TODO: これは0x7Fではないのか？
    const __m128 idp = _mm_rsqrt_ps_accurate(dp);
    xyz_ = _mm_mul_ps(xyz_, idp);
    return *this;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Vec3::normalized() const
{
    Vec3 v = *this;
    v.normalize();
    return v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool Vec3::isNormalized() const
{
    // この数値は適当
    const float eps = 1.0e-3f;
    return isNormalized(eps);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool Vec3::isNormalized(float eps) const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return fabsf(lengthSq() - 1.0f) < eps;
#elif defined(AL_MATH_USE_AVX2)
    // TODO: SIMDにしてboolinvecを返すようにする
    return (std::fabsf(lengthSq() - 1.0f) < eps);
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE void Vec3::scale(float scale)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ *= scale;
    y_ *= scale;
    z_ *= scale;
#elif defined(AL_MATH_USE_AVX2)
    const __m128 s = _mm_set1_ps(scale);
    xyz_ = _mm_mul_ps(xyz_, s);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec3::length() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return sqrtf(lengthSq());
#elif defined(AL_MATH_USE_AVX2)
    return length(xyz_);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec3::lengthSq() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return x_*x_ + y_*y_ + z_*z_;
#elif defined(AL_MATH_USE_AVX2)
    return lengthSq(xyz_);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3& Vec3::invert()
{
    Vec3 tmp = inverted();
    *this = tmp;
    return *this;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Vec3::inverted() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(1.0f / x_, 1.0f / y_, 1.0f / z_);
    assert(false);
#elif defined(AL_MATH_USE_AVX2)
    return _mm_rsqrt_ps_accurate(xyz_);
#endif
}

/*
-------------------------------------------------
0になっているレーンだけdefaultValueを設定する
-------------------------------------------------
*/
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
    return Vec3(inv(x_), inv(y_), inv(z_));
#elif defined(AL_MATH_USE_AVX2)
    //const __m128 dv = _mm_set_ps1(defaultValue);
    const __m128 zero = _mm_setzero_ps();
    const __m128 mask = _mm_cmpeq_ps(zero, xyz_);
    const __m128 rcp = _mm_rcp_ps_accurate(xyz_);
    return _mm_andnot_ps(mask, rcp);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Vec3::reflect(Vec3 v) const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float factor = -2.0f * dot(*this, v);
    return v + factor * (*this);
#elif defined(AL_MATH_USE_AVX2)
    // TODO: SIMDnize
    const float factor = dot(*this, v) * -2.0f;
    return v + factor * (*this);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Vec3::x() const
{
    return float(vx());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Vec3::y() const
{
    return float(vy());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Vec3::z() const
{
    return float(vz());
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec Vec3::vx() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return FloatInVec(x_);
#else
    return FloatInVec(_mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 0)));
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec Vec3::vy() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return FloatInVec(y_);
#else
    return FloatInVec(_mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 1)));
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec Vec3::vz() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return FloatInVec(z_);
#else
    return FloatInVec(_mm_shuffle_ps(xyz_, xyz_, _MM_SHUFFLE(0, 0, 0, 2)));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Vec3::setX(float x)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = x;
#else
    // vvyy
    const __m128 tmp0 = _mm_shuffle_ps(_mm_set_ps1(x), xyz_, _MM_SHUFFLE(1, 1, 0, 0));
    // vyz
    const __m128 tmp1 = _mm_shuffle_ps(tmp0, xyz_, _MM_SHUFFLE(2, 2, 2, 0));
    xyz_ = tmp1;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Vec3::setY(float y)
{
#if defined(AL_MATH_USE_NO_SIMD)
    y_ = y;
#else
    // xxvv
    const __m128 tmp0 = _mm_shuffle_ps(xyz_, _mm_set_ps1(y), _MM_SHUFFLE(1, 1, 0, 0));
    // vyz
    const __m128 tmp1 = _mm_shuffle_ps(tmp0, xyz_, _MM_SHUFFLE(2, 2, 2, 0));
    xyz_ = tmp1;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Vec3::setZ(float z)
{
#if defined(AL_MATH_USE_NO_SIMD)
    z_ = z;
#else
    // xyv
    const __m128 tmp1 = _mm_shuffle_ps(xyz_, _mm_set_ps1(z), _MM_SHUFFLE(3, 3, 1, 0));
    xyz_ = tmp1;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE void Vec3::setVX(FloatInVec x)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = float(x);
#else
    // vvyy
    const __m128 tmp0 = _mm_shuffle_ps(x, xyz_, _MM_SHUFFLE(1, 1, 0, 0));
    // vyz
    const __m128 tmp1 = _mm_shuffle_ps(tmp0, xyz_, _MM_SHUFFLE(2, 2, 2, 0));
    xyz_ = tmp1;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE void Vec3::setVY(FloatInVec y)
{
#if defined(AL_MATH_USE_NO_SIMD)
    y_ = float(y);
#else
    // xxvv
    const __m128 tmp0 = _mm_shuffle_ps(xyz_, y, _MM_SHUFFLE(1, 1, 0, 0));
    // vyz
    const __m128 tmp1 = _mm_shuffle_ps(tmp0, xyz_, _MM_SHUFFLE(2, 2, 2, 0));
    xyz_ = tmp1;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE void Vec3::setVZ(FloatInVec z)
{
#if defined(AL_MATH_USE_NO_SIMD)
    z_ = float(z);
#else
    // xyv
    const __m128 tmp1 = _mm_shuffle_ps(xyz_, z, _MM_SHUFFLE(3, 3, 1, 0));
    xyz_ = tmp1;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Vec3::operator[](int32_t index) const
{
    AL_ASSERT_DEBUG(0 <= index && index <= 2);
    switch (index)
    {
    case 0: return x();
    case 1: return y();
    case 2: return z();
    default: return 0.0f;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec3::length(Vec3 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return std::sqrtf(Vec3::lengthSq(v));
#elif defined(AL_MATH_USE_AVX2)
    return _mm_sqrt_ps(lengthSq(v));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec3::lengthSq(Vec3 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    assert(false);
    return v.x_ * v.x_ + v.y_ * v.y_  + v.z_ *  v.z_;
#else
    return Vec3::dot(v, v);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Vec3::min(Vec3 lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float x = std::min(lhs.x(), rhs.x());
    const float y = std::min(lhs.y(), rhs.y());
    const float z = std::min(lhs.z(), rhs.z());
    return Vec3(x, y, z);
#else
    return _mm_min_ps(lhs.xyz_, rhs.xyz_);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Vec3::max(Vec3 lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float x = std::max(lhs.x(), rhs.x());
    const float y = std::max(lhs.y(), rhs.y());
    const float z = std::max(lhs.z(), rhs.z());
    return Vec3(x, y, z);
#else
    return _mm_max_ps(lhs.xyz_, rhs.xyz_);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Vec3::hmin(Vec3 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return std::min({v.x_,v.y_,v.z_});
#else
    const __m128 xyz = v.xyz_;
    const __m128 yzx = _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 2, 1));
    const __m128 zxy = _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 0, 2));
    const __m128 hmn = _mm_min_ps(_mm_min_ps(xyz, yzx), zxy);
    return hmn;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Vec3::hmax(Vec3 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return std::max({v.x_,v.y_,v.z_});
#else
    const __m128 xyz = v.xyz_;
    const __m128 yzx = _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 0, 2, 1));
    const __m128 zxy = _mm_shuffle_ps(xyz, xyz, _MM_SHUFFLE(0, 1, 0, 2));
    const __m128 hmx = _mm_max_ps(_mm_max_ps(xyz, yzx), zxy);
    return hmx;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec3::distance(Vec3 lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const Vec3 tmp = lhs - rhs;
    return tmp.length();
#elif defined(AL_MATH_USE_AVX2)
    const __m128 dif = _mm_sub_ps(lhs.xyz_, rhs.xyz_);
    return length(dif);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec3::distanceSq(Vec3 lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const Vec3 tmp = lhs - rhs;
    return tmp.lengthSq();
#elif defined(AL_MATH_USE_AVX2)
    const __m128 dif = _mm_sub_ps(lhs.xyz_, rhs.xyz_);
    return lengthSq(dif);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec3::dot(Vec3 lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        lhs.x_ * rhs.x_ +
        lhs.y_ * rhs.y_ +
        lhs.z_ * rhs.z_;
#elif defined(AL_MATH_USE_AVX2)
    return _mm_dp_ps(lhs.xyz_, rhs.xyz_, 0x7F);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Vec3::cross(Vec3 xyz_, Vec3 abc)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec3(
            xyz_.y_ * abc.z_ - xyz_.z_ * abc.y_,
            xyz_.z_ * abc.x_ - xyz_.x_ * abc.z_,
            xyz_.x_ * abc.y_ - xyz_.y_ * abc.x_);
#elif defined(AL_MATH_USE_AVX2)
    const __m128 tmp0 = _mm_shuffle_ps(xyz_.xyz_, xyz_.xyz_, _MM_SHUFFLE(3, 0, 2, 1));
    const __m128 tmp1 = _mm_shuffle_ps(abc.xyz_, abc.xyz_, _MM_SHUFFLE(3, 1, 0, 2));
    const __m128 tmp2 = _mm_shuffle_ps(xyz_.xyz_, xyz_.xyz_, _MM_SHUFFLE(3, 1, 0, 2));
    const __m128 tmp3 = _mm_shuffle_ps(abc.xyz_, abc.xyz_, _MM_SHUFFLE(3, 0, 2, 1));
    return _mm_sub_ps(_mm_mul_ps(tmp0, tmp1), _mm_mul_ps(tmp2, tmp3));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Vec3::mul(Vec3 lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec3(
            lhs.x_ * rhs.x_,
            lhs.y_ * rhs.y_,
            lhs.z_ * rhs.z_);
#elif defined(AL_MATH_USE_AVX2)
    return _mm_mul_ps(lhs.xyz_, rhs.xyz_);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3 operator + (Vec3 lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec3(
            lhs.x() + rhs.x(),
            lhs.y() + rhs.y(),
            lhs.z() + rhs.z());
#elif defined(AL_MATH_USE_AVX2)
    return _mm_add_ps(lhs, rhs);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3 operator - (Vec3 lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec3(
            lhs.x() - rhs.x(),
            lhs.y() - rhs.y(),
            lhs.z() - rhs.z());
#elif defined(AL_MATH_USE_AVX2)
    return _mm_sub_ps(lhs, rhs);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3 operator - (Vec3 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return  Vec3(-v.x(), -v.y(), -v.z());
#elif defined(AL_MATH_USE_AVX2)
    return _mm_sub_ps(_mm_setzero_ps(), v);
#endif
}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3& operator += (Vec3& lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    lhs = lhs + rhs;
    return lhs;
#elif defined(AL_MATH_USE_AVX2)
    lhs = _mm_add_ps(lhs, rhs);
    return lhs;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3& operator -= (Vec3& lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    lhs = lhs - rhs;
    return lhs;
#elif defined(AL_MATH_USE_AVX2)
    lhs = _mm_sub_ps(lhs, rhs);
    return lhs;
#endif
}
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static BoolInVec operator == (Vec3 lhs, Vec3 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        (lhs.x() == rhs.x()) &&
        (lhs.y() == rhs.y()) &&
        (lhs.z() == rhs.z());
#elif defined(AL_MATH_USE_AVX2)
    const __m128 mask = _mm_cmpeq_ps(lhs, rhs);
    const int32_t maskPacked = _mm_movemask_ps(mask);
    return (maskPacked & 0x07) == 0x07;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static BoolInVec operator != (Vec3 lhs, Vec3 rhs)
{
    return !(lhs == rhs);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3 operator * (float f, Vec3 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec3(
            f * v.x(),
            f * v.y(),
            f * v.z());
#elif defined(AL_MATH_USE_AVX2)
    const __m128 s = _mm_set1_ps(f);
    return _mm_mul_ps(s, v);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3 operator * (Vec3 v, float f)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return f*v;
#elif defined(AL_MATH_USE_AVX2)
    const __m128 s = _mm_set1_ps(f);
    return _mm_mul_ps(v, s);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3& operator *= (Vec3& v, float f)
{
#if defined(AL_MATH_USE_NO_SIMD)
    v = v * f;
    return v;
#elif defined(AL_MATH_USE_AVX2)
    const __m128 s = _mm_set1_ps(f);
    v = _mm_mul_ps(s, v);
    return v;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3 operator / (Vec3 v, float f)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float inv = 1.0f / f;
    return Vec3(v.x() * inv, v.y() * inv, v.z() * inv);
#elif defined(AL_MATH_USE_AVX2)
    const __m128 s = _mm_rcp_ps_accurate(_mm_set1_ps(f));
    return _mm_mul_ps(v, s);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec4::Vec4(_In_reads_(4) const float* es)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = es[0];
    y_ = es[1];
    z_ = es[2];
    w_ = es[3];
#else
    // NOTE: 16byteアライメントは揃っている前提
    AL_ASSERT_DEBUG( is_aligned(es, 16) );
    xyzw_ = _mm_load_ps(es);
#endif
}


/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec4::Vec4(float ax, float ay, float az, float aw)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = ax;
    y_ = ay;
    z_ = az;
    w_ = aw;
#elif defined(AL_MATH_USE_AVX2)
    xyzw_ = _mm_set_ps(aw, az, ay, ax);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec4::Vec4(float e)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = e;
    y_ = e;
    z_ = e;
    w_ = e;
#elif defined(AL_MATH_USE_AVX2)
    xyzw_ = _mm_set_ps1(e);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec4::Vec4(__m128 other)
{
#if defined(AL_MATH_USE_NO_SIMD)
    x_ = _mm_cvtss_f32(_mm_shuffle_ps(other, other, _MM_SHUFFLE(0, 0, 0, 0)));
    y_ = _mm_cvtss_f32(_mm_shuffle_ps(other, other, _MM_SHUFFLE(0, 0, 0, 1)));
    z_ = _mm_cvtss_f32(_mm_shuffle_ps(other, other, _MM_SHUFFLE(0, 0, 0, 2)));
    w_ = _mm_cvtss_f32(_mm_shuffle_ps(other, other, _MM_SHUFFLE(0, 0, 0, 3)));
#elif defined(AL_MATH_USE_AVX2)
    xyzw_ = other;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE float Vec4::x()const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return x_;
#else
    return _mm_cvtss_f32(_mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(0, 0, 0, 0)));
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE float Vec4::y()const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return y_;
#else
    return _mm_cvtss_f32(_mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(1, 1, 1, 1)));
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE float Vec4::z()const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return z_;
#else
    return _mm_cvtss_f32(_mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(2, 2, 2, 2)));
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE float Vec4::w()const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return w_;
#else
    return _mm_cvtss_f32(_mm_shuffle_ps(xyzw_, xyzw_, _MM_SHUFFLE(3, 3, 3, 3)));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec4::length() const
{
    return Vec4::length(*this);
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE FloatInVec Vec4::lengthSq() const
{
    return Vec4::lengthSq(*this);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec4& Vec4::normalize()
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float invLen = 1.0f / length();
    x_ *= invLen;
    y_ *= invLen;
    z_ *= invLen;
    w_ *= invLen;
#elif defined(AL_MATH_USE_AVX2)
    const __m128 dp = _mm_dp_ps(xyzw_, xyzw_, 0xFF);
    const __m128 idp = _mm_rsqrt_ps_accurate(dp);
    xyzw_ = _mm_mul_ps(xyzw_, idp);
#endif
    return *this;
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
Vec4 Vec4::normalized() const
{
    Vec4 ret = *this;
    ret.normalize();
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Vec4::operator[](int32_t index) const
{
    AL_ASSERT_DEBUG(0 <= index && index <= 3);
    switch (index)
    {
    case 0: return x();
    case 1: return y();
    case 2: return z();
    case 3: return w();
    default: return 0.0f;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec4::dot(Vec4 lhs, Vec4 rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        lhs.x_ * rhs.x_ +
        lhs.y_ * rhs.y_ +
        lhs.z_ * rhs.z_ +
        lhs.w_ * rhs.w_;
#else
    return _mm_dp_ps(lhs.xyzw_, rhs.xyzw_, 0xFF);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec4::length(Vec4 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return std::sqrtf(v.lengthSq());
#elif defined(AL_MATH_USE_AVX2)
    return _mm_sqrt_ps(lengthSq(v));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE FloatInVec Vec4::lengthSq(Vec4 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return (v.x_ * v.x_)+(v.y_ * v.y_)+(v.z_ * v.z_)+(v.w_ * v.w_);
#elif defined(AL_MATH_USE_AVX2)
    return dot(v, v);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix3x3::Matrix3x3(_In_reads_(9) const float* es)
{
#if defined(AL_MATH_USE_NO_SIMD)
    e[0] = es[0];
    e[1] = es[1];
    e[2] = es[2];
    e[3] = es[3];
    e[4] = es[4];
    e[5] = es[5];
    e[6] = es[6];
    e[7] = es[7];
    e[8] = es[8];
#else
    AL_ASSERT_ALWAYS(false);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix3x3::Matrix3x3(
    float ae11, float ae12, float ae13,
    float ae21, float ae22, float ae23,
    float ae31, float ae32, float ae33)
{
#if defined(AL_MATH_USE_NO_SIMD)
    e11 = ae11;
    e12 = ae12;
    e13 = ae13;
    e21 = ae21;
    e22 = ae22;
    e23 = ae23;
    e31 = ae31;
    e32 = ae32;
    e33 = ae33;
#else
    row0 = _mm_set_ps(0.0f, ae13, ae12, ae11);
    row1 = _mm_set_ps(0.0f, ae23, ae22, ae21);
    row2 = _mm_set_ps(0.0f, ae33, ae32, ae31);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix3x3::Matrix3x3(
    Vec3 row0,
    Vec3 row1,
    Vec3 row2)
{
    set(row0, row1, row2);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix3x3::set(
    Vec3 aRow0,
    Vec3 aRow1,
    Vec3 aRow2)
{
    setRow<0>(aRow0);
    setRow<1>(aRow1);
    setRow<2>(aRow2);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix3x3::identity()
{
#if defined(AL_MATH_USE_NO_SIMD)
    e11 = 1.0f; e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
    e21 = 0.0f; e22 = 1.0f; e23 = 0.0f; e24 = 0.0f;
    e31 = 0.0f; e32 = 0.0f; e33 = 1.0f; e34 = 0.0f;
    e41 = 0.0f; e42 = 0.0f; e43 = 0.0f; e44 = 1.0f;
#else
    // NOTE: set_ps()直接は遅いかもしれない
    row0 = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
    row1 = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
    row2 = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Matrix3x3::det() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float d =
        (e11 * e22 * e33 + e21 * e32 * e13 + e31 * e12 * e23) -
        (e11 * e32 * e23 + e31 * e22 * e13 + e21 * e12 * e33);
    return d;
#else
    const __m128 C_1_1_1_0 = _mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f);
    const __m128 e11_12_21_22 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(1, 0, 1, 0));
    const __m128 e22_21x_31x_32 = _mm_shuffle_ps(row1, row2, _MM_SHUFFLE(1, 0, 0, 1));
    const __m128 e11_13_31_33 = _mm_shuffle_ps(row0, row2, _MM_SHUFFLE(2, 0, 2, 0));
    const __m128 e11x_13_21x_23 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(2, 0, 2, 0));
    //
    const __m128 e11_21_31 = _mm_shuffle_ps(e11_12_21_22, row2, _MM_SHUFFLE(0, 0, 2, 0));
    const __m128 e22_32_12 = _mm_shuffle_ps(e22_21x_31x_32, row0, _MM_SHUFFLE(0, 1, 3, 0));
    const __m128 e33_13_23 = _mm_shuffle_ps(e11_13_31_33, row1, _MM_SHUFFLE(0, 2, 1, 3));
    const __m128 e11_31_21 = _mm_shuffle_ps(e11_13_31_33, row1, _MM_SHUFFLE(0, 0, 2, 0));
    const __m128 e32_22_12 = _mm_shuffle_ps(e22_21x_31x_32, row0, _MM_SHUFFLE(0, 1, 0, 3));
    const __m128 e23_13_33 = _mm_shuffle_ps(e11x_13_21x_23, row2, _MM_SHUFFLE(0, 2, 1, 3));
    const __m128 tmp0 =
        _mm_sub_ps(
            _mm_mul_ps(_mm_mul_ps(e11_21_31, e22_32_12), e33_13_23),
            _mm_mul_ps(_mm_mul_ps(e11_31_21, e32_22_12), e23_13_33));
    const __m128 detV = _mm_dp_ps(tmp0, C_1_1_1_0, 0x7F);
    //
    return _mm_extract_ps_fast<0>(detV);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix3x3::inverse()
{
    *this = inversed();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix3x3::transpose()
{
#if defined(AL_MATH_USE_NO_SIMD)
    std::swap(e12, e21);
    std::swap(e13, e31);
    std::swap(e23, e32);
#else
    const __m128 tmp0 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(1, 0, 1, 0));
    const __m128 tmp1 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(1, 0, 1, 0));
    const __m128 tmp2 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(3, 2, 3, 2));
    const __m128 tmp3 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 2, 3, 2));
    row0 = _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0));
    row1 = _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(3, 1, 3, 1));
    row2 = _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(2, 0, 2, 0));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Matrix3x3::transform(Vec3 v) const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float x = Vec3::dot(row<0>(),v);
    const float y = Vec3::dot(row<1>(),v);
    const float z = Vec3::dot(row<2>(),v);
    return Vec3(x,y,z);
#else
    // TODO: Matrix4x4::transform()のようにすれば遅いdpを使わなくてすむのでそうする
    const __m128 v0 = _mm_dp_ps(row0, v, 0x7F);
    const __m128 v1 = _mm_dp_ps(row1, v, 0x7F);
    const __m128 v2 = _mm_dp_ps(row2, v, 0x7F);
    //
    const __m128 tmp = _mm_shuffle_ps(v0, v1, 0x44);
    const __m128 ret = _mm_shuffle_ps(tmp, v2, 0x88);
    return ret;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix3x3 Matrix3x3::inversed() const
{
#if defined(AL_MATH_USE_NO_SIMD)
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
#else
    //
    const __m128 e12_13_22_23 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(2, 1, 2, 1));
    const __m128 e11_13_21_23 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(2, 0, 2, 0));
    const __m128 e11_12_21_22 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(1, 0, 1, 0));
    //
    const __m128 e22_13_12 = _mm_shuffle_ps(e12_13_22_23, row0, _MM_SHUFFLE(0, 1, 1, 2));
    const __m128 e33_32_23 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(0, 2, 1, 2));
    const __m128 e23_12_13 = _mm_shuffle_ps(e12_13_22_23, row0, _MM_SHUFFLE(0, 2, 0, 3));
    const __m128 e32_33_22 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(0, 1, 2, 1));
    //
    const __m128 e23_11_13 = _mm_shuffle_ps(e11_13_21_23, row0, _MM_SHUFFLE(0, 2, 0, 3));
    const __m128 e31_33_21 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(0, 0, 2, 0));
    const __m128 e21_13_11 = _mm_shuffle_ps(e11_13_21_23, row0, _MM_SHUFFLE(0, 0, 1, 2));
    const __m128 e33_31_23 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(0, 2, 0, 2));
    //
    const __m128 e21_12_11 = _mm_shuffle_ps(e11_12_21_22, row0, _MM_SHUFFLE(0, 0, 1, 2));
    const __m128 e32_31_22 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(0, 1, 0, 1));
    const __m128 e22_11_12 = _mm_shuffle_ps(e11_12_21_22, row0, _MM_SHUFFLE(0, 1, 0, 3));
    const __m128 e31_32_21 = _mm_shuffle_ps(row2, row1, _MM_SHUFFLE(0, 0, 1, 0));
    //
    const __m128 e11_13_31_33 = _mm_shuffle_ps(row0, row2, _MM_SHUFFLE(2, 0, 2, 0));
    const __m128 e22_21x_31x_32 = _mm_shuffle_ps(row1, row2, _MM_SHUFFLE(1, 0, 0, 1));
    const __m128 e11x_13_21x_23 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(2, 0, 2, 0));
    //
    const __m128 e11_21_31 = _mm_shuffle_ps(e11_12_21_22, row2, _MM_SHUFFLE(0, 0, 2, 0));
    const __m128 e22_32_12 = _mm_shuffle_ps(e22_21x_31x_32, row0, _MM_SHUFFLE(0, 1, 3, 0));
    const __m128 e33_13_23 = _mm_shuffle_ps(e11_13_31_33, row1, _MM_SHUFFLE(0, 2, 1, 3));
    const __m128 e11_31_21 = _mm_shuffle_ps(e11_13_31_33, row1, _MM_SHUFFLE(0, 0, 2, 0));
    const __m128 e32_22_12 = _mm_shuffle_ps(e22_21x_31x_32, row0, _MM_SHUFFLE(0, 1, 0, 3));
    const __m128 e23_13_33 = _mm_shuffle_ps(e11x_13_21x_23, row2, _MM_SHUFFLE(0, 2, 1, 3));
    // det()
    const __m128 tmp =
        _mm_sub_ps(
            _mm_mul_ps(_mm_mul_ps(e11_21_31, e22_32_12), e33_13_23),
            _mm_mul_ps(_mm_mul_ps(e11_31_21, e32_22_12), e23_13_33));
    const __m128 detV = _mm_dp_ps(tmp, _mm_set_ps1(1.0f), 0x7F);
    const __m128 invDet = _mm_rcp_ps_accurate(detV);
    //
    Matrix3x3 m;
    m.row0 =
        _mm_mul_ps(
            _mm_sub_ps(
                _mm_mul_ps(e22_13_12, e33_32_23),
                _mm_mul_ps(e23_12_13, e32_33_22)),
            invDet);
    m.row1 =
        _mm_mul_ps(
            _mm_sub_ps(
                _mm_mul_ps(e23_11_13, e31_33_21),
                _mm_mul_ps(e21_13_11, e33_31_23)),
            invDet);
    m.row2 =
        _mm_mul_ps(
            _mm_sub_ps(
                _mm_mul_ps(e21_12_11, e32_31_22),
                _mm_mul_ps(e22_11_12, e31_32_21)),
            invDet);
    //
    return m;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix3x3 Matrix3x3::transposed() const
{
    Matrix3x3 m = *this;
    m.transpose();
    return m;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<int32_t index>
INLINE Vec3 Matrix3x3::row() const
{
    static_assert((0 <= index) && (index <= 2), "");

#if defined(AL_MATH_USE_NO_SIMD)
    switch (index)
    {
    case 0: return Vec3(e11, e12, e13);
    case 1: return Vec3(e21, e22, e23);
    case 2: return Vec3(e31, e32, e33);
    }
    return Vec3(0.0f);
#else
    switch (index)
    {
    case 0: return row0;
    case 1: return row1;
    case 2: return row2;
    }
    return Vec3(0.0f);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<int32_t index>
INLINE void Matrix3x3::setRow(Vec3 v)
{
    static_assert((0<=index) && (index <= 2), "");
#if defined(AL_MATH_USE_NO_SIMD)
    switch (index)
    {
        case 0:
            e11 = v.x();
            e12 = v.y();
            e13 = v.z();
            break;
        case 1:
            e21 = v.x();
            e22 = v.y();
            e23 = v.z();
            break;
        case 2:
            e31 = v.x();
            e32 = v.y();
            e33 = v.z();
            break;
    }
#else
    static_assert((0 <= index) && (index <= 2),"");
    switch (index)
    {
    case 0:
        row0 = v;
        break;
    case 1:
        row1 = v;
        break;
    case 2:
        row2 = v;
        break;
    }
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix4x4::Matrix4x4(_In_reads_(16) const float* es)
{   
#if defined(AL_MATH_USE_NO_SIMD)
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
#else
    AL_ASSERT_ALWAYS(false);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix4x4::Matrix4x4(const Matrix3x3& m33)
{
#if defined(AL_MATH_USE_NO_SIMD)
    e11 = m33.e11; e12 = m33.e12; e13 = m33.e13; e14 = 0.0f;
    e21 = m33.e21; e22 = m33.e22; e23 = m33.e23; e24 = 0.0f;
    e31 = m33.e31; e32 = m33.e32; e33 = m33.e33; e34 = 0.0f;
    e41 = 0.0f;    e42 = 0.0f;    e43 = 0.0f;    e44 = 1.0f;
#else
    const Vec4 oooz = Vec4(1.0f, 1.0f, 1.0f, 0.0f);
    row0 = _mm_mul_ps(m33.row0, oooz);
    row1 = _mm_mul_ps(m33.row1, oooz);
    row2 = _mm_mul_ps(m33.row2, oooz);
    row3 = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix4x4::constructAsTranslation(Vec3 v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float x = v.x();
    const float y = v.y();
    const float z = v.z();
    e11 = 1.0f; e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
    e21 = 0.0f; e22 = 1.0f; e23 = 0.0f; e24 = 0.0f;
    e31 = 0.0f; e32 = 0.0f; e33 = 1.0f; e34 = 0.0f;
    e41 = x;    e42 = y;    e43 = z;    e44 = 1.0f;
#else
    row0 = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    row1 = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
    row2 = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
    row3 = _mm_add_ps(_mm_mul_ps(v, _mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f)), _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix4x4::constructAsScale(Vec3 scale)
{
#if defined(AL_MATH_USE_NO_SIMD)
    const float sx = scale.x();
    const float sy = scale.y();
    const float sz = scale.z();
    e11 = sx;   e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
    e21 = 0.0f; e22 = sy;   e23 = 0.0f; e24 = 0.0f;
    e31 = 0.0f; e32 = 0.0f; e33 = sz;   e34 = 0.0f;
    e41 = 0.0f; e42 = 0.0f; e43 = 0.0f; e44 = 1.0f;
#else
    row0 = _mm_mul_ps(scale, _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f));
    row1 = _mm_mul_ps(scale, _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f));
    row2 = _mm_mul_ps(scale, _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f));
    row3 = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
#endif
}


/*
-------------------------------------------------
http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/
回転軸は正規化されている前提
-------------------------------------------------
*/
INLINE void Matrix4x4::constructAsRotation(Vec3 axis, float angle)
{
    AL_ASSERT_DEBUG(axis.isNormalized());
#if defined(AL_MATH_USE_NO_SIMD)
    //
    const float s = std::sinf(angle);
    const float c = std::cosf(angle);
    const float t = 1.0f - c;
    const float x = axis.x();
    const float y = axis.y();
    const float z = axis.z();
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
#else
    // NOTE: 仮実装
    const float s = std::sinf(angle);
    const float c = std::cosf(angle);
    const float t = 1.0f - c;
    const float x = axis.x();
    const float y = axis.y();
    const float z = axis.z();
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
    row0 = _mm_set_ps(0.0f, txy + sy, txy - sz, c + txx);
    row1 = _mm_set_ps(0.0f, tyz - sx, c + tyy, txy + sz);
    row2 = _mm_set_ps(0.0f, c + tzz, tyz + sx, txz - sy);
    row3 = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix4x4::constructAsViewMatrix(Vec3 origin,
    Vec3 target,
    Vec3 up)
{
    //
    const Vec3 zaxis = (target - origin).normalized();
    const Vec3 xaxis = Vec3::cross(up, zaxis).normalized();
    const Vec3 yaxis = Vec3::cross(zaxis, xaxis);
#if defined(AL_MATH_USE_NO_SIMD)
    e11 = xaxis.x(); e12 = yaxis.x(); e13 = zaxis.x(); e14 = 0.0f;
    e21 = xaxis.y(); e22 = yaxis.y(); e23 = zaxis.y(); e24 = 0.0f;
    e31 = xaxis.z(); e32 = yaxis.z(); e33 = zaxis.z(); e34 = 0.0f;
    e41 = -Vec3::dot(xaxis, origin);
    e42 = -Vec3::dot(yaxis, origin);
    e43 = -Vec3::dot(zaxis, origin);
    e44 = 1.0f;
#else
    Matrix3x3 tmp;
    tmp.row0 = xaxis;
    tmp.row1 = yaxis;
    tmp.row2 = zaxis;
    tmp.transpose();
    row0 = xaxis;
    row1 = yaxis;
    row2 = zaxis;
    // transpose
    const __m128 tmp0 = _mm_shuffle_ps(row0, row1, 0x44);
    const __m128 tmp2 = _mm_shuffle_ps(row0, row1, 0xEE);
    const __m128 tmp1 = _mm_shuffle_ps(row2, row2, 0x44);
    const __m128 tmp3 = _mm_shuffle_ps(row2, row2, 0xEE);
    row0 = _mm_mul_ps(_mm_shuffle_ps(tmp0, tmp1, 0x88), _mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f));
    row1 = _mm_mul_ps(_mm_shuffle_ps(tmp0, tmp1, 0xDD), _mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f));
    row2 = _mm_mul_ps(_mm_shuffle_ps(tmp2, tmp3, 0x88), _mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f));
    // row3の計算
    const __m128 r30 = _mm_dp_ps(xaxis, origin, 0x7F);
    const __m128 r31 = _mm_dp_ps(yaxis, origin, 0x7F);
    const __m128 r32 = _mm_dp_ps(zaxis, origin, 0x7F);
    const __m128 r33 = _mm_set_ps1(-1.0f);
    const __m128 tmp4 = _mm_shuffle_ps(r30, r31, _MM_SHUFFLE(0, 0, 0, 0));
    const __m128 tmp5 = _mm_shuffle_ps(r32, r33, _MM_SHUFFLE(0, 0, 0, 0));
    const __m128 tmp6 = _mm_shuffle_ps(tmp4, tmp5, _MM_SHUFFLE(2, 0, 2, 0));
    const __m128 neg = _mm_set_ps1(-1.0f);
    row3 = _mm_mul_ps(neg, tmp6);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix4x4::fillZero()
{
#if defined(AL_MATH_USE_NO_SIMD)
    e11 = 0.0f; e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
    e21 = 0.0f; e22 = 0.0f; e23 = 0.0f; e24 = 0.0f;
    e31 = 0.0f; e32 = 0.0f; e33 = 0.0f; e34 = 0.0f;
    e41 = 0.0f; e42 = 0.0f; e43 = 0.0f; e44 = 1.0f;
#else
    AL_ASSERT_ALWAYS(false);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix4x4::identity()
{
#if defined(AL_MATH_USE_NO_SIMD)
    e11 = 1.0f; e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
    e21 = 0.0f; e22 = 1.0f; e23 = 0.0f; e24 = 0.0f;
    e31 = 0.0f; e32 = 0.0f; e33 = 1.0f; e34 = 0.0f;
    e41 = 0.0f; e42 = 0.0f; e43 = 0.0f; e44 = 1.0f;
#else
    // NOTE: set_ps()直接は遅いかもしれない
    row0 = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
    row1 = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
    row2 = _mm_set_ps(0.0f, 1.0f, 1.0f, 0.0f);
    row3 = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Matrix4x4::transform(Vec3 v) const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const Vec4 nv(v.x(), v.y(), v.z(), 1.0f);
    return
        Vec3(
            Vec4::dot(columnVector(0), nv),
            Vec4::dot(columnVector(1), nv),
            Vec4::dot(columnVector(2), nv));
#else
    __m128 tmp;
    tmp = _mm_fmadd_ps(v.xxx(), row0, row3);
    tmp = _mm_fmadd_ps(v.yyy(), row1, tmp);
    tmp = _mm_fmadd_ps(v.zzz(), row2, tmp);
    return tmp;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec4 Matrix4x4::transform(Vec4 v) const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec4(
            Vec4::dot(columnVector(0), v),
            Vec4::dot(columnVector(1), v),
            Vec4::dot(columnVector(2), v),
            Vec4::dot(columnVector(3), v));
#else
    AL_ASSERT_ALWAYS(false);
#endif
}

/*
-------------------------------------------------
operator[]
RawVectorへのアクセス
-------------------------------------------------
*/
INLINE const Vec4& Matrix4x4::operator [] (int32_t index) const
{
#if defined(AL_MATH_USE_NO_SIMD)
    AL_ASSERT_DEBUG(0 <= index && index <= 3);
    return *(Vec4*)(e + index * 4);
#else
    AL_ASSERT_ALWAYS(false);
    switch (index)
    {
    case 0: return row0;
    case 1: return row1;
    case 2: return row2;
    case 3: return row3;
}
    AL_ASSERT_ALWAYS(false);
    return row0;
#endif
}

/*
-------------------------------------------------
operator[]
RowVectorへのアクセス
-------------------------------------------------
*/
INLINE Vec4& Matrix4x4::operator [] (int32_t index)
{
#if defined(AL_MATH_USE_NO_SIMD)
    AL_ASSERT_DEBUG(0 <= index && index <= 3);
    return *(Vec4*)(e + index * 4);
#else
    AL_ASSERT_ALWAYS(false);
    return Vec4(0.0f); // TODO: ローカル変数だからよくない
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec4 Matrix4x4::columnVector(int32_t index) const
{
#if defined(AL_MATH_USE_NO_SIMD)
    AL_ASSERT_DEBUG(0 <= index && index <= 3);
    return Vec4(
        *(&e11 + index),
        *(&e21 + index),
        *(&e31 + index),
        *(&e41 + index)
    );
#else
    AL_ASSERT_ALWAYS(false);
    return Vec4(0.0f);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Matrix4x4::det() const
{
#if defined(AL_MATH_USE_NO_SIMD)
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
        +e11 * d1
        - e12 * d2
        + e13 * d3
        - e14 * d4;
    return d;
#else
    // NOTE: 仮実装
    const float e11 = _mm_extract_ps_fast<0>(row0);
    const float e12 = _mm_extract_ps_fast<1>(row0);
    const float e13 = _mm_extract_ps_fast<2>(row0);
    const float e14 = _mm_extract_ps_fast<3>(row0);
    //
    const float e21 = _mm_extract_ps_fast<0>(row1);
    const float e22 = _mm_extract_ps_fast<1>(row1);
    const float e23 = _mm_extract_ps_fast<2>(row1);
    const float e24 = _mm_extract_ps_fast<3>(row1);
    //
    const float e31 = _mm_extract_ps_fast<0>(row2);
    const float e32 = _mm_extract_ps_fast<1>(row2);
    const float e33 = _mm_extract_ps_fast<2>(row2);
    const float e34 = _mm_extract_ps_fast<3>(row2);
    //
    const float e41 = _mm_extract_ps_fast<0>(row3);
    const float e42 = _mm_extract_ps_fast<1>(row3);
    const float e43 = _mm_extract_ps_fast<2>(row3);
    const float e44 = _mm_extract_ps_fast<3>(row3);
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
        +e11 * d1
        - e12 * d2
        + e13 * d3
        - e14 * d4;
    return d;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix4x4::inverse()
{
#if defined(AL_MATH_USE_NO_SIMD)
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
#else
    // NOTE: 仮実装
    const float e11 = _mm_extract_ps_fast<0>(row0);
    const float e12 = _mm_extract_ps_fast<1>(row0);
    const float e13 = _mm_extract_ps_fast<2>(row0);
    const float e14 = _mm_extract_ps_fast<3>(row0);
    //
    const float e21 = _mm_extract_ps_fast<0>(row1);
    const float e22 = _mm_extract_ps_fast<1>(row1);
    const float e23 = _mm_extract_ps_fast<2>(row1);
    const float e24 = _mm_extract_ps_fast<3>(row1);
    //
    const float e31 = _mm_extract_ps_fast<0>(row2);
    const float e32 = _mm_extract_ps_fast<1>(row2);
    const float e33 = _mm_extract_ps_fast<2>(row2);
    const float e34 = _mm_extract_ps_fast<3>(row2);
    //
    const float e41 = _mm_extract_ps_fast<0>(row3);
    const float e42 = _mm_extract_ps_fast<1>(row3);
    const float e43 = _mm_extract_ps_fast<2>(row3);
    const float e44 = _mm_extract_ps_fast<3>(row3);
    //
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
    row0 = _mm_set_ps(
        id * m41.det() * (-1.0f),
        id * m31.det() * (+1.0f),
        id * m21.det() * (-1.0f),
        id * m11.det() * (+1.0f));
    //
    row1 = _mm_set_ps(
        id * m42.det() * (+1.0f),
        id * m32.det() * (-1.0f),
        id * m22.det() * (+1.0f),
        id * m12.det() * (-1.0f));
    //
    row2 = _mm_set_ps(
        id * m43.det() * (-1.0f),
        id * m33.det() * (+1.0f),
        id * m23.det() * (-1.0f),
        id * m13.det() * (+1.0f));


    row3 = _mm_set_ps(
        id * m44.det() * (+1.0f),
        id * m34.det() * (-1.0f),
        id * m24.det() * (+1.0f),
        id * m14.det() * (-1.0f));

    return;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix4x4::transpose()
{
#if defined(AL_MATH_USE_NO_SIMD)
    std::swap(e12, e21);
    std::swap(e13, e31);
    std::swap(e14, e41);
    std::swap(e23, e32);
    std::swap(e24, e42);
    std::swap(e34, e43);
#else
    const __m128 tmp0 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(1, 0, 1, 0));
    const __m128 tmp2 = _mm_shuffle_ps(row0, row1, _MM_SHUFFLE(3, 2, 3, 2));
    const __m128 tmp1 = _mm_shuffle_ps(row2, row3, _MM_SHUFFLE(1, 0, 1, 0));
    const __m128 tmp3 = _mm_shuffle_ps(row2, row3, _MM_SHUFFLE(3, 2, 3, 2));
    row0 = _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0));
    row1 = _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(3, 1, 3, 1));
    row2 = _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(2, 0, 2, 0));
    row3 = _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(3, 1, 3, 1));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix4x4 Matrix4x4::inversed() const
{
    Matrix4x4 m = *this;
    m.inverse();
    return m;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix4x4 Matrix4x4::transposed() const
{
    Matrix4x4 m = *this;
    m.transpose();
    return m;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix3x3 Matrix4x4::extract3x3() const
{
#if defined(AL_MATH_USE_NO_SIMD)
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
#else
    Matrix3x3 m;
    m.row0 = row0;
    m.row1 = row1;
    m.row2 = row2;
    return m;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix4x4 Matrix4x4::mul(const Matrix4x4& lhs, const Matrix4x4& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
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
#else
    Matrix4x4 tmp = rhs;
    tmp.transpose();
    // TODO: floatinvecのまま受けてシャッフルすべき
    Matrix4x4 m;
    m.row0 =
        _mm_set_ps(
            Vec4::dot(lhs.row0, tmp.row3),
            Vec4::dot(lhs.row0, tmp.row2),
            Vec4::dot(lhs.row0, tmp.row1),
            Vec4::dot(lhs.row0, tmp.row0));
    m.row1 =
        _mm_set_ps(
            Vec4::dot(lhs.row1, tmp.row3),
            Vec4::dot(lhs.row1, tmp.row2),
            Vec4::dot(lhs.row1, tmp.row1),
            Vec4::dot(lhs.row1, tmp.row0));
    m.row2 =
        _mm_set_ps(
            Vec4::dot(lhs.row2, tmp.row3),
            Vec4::dot(lhs.row2, tmp.row2),
            Vec4::dot(lhs.row2, tmp.row1),
            Vec4::dot(lhs.row2, tmp.row0));
    m.row3 =
        _mm_set_ps(
            Vec4::dot(lhs.row3, tmp.row3),
            Vec4::dot(lhs.row3, tmp.row2),
            Vec4::dot(lhs.row3, tmp.row1),
            Vec4::dot(lhs.row3, tmp.row0));
    return m;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec4 Matrix4x4::mul(const Vec4& v, const Matrix4x4& m)
{
#if defined(AL_MATH_USE_NO_SIMD)
    return
        Vec4(
            v.x() * m.e11 + v.y() * m.e12 + v.z() * m.e13 + v.w() * m.e14,
            v.x() * m.e21 + v.y() * m.e22 + v.z() * m.e23 + v.w() * m.e24,
            v.x() * m.e31 + v.y() * m.e32 + v.z() * m.e33 + v.w() * m.e34,
            v.x() * m.e41 + v.y() * m.e42 + v.z() * m.e43 + v.w() * m.e44);
#else
    // TODO: floatinvecのまま受けてシャッフルすべき
    const float x = Vec4::dot(v, m.row0);
    const float y = Vec4::dot(v, m.row1);
    const float z = Vec4::dot(v, m.row2);
    const float w = Vec4::dot(v, m.row3);
    return Vec4(x, y, z, w);
#endif
}


/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Matrix4x4& Matrix4x4::operator=(const Matrix4x4 & other)
{
#if defined(AL_MATH_USE_NO_SIMD)
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
#else
    row0 = other.row0;
    row1 = other.row1;
    row2 = other.row2;
    row3 = other.row3;
#endif
    return *this;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Matrix4x4::extractViewmatRight() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(e11, e21, e31);
#else
    AL_ASSERT_ALWAYS(false);
    return Vec3();
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Vec3 Matrix4x4::extractViewmatUp() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(e12, e22, e32);
#else
    AL_ASSERT_ALWAYS(false);
    return Vec3();
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Vec3 Matrix4x4::extractViewmatDir() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    return Vec3(e13, e23, e33);
#else
    AL_ASSERT_ALWAYS(false);
    return Vec3();
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Matrix4x4 operator + (const Matrix4x4& lhs, const Matrix4x4& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
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
#else
    AL_ASSERT_ALWAYS(false);
    return lhs;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE Matrix4x3::Matrix4x3(_In_reads_(12) const float* es)
{
    e11 = es[0]; e12 = es[1]; e13 = es[2]; e14 = es[3];
    e21 = es[4]; e22 = es[5]; e23 = es[6]; e24 = es[7];
    e31 = es[8]; e32 = es[9]; e33 = es[10]; e34 = es[11];
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix4x3::fillZero()
{
    e11 = 0.0f; e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
    e21 = 0.0f; e22 = 0.0f; e23 = 0.0f; e24 = 0.0f;
    e31 = 0.0f; e32 = 0.0f; e33 = 0.0f; e34 = 0.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Matrix4x3::identity()
{
    e11 = 1.0f; e12 = 0.0f; e13 = 0.0f; e14 = 0.0f;
    e21 = 0.0f; e22 = 1.0f; e23 = 0.0f; e24 = 0.0f;
    e31 = 0.0f; e32 = 0.0f; e33 = 1.0f; e34 = 0.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Quat::Quat(Vec3 xyz, float s)
{
    qv_ = xyz;
    qs_ = s;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Quat::Quat(float x, float y, float z, float w)
{
    qv_ = Vec3(x, y, z);
    qs_ = w;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3 Quat::qv() const
{
    return qv_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Quat::qs() const
{
    return qs_;
}

/*
-------------------------------------------------
無回転にする
-------------------------------------------------
*/
INLINE void Quat::identity()
{
    qv_.zero();
    qs_ = 1.0f;
}

/*
-------------------------------------------------
共役を返す
-------------------------------------------------
*/
INLINE Quat Quat::conjugated() const
{
    return Quat(-qv_, qs_);
}

/*
-------------------------------------------------
ベクトルの回転
回転にのみ利用するので常に正規化されている前提で使われる。
そのため、逆数は共役と等しくなる
-------------------------------------------------
*/
//
INLINE Vec3 Quat::rotate(Vec3 av) const
{
    const Quat v(av, 0.0f);
    const Quat q = *this;
    const Quat qc = conjugated();
    const Quat r = q * v * qc;
    return r.qv_;
}

/*
-------------------------------------------------
回転行列にする
-------------------------------------------------
*/
INLINE Matrix3x3 Quat::toMatrix() const
{
    const float x = qv_.x();
    const float y = qv_.y();
    const float z = qv_.z();
    const float w = qs_;
    return
        Matrix3x3(
            // e11
            1.0f - 2.0f * y * y - 2.0f * z * z,
            // e12
            2.0f * x * y + 2.0f * z * w,
            // e13
            2.0f * x * z - 2.0f * y * w,
            // e21
            2.0f * x * y - 2.0f * z * w,
            // e22
            1.0f - 2.0f * x * x - 2.0f * z * z,
            // e23
            2.0f * y * z + 2.0f * x * w,
            // e31
            2.0f * x * z + 2.0f * y * w,
            // e32
            2.0f * y * z + 2.0f * x * w,
            // e33
            1.0f - 2.0f * x * x - 2.0f * y * y
        );
}

/*
-------------------------------------------------
正規化
-------------------------------------------------
*/
INLINE Quat Quat::normalized() const
{
    const float invlen = 1.0f / std::sqrtf(qv_.lengthSq() + qs_ * qs_);
    return Quat(invlen * qv_, invlen * qs_);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Quat Quat::fromAxisRot(Vec3 axis, float theta)
{
    const float ht = theta * 0.5f;
    return Quat(axis * std::sinf(ht), std::cosf(ht));
}

/*
-------------------------------------------------
lerp
-------------------------------------------------
*/
INLINE Quat Quat::lerp(Quat a, Quat b, float t)
{
    const Quat r = a * (1.0f - t) + b * t;
    return r.normalized();
}

/*
-------------------------------------------------
slerp
-------------------------------------------------
*/
INLINE Quat Quat::slerp(Quat p, Quat q, float t)
{
    const float theta = std::acosf(Quat::dot(p, q));
    const float invSinTheta = 1.0f / std::sinf(theta);
    const float wp = std::sinf((1.0f - t) * theta) * invSinTheta;
    const float wq = std::sinf(t * theta) * invSinTheta;
    return p * wp + q * wq;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE float Quat::dot(Quat p, Quat q)
{
    return Vec3::dot(p.qv_, q.qv_) + p.qs_ * q.qs_;
}

/*
-------------------------------------------------
スケール
-------------------------------------------------
*/
INLINE Quat Quat::operator * (float s) const
{
    Quat r = *this;
    r.qv_ *= s;
    r.qs_ *= s;
    return r;
}

/*
-------------------------------------------------
加算
-------------------------------------------------
*/
INLINE Quat Quat::operator + (Quat other) const
{
    Quat r;
    r.qv_ = qv_ + other.qv_;
    r.qs_ = qs_ + other.qs_;
    return r;
}

/*
-------------------------------------------------
グラスマン積
-------------------------------------------------
*/
INLINE Quat Quat::operator * (Quat q) const
{
    Quat p = *this;
    //
    const Vec3 pv = p.qv_;
    const float ps = p.qs_;
    const Vec3 qv = q.qv_;
    const float qs = q.qs_;
    // TODO: もう少し簡略にできるはず
    const Vec3 v = ps * qv + qs * pv + Vec3::cross(pv, qv);
    const float w = ps * qs - Vec3::dot(pv, qv);
    //
    return Quat(v, w);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Vec3Pack8::Vec3Pack8(
                            __m256 axs,
                            __m256 ays,
                            __m256 azs)
:xs(axs), ys(ays), zs(azs)
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static __m256 combine128(__m128 a, __m128 b)
{
    const __m256 tmp = _mm256_castps128_ps256(a);
    return _mm256_insertf128_ps(tmp, b, 1);
}

/*
-------------------------------------------------
xyz_が8回連続で続くfloat配列からの構築
参考: http://www.isus.jp/specials/3d-vector-normalization/
-------------------------------------------------
*/
INLINE void Vec3Pack8::set(
                           // xyz_が8回
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

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Vec3Pack8::set(
                           Vec3 v0,
                           Vec3 v1,
                           Vec3 v2,
                           Vec3 v3,
                           Vec3 v4,
                           Vec3 v5,
                           Vec3 v6,
                           Vec3 v7)
{
#if defined(AL_MATH_USE_NO_SIMD)
    assert(false);
#elif defined(AL_MATH_USE_AVX2)
    const __m256 x0y0z0w0_x4y4z4w4 = combine128(v0, v4);
    const __m256 x1y1z1w1_x5y5z5w5 = combine128(v1, v5);
    const __m256 x2y2z2w2_x6y6z6w6 = combine128(v2, v6);
    const __m256 x3y3z3w3_x7y7z7w7 = combine128(v3, v7);
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Bool8 Vec3Pack8::isNormalized() const
{
    return isNormalized(1.0e-5f);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Float8 Vec3Pack8::length(const Vec3Pack8& v)
{
#if defined(AL_MATH_USE_NO_SIMD)
    assert(false);
#elif defined(AL_MATH_USE_AVX2)
    return _mm256_sqrt_ps(lengthSq(v));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Float8 Vec3Pack8::distance(const Vec3Pack8& lhs, const Vec3Pack8& rhs)
{
#if defined(AL_MATH_USE_NO_SIMD)
    assert(false);
#elif defined(AL_MATH_USE_AVX2)
    return _mm256_sqrt_ps(distanceSq(lhs, rhs));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE static Vec3Pack8 operator * (const Vec3Pack8& v, float f)
{
    return f * v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
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

/*
-------------------------------------------------
-------------------------------------------------
*/
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
