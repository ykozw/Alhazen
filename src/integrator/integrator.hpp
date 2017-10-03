#pragma once

#include "pch.hpp"
#include "core/spectrum.hpp"
#include "core/ray.hpp"
#include "core/ray.hpp"
#include "core/scenegeometory.hpp"
#include "sampler/sampler.hpp"

class LTEIntegrator;
typedef std::shared_ptr<LTEIntegrator> LTEIntegratorPtr;

/*
-------------------------------------------------
レンダリングアルゴリズムはこれを継承して実装される
radiance()は複数のスレッドから呼ばれても問題のない作りを期待する
-------------------------------------------------
*/
class LTEIntegrator
	:public Object
{
public:
    virtual ~LTEIntegrator(){}
    virtual bool preRendering(
        const SceneGeometory& scene,
        AllBSDFList& bsdfList ) = 0;
    virtual bool postRendering() = 0;
    virtual Spectrum radiance(
        const Ray& ray, 
        const SceneGeometory& scene,
        SamplerPtr sampler ) = 0;
};
