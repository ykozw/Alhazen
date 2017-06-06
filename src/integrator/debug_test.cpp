#include "pch.hpp"
#include "integrator.hpp"
#include "core/orthonormalbasis.hpp"

//-------------------------------------------------
// TestIntegrator
//-------------------------------------------------
class TestIntegrator
    :public SurfaceIntegrator
{
public:
    TestIntegrator() = default;
    TestIntegrator(const ObjectProp& objectProp);
    SurfaceIntegratorPtr clone() override
    {
        TestIntegrator* ptr = new TestIntegrator();
        ptr->rng_ = rng_;
        return SurfaceIntegratorPtr(ptr);
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
    XorShift128 rng_;
};

REGISTER_OBJECT(SurfaceIntegrator,TestIntegrator);

//-------------------------------------------------
// TestIntegrator()
//-------------------------------------------------
TestIntegrator::TestIntegrator(const ObjectProp& objectProp)
{
    static_cast<void>(objectProp);
}

//-------------------------------------------------
// radiance()
//-------------------------------------------------
Spectrum TestIntegrator::radiance(
    const Ray& screenRay,
    const SceneGeometory& scene,
    SamplerPtr sampler)
{
#if 1
    return Spectrum(0.1f);
#else
    //
    Intersect isect;
    const IntersectMode isectMode = IntersectMode::NO_LIGHTS;
    scene.intersect(screenRay, isectMode, &isect);
    if (!isect.isHit)
    {
        //
        return Spectrum::createAsBlack();
    }
    else
    {
        Vec3 localWo;
        Vec3 localWi;
        Vec3 worldWi;
        float pdf;
        const OrthonormalBasis local(isect.normal);
        local.world2local(-screenRay.d, &localWo);
        isect.bsdf->bsdfSample(localWo, isect.uv, rng_, &localWi, &pdf);
        local.local2world(localWi, &worldWi);
        const Spectrum envEmit = scene.lights()[0]->emittionDir(worldWi);
        return envEmit;
    }
#endif
}
