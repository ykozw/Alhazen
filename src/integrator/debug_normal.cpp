#include "pch.hpp"
#include "integrator.hpp"

//-------------------------------------------------
// NormalSurfaceIntegrator
//-------------------------------------------------
class NormalSurfaceIntegrator
    :public SurfaceIntegrator
{
public:
    NormalSurfaceIntegrator() = default;
    NormalSurfaceIntegrator(const ObjectProp& objectProp);
    SurfaceIntegratorPtr clone() override
    {
        NormalSurfaceIntegrator* newIntegrator = new NormalSurfaceIntegrator;
        return SurfaceIntegratorPtr(newIntegrator);
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

REGISTER_OBJECT(SurfaceIntegrator,NormalSurfaceIntegrator);

//-------------------------------------------------
// NormalSurfaceIntegrator()
//-------------------------------------------------
NormalSurfaceIntegrator::NormalSurfaceIntegrator(const ObjectProp& objectProp)
{
    static_cast<void>(objectProp);
}

//-------------------------------------------------
// radiance()
//-------------------------------------------------
Spectrum NormalSurfaceIntegrator::radiance(
    const Ray& screenRay,
    const SceneGeometory& scene,
    SamplerPtr sampler)
{
    Intersect isect;
    // 何もない場合は0を返す
    const bool skipLight = true;
    if (!scene.intersect(screenRay, skipLight, &isect))
    {
        return Spectrum(0.0f);
    }
    Vec3 n = isect.normal;
    Vec3 v = Vec3::mul((n.normalized() + Vec3(1.0f)), Vec3(0.5f));
    return Spectrum::createFromRGB({ { v.x, v.y, v.z } }, false);
}
