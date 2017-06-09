#include "pch.hpp"
#include "integrator.hpp"

//-------------------------------------------------
// MaterialIntegrator
//-------------------------------------------------
class MaterialIntegrator
    :public SurfaceIntegrator
{
public:
    MaterialIntegrator(const ObjectProp& objectProp);
    SurfaceIntegratorPtr clone() override
    {
        // TODO: 実装
        AL_ASSERT_DEBUG(false);
        return nullptr;
    }
    bool preRendering(const SceneGeometory& scene, AllBSDFList& bsdfList) override
    {
        return true;
    }
    bool postRendering() override
    {
        return true;
    }
    Spectrum radiance(
        const Ray& ray,
        const SceneGeometory& scene,
        SamplerPtr sampler) override;
private:
};

REGISTER_OBJECT(SurfaceIntegrator,MaterialIntegrator);

//-------------------------------------------------
// DepthSurfaceIntegrator()
//-------------------------------------------------
MaterialIntegrator::MaterialIntegrator(const ObjectProp& objectProp)
{
    static_cast<void>(objectProp);
}

//-------------------------------------------------
// hashFunc()
// HACK: 間に合わせ。
//-------------------------------------------------
static uint32_t hashFunc(const void* buf, size_t buflength)
{
    const uint8_t* buffer = (const uint8_t*)buf;

    uint32_t s1 = 1;
    uint32_t s2 = 0;

    for (size_t n = 0; n < buflength; n++)
    {
        s1 = (s1 + buffer[n]) % 65521;
        s2 = (s2 + s1) % 65521;
    }
    return (s2 << 16) | s1;
}

//-------------------------------------------------
// radiance()
//-------------------------------------------------
Spectrum MaterialIntegrator::radiance(
    const Ray& screenRay,
    const SceneGeometory& scene,
    SamplerPtr sampler)
{
    // 何もない場合は0を返す
    Intersect isect;
    const bool skipLight = true;
    if (!scene.intersect(screenRay, skipLight, &isect))
    {
        return Spectrum(0.0f);
    }
    // ポインタからハッシュ値を計算する
    const size_t ptr = (size_t)isect.bsdf.get();
    const uint32_t color = hashFunc(&ptr, 4);
    const uint8_t r = color & 0xFF;
    const uint8_t g = (color >> 8) & 0xFF;
    const uint8_t b = (color >> 16) & 0xFF;
    const float fr = (float)r / 255.0f;
    const float fg = (float)g / 255.0f;
    const float fb = (float)b / 255.0f;
    return Spectrum::createFromRGB({ { fr, fg, fb } }, false);
}
