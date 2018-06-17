#pragma once

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
class LTEIntegrator : public Object
{
public:
    virtual ~LTEIntegrator() {}
    virtual bool preRendering(const IsectScene*  scene) = 0;
    virtual bool postRendering() = 0;
    virtual Spectrum radiance(const Ray& ray,
                              const IsectScene* scene,
                              Sampler* sampler) const = 0;
};
