#pragma once

#include "core/object.hpp"
#include "core/rng.hpp"
#include "sampler/samplerutil.inl"

/*
-------------------------------------------------
xy: [0,1]のSquare上の一点
uv: 半径1の円上の一点。
xyz_: 単位球上での一点。(z-up)
tp: (theta,phi)。球面座標。それぞれPIと2PIで割られているので[0,1]。
-------------------------------------------------
*/

// SphericalCoordinate(tp) -> Dir(xyz_)
Vec3
remapSphericalCoordToDir(Vec2 tp);

// Dir(xyz_) -> SphericalCoordinate(tp)
Vec2
remapDirToSphericalCoord(Vec3 xyz_);

/*
-------------------------------------------------
ユーティリティ
-------------------------------------------------
*/
INLINE void
latinHypercube(RNG& rng, float* buf, uint32_t bufLen, size_t dim);
INLINE float
radicalInverseCore(int32_t basePrimNumber, int32_t index);
INLINE float
radicalInverseSlow(int32_t primNumberIndex, int32_t index);
INLINE float
radicalInverseFast(int32_t primNumberIndex, int32_t index);
INLINE float
radicalInverseBase2(uint32_t n, uint32_t scramble);
INLINE float
sobol2(uint32_t n, uint32_t scramble);
INLINE uint32_t
roundupNextSquareRoot(uint32_t v);
float
estimateStarDiscrepancy(const std::vector<Vec2>& srcSamples);
float
minimumDisptance(const std::vector<Vec2>& srcSamples);

/*
-------------------------------------------------
サンプラー
(ハッシュ,サンプル番号,次元)が同一であれば常に同じ値が得られる実装を心がける
-------------------------------------------------
*/
class Sampler;
typedef std::shared_ptr<Sampler> SamplerPtr;
class Sampler : public Object
{
public:
    Sampler() = default;
    virtual ~Sampler() = default;
    /*
    -------------------------------------------------
    ピクセルや頂点に固有のハッシュ値を設定する
    -------------------------------------------------
    */
    void setHash(uint64_t hash);
    /*
    -------------------------------------------------
    サンプル番号を指定
    -------------------------------------------------
    */
    void startSample(int32_t sampleNo);
    /*
    -------------------------------------------------
    次元の取得と設定
    -------------------------------------------------
    */
    uint32_t dimention() const;
    void setDimention(uint32_t dimention);
    /*
    -------------------------------------------------
    サンプルの取得
    厳密に[0,1)を守るようにする
    -------------------------------------------------
    */
    virtual float get1d() = 0;
    Vec2 get2d();

    // Disc(uv)
    Vec2 getDiscAccurate();
    Vec2 getDiscConcentric();

    // Hemisphere(xyz_)
    Vec3 getHemisphere(float* pdf = nullptr);
    Vec3 getHemisphereCosineWeighted(float* pdf = nullptr);

    // Sphere(xyz_)
    Vec3 getSphere(float* pdf);

    // Cone(xyz_でz軸が中心)
    Vec3 getCone(float cosTheta);

    // Triangleの重心座標
    Vec2 getTriangle();

    /*
    -------------------------------------------------
    サンプルの取得
    [0,size)までの整数値を返す
    -------------------------------------------------
    */
    uint32_t getSize(uint32_t size);

protected:
    virtual void onStartSample(uint32_t sampleNo) = 0;

protected:
    uint64_t hash_ = 0;
    uint32_t sampleNo_ = 0;
    uint32_t dimention_ = 0;
};

/*
-------------------------------------------------
一様乱数サンプラー
-------------------------------------------------
*/
class SamplerIndepent AL_FINAL : public Sampler
{
public:
    SamplerIndepent() = default;
    SamplerIndepent(const ObjectProp& prop);
    float get1d() override;

protected:
    void onStartSample(uint32_t sampleNo) override;

private:
    XorShift128 rng_;
};

/*
-------------------------------------------------
Halton列
-------------------------------------------------
*/
class SamplerHalton AL_FINAL : public Sampler
{
public:
    SamplerHalton() = default;
    SamplerHalton(const ObjectProp& prop);
    float get1d() override;

protected:
    void onStartSample(uint32_t sampleNo) override;

private:
    std::vector<float> offsets_;
};

#if 0
/*
-------------------------------------------------
層化サンプラー
普通に実装すると事前にサンプル数を指定しないとならなくなるので
Stochastic universal sampling
https://en.wikipedia.org/wiki/Stochastic_universal_sampling
を各次元に対して指定するような仕組みにする
-------------------------------------------------
*/
class SamplerStratified AL_FINAL
    :public Sampler
{
public:
    float get1d() override;
protected:
    void onStartSample(uint32_t sampleNo) override;
private:
    XorShift128 rng_;
};
#endif

/*
-------------------------------------------------
特殊なサンプラー
半球上のみを取り扱うときに使える
-------------------------------------------------
*/
class SphericalFibonacci AL_FINAL
{
public:
    SphericalFibonacci(uint32_t seed = 123456789);
    SphericalFibonacci(const ObjectProp& prop);
    bool isCosineWeighted() const;
    void prepare(const int32_t sampleNum);
    Vec3 sample(int32_t sampleNo);

private:
    XorShift128 rng_;
    std::vector<int32_t> permutation_;
    float invSampleNum_ = 0.0f;
};
