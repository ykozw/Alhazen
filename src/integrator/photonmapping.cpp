#include "pch.hpp"
#include "core/rng.hpp"
#include "core/orthonormalbasis.hpp"
#include "core/unittest.hpp"
#include "integrator/integrator.hpp"
#include "sampler/sampler.hpp"
#include "accelerator/kdtree.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class PMIntegrator AL_FINAL : public LTEIntegrator {
 public:
  PMIntegrator() {}
  PMIntegrator(const ObjectProp& objectProp);
  bool preRendering(const SceneGeometory& scene,
                    AllBSDFList& bsdfList) override;
  bool postRendering() override { return true; }
  Spectrum radiance(const Ray& ray, const SceneGeometory& scene,
                    SamplerPtr sampler) override;

 private:
  int32_t numPhoton_ = 0;
  // フォトン
  struct Photon {
    Vec3 pos;
    Vec3 wiWorld;
    Spectrum spectrum;
    //
    Vec3 position() const { return pos; }
  };
  KdTree<Photon> photonMap_;
  std::vector<const Photon*> photons_;
  std::vector<Photon> photonsSrc_;
};
REGISTER_OBJECT(LTEIntegrator, PMIntegrator);

/*
-------------------------------------------------
-------------------------------------------------
*/
PMIntegrator::PMIntegrator(const ObjectProp& objectProp) {
  numPhoton_ = 1024 * 16;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool PMIntegrator::preRendering(const SceneGeometory& scene,
                                AllBSDFList& bsdfList) {
  //
  const auto& lights = scene.lights();
  // 光源がなければ終了
  if (lights.empty()) {
    return false;
  }

  //
  // SamplerPtr sampler = std::make_shared<SamplerHalton>();
  SamplerPtr sampler = std::make_shared<SamplerIndepent>();
  sampler->setHash(1);

  // フォトントレーシング
  for (int32_t pn = 0; pn < numPhoton_; ++pn) {
    sampler->startSample(pn);
    // 光源の選択
    // const uint32_t lightIndex = sampler->getSize(uint32_t(lights.size()));
    // const LightPtr& light = lights[lightIndex];
    // 光源からのサンプル
    Vec3 samplePos = Vec3(0.0f, 0.0f, 0.0f);
    Spectrum emission;
    // float pdf = 0.0f;
    OrthonormalBasis<> lightLocalCoord;
    // TODO: emission方向によらない光源を仮定しているのを直す
    // light->sample(sampler, &samplePos, &emission, &pdf, &lightLocalCoord);
    // light->sampleLe(sampler,)
    // 射出方向の選択
    const Vec3 launchDirLocal = sampler->getHemisphere();
    const Vec3 launchDirWorld = lightLocalCoord.local2world(launchDirLocal);
    //
    Ray ray(samplePos, launchDirWorld);
    for (int32_t pathNo = 0; pathNo < 8; ++pathNo) {
      //
      Intersect isect;
      const bool skipLight = true;
      if (!scene.intersect(ray, skipLight, &isect)) {
        continue;
      }
      // フォトンの格納
      // HACK: 全てDiffuseかつ反射率が0.5と仮定している
      if (sampler->get1d() <= 0.5f) {
        Photon newPhoton;
        newPhoton.pos = isect.position;
        newPhoton.wiWorld = -ray.d;
        newPhoton.spectrum = emission;
        //
        photonsSrc_.push_back(newPhoton);
        break;
      }
      // 続けて反射する場合
      OrthonormalBasis<> lc(isect.normal);
      // const Vec3 wi = lc.world2local(ray.d);
      // HACK:
      // 適当に反射方向を決めてしまっている。Diffuseしか存在しないとしている
      const Vec3 wo = sampler->getHemisphere();
#if 0
            const Spectrum reflectance = isect.bsdf->bsdf(wo, wi, Vec2(0.0f));
            (void)reflectance;
#endif
      const Vec3 woWolrd = lc.local2world(wo);
      ray = Ray(isect.uppserSideOrigin(), woWolrd);
    }
  }
  //
  photonMap_.construct(photonsSrc_.data(), int32_t(photonsSrc_.size()));

  // const size_t nn = photonMap_.numNode();

  return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum PMIntegrator::radiance(const Ray& screenRay,
                                const SceneGeometory& scene,
                                SamplerPtr sampler) {
  // 適当に近いところにあるフォトンの数をそのまま輝度にする
  Intersect isect;
  const bool skipLight = true;
  if (!scene.intersect(screenRay, skipLight, &isect)) {
    return Spectrum(0.0f);
  }
  const int32_t numSerch = 32;
  photonMap_.findKNN(isect.position, numSerch, photons_);
  //
#if 0
    int32_t okayCount = 0;
    for (auto& photon : photons_)
    {
        const float dist = Vec3::distance(photon->position(), isect.position);
        okayCount += (dist < 0.1f) ? 1 : 0;
    }
    const float radiance = float(okayCount) / float(numSerch);
    return Spectrum(radiance);
#endif

  //なんかおかしいので直す
  //
  const OrthonormalBasis<> lc(isect.normal);
  const Vec3 woLocal = lc.world2local(screenRay.d);
  Spectrum spectrum(0.0f);
  for (const auto& photon : photons_) {
    const Vec3 wiLocal = lc.world2local(photon->wiWorld);
    const Spectrum reflectance = isect.bsdf->bsdf(woLocal, wiLocal);
    spectrum += photon->spectrum * reflectance;
  }
  spectrum /= float(photons_.size());

  return spectrum;
}
