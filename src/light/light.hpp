#pragma once

#include "pch.hpp"
#include "core/object.hpp"
#include "core/ray.hpp"
#include "core/spectrum.hpp"
#include "core/rng.hpp"
#include "core/distribution2d.hpp"
#include "core/orthonormalbasis.hpp"
#include "sampler/sampler.hpp"
#include "shape/shape.hpp"

/*
-------------------------------------------------
Light
-------------------------------------------------
*/
class Light
    :public Shape
{
public:
    Light(const ObjectProp& objectProp);
    virtual ~Light() = default;
    int32_t sampleNum() const;
    virtual bool isDeltaFunc() const = 0;
    virtual Spectrum emittion(
        Vec3 targetPos,
        Vec3 dir ) const = 0;
    virtual float pdf(
        Vec3 targetPos,
        Vec3 dir ) const = 0;
    virtual Spectrum sampleLe(
        SamplerPtr sampler,
        Vec3 targetPos,
        _Out_ Vec3* samplePos,
        _Out_ float* pdf) const = 0;
protected:
    int32_t sampleNum_;
};
typedef std::shared_ptr<Light> LightPtr;

/*
-------------------------------------------------
ConstantLight
-------------------------------------------------
*/
class ConstantLight
    :public Light
{
public:
    ConstantLight();
    ConstantLight(const ObjectProp& objectProp);
    ~ConstantLight() {}
    void init(const Spectrum& spectrum);
    bool isDeltaFunc() const override;
    Spectrum emittion(
        Vec3 targetPos,
        Vec3 dir) const override;
    float pdf(
        Vec3 targetPos,
        Vec3 dir ) const override;
    virtual Spectrum sampleLe(
        SamplerPtr sampler,
        Vec3 targetPos,
        _Out_ Vec3* samplePos,
        _Out_ float* pdf) const override;

    // ShapeのOverride
    bool hasAABB() const override
    {
        AL_ASSERT_DEBUG(!"not impl");
        return false;
    }
    AABB aabb() const override
    {
        AL_ASSERT_DEBUG(!"not impl");
        return AABB();
    }
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override
    {
        AL_ASSERT_DEBUG(!"not impl");
        return false;
    }

private:
    BSDFPtr black;
    Spectrum spectrum_;
    float faraway_;
};

/*
-------------------------------------------------
RectangleLight
-------------------------------------------------
*/
class RectangleLight
    :public Light
{
public:
    RectangleLight();
    RectangleLight(
        const ObjectProp& objectProp);
    void init(
        const Spectrum& spectrum,
        Vec3 center,
        Vec3 xaxis,
        Vec3 yaxis);
    ~RectangleLight() {}
    bool isDeltaFunc() const override;
    Spectrum emittion(
        Vec3 targetPos,
        Vec3 dir) const override;
    float pdf(
        Vec3 targetPos,
        Vec3 dir ) const override;
    Spectrum sampleLe(
        SamplerPtr sampler,
        Vec3 targetPos,
        _Out_ Vec3* samplePos,
        _Out_ float* pdf ) const override;
    float area() const;

    // ShapeのOverride
    bool hasAABB() const override;
    AABB aabb() const override;
    bool intersect(const Ray& ray, _Inout_ Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;
private:
    //
    Spectrum spectrum_;
    //
    Vec3 origin_;
    Vec3 xaxis2_;
    Vec3 yaxis2_;

    Vec3 xaxisNorm_;
    Vec3 yaxisNorm_;
    Vec3 zaxisNorm_;

    //
    float area_ = 0.0f;
    float invArea_ = 0.0f;

    // 補助用
    Vec3 verts_[4];
    Vec3 n_;
    Vec2 uvs_[4];
};

/*
-------------------------------------------------
SphereLight
-------------------------------------------------
*/
class SphereLight
    :public Light
{
public:
    SphereLight();
    SphereLight(
        const ObjectProp& objectProp);
    void init(
        Vec3 center,
        float radius,
        const Spectrum& emission);
    bool isDeltaFunc() const override;
    Spectrum emittion(
        Vec3 targetPos,
        Vec3 dir) const override;
    float pdf(
        Vec3 targetPos,
        Vec3 dir ) const override;
    Spectrum sampleLe(
        SamplerPtr sampler,
        Vec3 targetPos,
        _Out_ Vec3* samplePos,
        _Out_ float* pdf) const override;

    bool hasAABB() const override
    {
        AL_ASSERT_ALWAYS(false);
        return true;
    }
    AABB aabb() const override
    {
        AL_ASSERT_ALWAYS(false);
        return AABB();
    }
    bool intersect(const Ray& ray, _Inout_ Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override
    {
        AL_ASSERT_ALWAYS(false);
        return false;
    }

private:
    Vec3 center_ = Vec3(0.0f,0.0f,0.0f);
    float radius_ = 0.0f;
    Spectrum emission_;
    float area_ = 0.0f;
    float invArea_ = 0.0f;
};

/*
-------------------------------------------------
EnviromentLight
-------------------------------------------------
*/
class EnviromentLight
    :public Light
{
public:
    EnviromentLight(const ObjectProp& objectProp);
    ~EnviromentLight() {}
    void fill(int32_t w, int32_t h, std::function<Spectrum(float, float)> func);
    bool isDeltaFunc() const override;
    Spectrum emittion(
        Vec3 targetPos,
        Vec3 dir) const override;
    float pdf(
        Vec3 targetPos,
        Vec3 dir) const override
    {
        AL_ASSERT_ALWAYS(false);
        return 0.0f;
    }
    Spectrum sampleLe(
        SamplerPtr sampler,
        Vec3 targetPos,
        _Out_ Vec3* samplePos,
        _Out_ float* pdf) const override;
    const Image& image();

    // ShapeのOverride
    bool hasAABB() const override
    {
        AL_ASSERT_DEBUG("not impl");
        return false;
    }
    AABB aabb() const override
    {
        AL_ASSERT_DEBUG("not impl");
        return AABB();
    }
    bool intersect(const Ray& ray, Intersect* isect) const override
    {
        AL_ASSERT_DEBUG("not impl");
        return false;
    }
    bool intersectCheck(const Ray& ray) const override
    {
        AL_ASSERT_DEBUG("not impl");
        return false;
    }

private:
    void prepareImportSampling();
private:
    Image image_;
    Distribution2D distribution_;
};
