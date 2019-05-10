#include "sampler/sampler.hpp"
#include "core/floatstreamstats.hpp"
#include "util/mathmaticautil.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
const float ONE_MINUS_EPS = 0.999999940f; // nextFloatDown(1.0f);

/*
-------------------------------------------------
-------------------------------------------------
*/
void Sampler::setHash(uint64_t hash)
{
    AL_ASSERT_ALWAYS(hash != 0);
    hash_ = hash;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Sampler::startSample(int32_t sampleNo)
{
    //
    AL_ASSERT_DEBUG(hash_ != 0);
    //
    sampleNo_ = sampleNo;
    dimention_ = 0;
    //
    onStartSample(sampleNo);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
uint32_t Sampler::dimention() const { return dimention_; }

/*
-------------------------------------------------
-------------------------------------------------
*/
void Sampler::setDimention(uint32_t dimention) { dimention_ = dimention; }

/*
-------------------------------------------------
-------------------------------------------------
*/
Vec2 Sampler::get2d()
{
    const float x = get1d();
    const float y = get1d();
    return Vec2(x, y);
}

/*
-------------------------------------------------
Squre -> Discの変換
動径方向(u2)には一様だが、u1方向には歪みを持たせて
結果的に一様にしているため、不都合な場合がある。
詳しくはpbrt2 13.6.2を参照
-------------------------------------------------
*/
Vec2 Sampler::getDiscAccurate()
{
    const Vec2 xy = get2d();
    //
    const float r = std::sqrtf(xy.x());
    const float theta = xy.y() * PI * 2.0f;
    return Vec2(r * std::cosf(theta), r * sin(theta));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Vec2 Sampler::getDiscConcentric()
{
    const Vec2 xy = get2d();
    //
    // [0,1] -> [-1,1]
    const float sx = 2.0f * xy.x() - 1.0f;
    const float sy = 2.0f * xy.y() - 1.0f;
    //
    Vec2 uv;
    if (sx == 0.0 && sy == 0.0)
    {
        uv = Vec2(0.0f, 0.0f);
        return uv;
    }
    //
    float r;
    float theta;
    if (sx >= -sy)
    {
        // 右の領域
        if (sx > sy)
        {
            r = sx;
            if (sy > 0.0f)
            {
                theta = sy / r;
            }
            else
            {
                theta = 8.0f + sy / r;
            }
        }
        // 上の領域
        else
        {
            r = sy;
            theta = 2.0f - sx / r;
        }
    }
    else
    {
        // 左の領域
        if (sx <= sy)
        {
            r = -sx;
            theta = 4.0f - sy / r;
        }
        // 下の領域
        else
        {
            r = -sy;
            theta = 6.0f + sx / r;
        }
    }
    //
    theta *= PI * 0.25f;
    uv.setX(r * std::cosf(theta));
    uv.setY(r * std::sinf(theta));
    AL_ASSERT_DEBUG(-1.0f <= uv.x() && uv.x() <= 1.0f);
    AL_ASSERT_DEBUG(-1.0f <= uv.y() && uv.y() <= 1.0f);
    return uv;
}

/*
-------------------------------------------------
Hemisphere(xyz_)
単位球上で一様に分布する
-------------------------------------------------
*/
Vec3 Sampler::getHemisphere(float* pdf)
{
    //
    const Vec2 xy = get2d();
    const float phi = 2.0f * PI * xy.x();
    const float sinPhi = std::sinf(phi);
    const float cosPhi = std::cosf(phi);
    const float cosTheta = 1.0f - xy.y();
    const float sinTheta = std::sqrtf(1 - cosTheta * cosTheta);
    const Vec3 r = { sinTheta * cosPhi, sinTheta * sinPhi, cosTheta };
    if (pdf != nullptr)
    {
        *pdf = INV_PI * 0.5f;
    }
    //
    AL_ASSERT_DEBUG(std::fabsf(Vec3::dot(r, r) - 1.0f) < 0.01f);
    return r;
}

/*
-------------------------------------------------
Hemisphere(xyz_)
自動的にcosine weightになる
-------------------------------------------------
*/
Vec3 Sampler::getHemisphereCosineWeighted(float* pdf)
{
    //
    const Vec2 uv = getDiscConcentric();
    //
    AL_ASSERT_DEBUG(-1.0f <= uv.x() && uv.x() <= +1.0f);
    AL_ASSERT_DEBUG(-1.0f <= uv.y() && uv.y() <= +1.0f);
    AL_ASSERT_DEBUG(uv.lengthSq() <= 1.0001f);
    //
    const float lenSq = Vec2::dot(uv, uv);
    const float z = std::sqrtf(std::max(1.0f - lenSq, 0.0f));
    const Vec3 r = {uv.x(), uv.y(), z};
    //
    if (pdf != nullptr)
    {
        const float cosTheta = z;
        *pdf = cosTheta;
    }
    //
    AL_ASSERT_DEBUG(std::fabsf(Vec3::dot(r, r) - 1.0f) < 0.01f);
    return r;
}

/*
-------------------------------------------------
Sphere(xyz_)
-------------------------------------------------
*/
Vec3 Sampler::getSphere(float* pdf)
{
    const Vec2 xy = get2d();
    const float z = 2.0f * xy.x() - 1.0f;
    const float theta = (2.0f * xy.y() - 1.0f) * PI;
    const float iz = sqrtf(1.0f - z * z);
    const float x = sinf(theta) * iz;
    const float y = cosf(theta) * iz;
    *pdf = INV_PI * 0.25f;
    return Vec3{x, y, z};
}

/*
-------------------------------------------------
Cone(xyz_)
Coneの球冠をサンプル。-1を指定すると球面上と同等になる。
-------------------------------------------------
*/
Vec3 Sampler::getCone(float cosThetaMax)
{
    const Vec2 xy = get2d();
    const float cosTheta = alLerp2(cosThetaMax, 1.0f, xy.x());
    const float sinTheta = std::sqrtf(1.0f - cosTheta * cosTheta);
    const float phi = 2.0f * PI * xy.y();
    const float cosPhi = std::cosf(phi);
    const float sinPhi = std::sinf(phi);
    return Vec3{cosPhi * sinTheta, sinPhi * sinTheta, cosTheta};
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Vec2 Sampler::getTriangle()
{
#if 1 // こちらの方が2割早い
    const Vec2 xy = get2d();
    const float tmp = std::sqrtf(xy.x());
    return {1.0f - tmp, xy.y() * tmp};
#else
    Vec2 xy = get2d();
    if (xy.x() + xy.y() >= 1.0f)
    {
        xy = Vec2::one() - xy;
    }
    return xy;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
uint32_t Sampler::getSize(uint32_t size)
{
    const int32_t v = int32_t(get1d() * float(size));
    AL_ASSERT_DEBUG(v < int32_t(size));
    return v;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
REGISTER_OBJECT(Sampler, SamplerIndepent);

/*
-------------------------------------------------
-------------------------------------------------
*/
SamplerIndepent::SamplerIndepent(const ObjectProp& prop)
{
    //
}
/*
-------------------------------------------------
-------------------------------------------------
*/
float SamplerIndepent::get1d()
{
    ++dimention_;
    return std::min(ONE_MINUS_EPS, rng_.nextFloat());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SamplerIndepent::onStartSample(uint32_t sampleNo)
{
    /*
    NOTE:
    (ハッシュ,サンプル番号,次元)から同一の結果を返す必要があるためだが
    ここでsampleNoからseedを決定してるがこのような動作は
    SamplerIndepentのみ許されている。
    他のサンプラーではseedはハッシュ値からのみ決定すること
    */
    const uint64_t seed = std::hash<uint32_t>{}(sampleNo) ^ hash_;
    rng_ = XorShift128(uint32_t(seed));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
REGISTER_OBJECT(Sampler, SamplerHalton);

/*
-------------------------------------------------
-------------------------------------------------
*/
SamplerHalton::SamplerHalton(const ObjectProp& prop) {}

/*
-------------------------------------------------
-------------------------------------------------
*/
float SamplerHalton::get1d()
{
    const float offset = offsets_[dimention_ % offsets_.size()];
    const float tmp = radicalInverseFast(dimention_, sampleNo_) + offset;
// NOTE: tmpは[0,2]が確定しているのでfmodf()を使わないで高速に計算できる
#if 0
    const float v = std::fmodf(tmp, 1.0f);
    const float sv = std::min(v, ONE_MINUS_EPS);
#else
    const float v = (tmp >= 1.0f) ? (tmp - 1.0f) : tmp;
    const float sv = (v > ONE_MINUS_EPS) ? ONE_MINUS_EPS : v;
#endif
    ++dimention_;
    return sv;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SamplerHalton::onStartSample(uint32_t sampleNo)
{
    (void)sampleNo;
    XorShift128 rng((int32_t)hash_);
    offsets_.resize(512);
    for (auto& offset : offsets_)
    {
        offset = rng.nextFloat();
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
SphericalFibonacci::SphericalFibonacci(uint32_t seed) { rng_.setSeed(seed); }

/*
-------------------------------------------------
-------------------------------------------------
*/
SphericalFibonacci::SphericalFibonacci(const ObjectProp& objectProp)
{
    // カテゴリー名が"SamplerHemisphere"かチェック
    // AL_ASSERT_DEBUG(objectProp.tag() == "SamplerHemisphere");

    // TODO: オフセット指定を入れる？

    // シード設定する
    const int32_t seed =
        objectProp.findChildBy("name", "seed").asInt(123456789);
    rng_.setSeed(seed);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SphericalFibonacci::isCosineWeighted() const { return true; }

/*
-------------------------------------------------
-------------------------------------------------
*/
void SphericalFibonacci::prepare(const int32_t sampleNum)
{
    if (permutation_.size() != sampleNum)
    {
        permutation_.resize(sampleNum);
        std::iota(permutation_.begin(), permutation_.end(), 0);
    }
    // 順列をシャッフル
    rng_.shuffle(permutation_.begin(), permutation_.end());
    //
    invSampleNum_ = 1.0f / float(sampleNum);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Vec3 SphericalFibonacci::sample(int32_t sampleNo)
{
    AL_ASSERT_DEBUG(sampleNo < permutation_.size());
#if 0
    const float sn = float(permutation_[sampleNo]);
#else
    const float sn = float(sampleNo);
#endif
    const float tmp = (sqrtf(5.0f) - 1.0f) * sn * 0.5f;
    const float phi = (tmp - std::floorf(tmp)) * 2.0f * PI;
    ;
    const float z = sn * invSampleNum_;
    const float theta = std::acosf(z);
    const float sinTheta = std::sinf(theta);
    const float x = std::cosf(phi) * sinTheta;
    const float y = std::sinf(phi) * sinTheta;
    return Vec3(x, y, z);
}