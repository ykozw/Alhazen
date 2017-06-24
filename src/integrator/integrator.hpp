#pragma once

#include "pch.hpp"
#include "core/spectrum.hpp"
#include "core/ray.hpp"
#include "core/ray.hpp"
#include "core/scenegeometory.hpp"
#include "sampler/sampler.hpp"

class SurfaceIntegrator;
typedef std::shared_ptr<SurfaceIntegrator> SurfaceIntegratorPtr;

/*
-------------------------------------------------
-------------------------------------------------
*/
class SurfaceIntegrator
	:public Object
{
public:
    virtual ~SurfaceIntegrator(){}
    virtual SurfaceIntegratorPtr clone() = 0;
    virtual bool preRendering(
        const SceneGeometory& scene,
        AllBSDFList& bsdfList ) = 0;
    virtual bool postRendering() = 0;
    virtual Spectrum radiance(
        const Ray& ray, 
        const SceneGeometory& scene,
        SamplerPtr sampler ) = 0;
};
