#include "pch.hpp"
#include "integrator.hpp"


/*
-------------------------------------------------
-------------------------------------------------
*/
class UVSurfaceIntegrator
    :public SurfaceIntegrator
{
public:
    UVSurfaceIntegrator(const ObjectProp& objectProp);
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
REGISTER_OBJECT(SurfaceIntegrator,UVSurfaceIntegrator);

/*
-------------------------------------------------
-------------------------------------------------
*/
UVSurfaceIntegrator::UVSurfaceIntegrator(const ObjectProp& objectProp)
{
    static_cast<void>(objectProp);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum UVSurfaceIntegrator::radiance(
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
    const Vec2& uv = isect.uv;
    return Spectrum::createFromRGB({ { uv.x, uv.y, 0.0f } }, false);
}
