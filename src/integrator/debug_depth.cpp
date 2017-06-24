#include "pch.hpp"
#include "integrator.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class DepthSurfaceIntegrator
    :public SurfaceIntegrator
{
public:
    DepthSurfaceIntegrator(const ObjectProp& objectProp);
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
    Spectrum radiance(const Ray& ray, const SceneGeometory& scene, SamplerPtr sampler) override;
private:
};

REGISTER_OBJECT(SurfaceIntegrator,DepthSurfaceIntegrator);

/*
-------------------------------------------------
-------------------------------------------------
*/
DepthSurfaceIntegrator::DepthSurfaceIntegrator(const ObjectProp& objectProp)
{
    static_cast<void>(objectProp);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum DepthSurfaceIntegrator::radiance(
    const Ray& screenRay,
    const SceneGeometory& scene,
    SamplerPtr sampler)
{
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
}
