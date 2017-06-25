#include "pch.hpp"
#include "integrator.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class DebugSurfaceIntegrator
    :public SurfaceIntegrator
{
public:
    DebugSurfaceIntegrator(const ObjectProp& objectProp);
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
REGISTER_OBJECT(SurfaceIntegrator, DebugSurfaceIntegrator);

/*
-------------------------------------------------
-------------------------------------------------
*/
DebugSurfaceIntegrator::DebugSurfaceIntegrator(const ObjectProp& objectProp)
{
    static_cast<void>(objectProp);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum DebugSurfaceIntegrator::radiance(
    const Ray& screenRay,
    const SceneGeometory& scene,
    SamplerPtr sampler)
{
#if 0 // UVデバッグ
    // 何もない場合は0を返す
    Intersect isect;
    const bool skipLight = true;
    if (!scene.intersect(screenRay, skipLight, &isect))
    {
        return Spectrum(0.0f);
    }
    const Vec2& uv = isect.uv;
    return Spectrum::createFromRGB({ { uv.x, uv.y, 0.0f } }, false);
#elif 0 // 深度デバッグ
    // 何もない場合は0を返す
    Intersect isect;
    bool skipLight = false;
    if (!scene.intersect(screenRay, skipLight, &isect))
    {
        return Spectrum(0.0f);
    }
    float c = isect.t / 10.0f;
    c = alClamp(c, 0.0f, 1.0f);
    return Spectrum::createFromRGB({ { c, c, c } }, false);
#elif 0 // Normalデバッグ
    Intersect isect;
    // 何もない場合は0を返す
    const bool skipLight = true;
    if (!scene.intersect(screenRay, skipLight, &isect))
    {
        return Spectrum(0.0f);
    }
    Vec3 n = isect.normal;
    Vec3 v = Vec3::mul((n.normalized() + Vec3(1.0f)), Vec3(0.5f));
    return Spectrum::createFromRGB({ { v.x(), v.y(), v.z() } }, false);
#elif 0 // Materialデバッグ
    /*
    -------------------------------------------------
    hashFunc()
    HACK: 間に合わせ。
    -------------------------------------------------
    */
    const auto hashFunc = [](const void* buf, size_t buflength) -> uint32_t
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
    };
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

#else
    return Spectrum::createFromRGB({ 0.0f,0.0f,0.0f }, false);
#endif
}
