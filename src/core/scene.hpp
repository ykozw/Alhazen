#pragma once

#include "pch.hpp"
#include "core/object.hpp"
#include "core/image.hpp"
#include "core/math.hpp"
#include "core/scenegeometory.hpp"
#include "shape/shape.hpp"
#include "bsdf/bsdf.hpp"
#include "sensor/sensor.hpp"
#include "integrator/integrator.hpp"
#include "sampler/sampler.hpp"
#include "tonemapper/tonemapper.hpp"
#include "denoiser/denoiser.hpp"

/*
-------------------------------------------------
Scene
render()などは複数のスレッドから同時に呼ばれることに注意
-------------------------------------------------
*/
class Scene AL_FINAL
{
public:
    Scene();
    Scene(const ObjectProp& objectProp);
    ~Scene();
    int32_t totalTaskNum() const;
    int32_t taskNumPerLoop() const;
    uint32_t developIntervalInMs() const;
    uint32_t timeOutInMs() const;
    SubFilm& render(int32_t taskNo);
    Spectrum renderPixel(int32_t px, int32_t py);
    void developLDR(const std::string& filmName, bool isFinal);
    void dumpHDR(const std::string& fileName);

private:
    SceneGeometory geometory_;
    SensorPtr sensor_ = nullptr;
    TonemapperPtr tonemapper_ = nullptr;
    DenoiserPtr denoiser_ = nullptr;
    LTEIntegratorPtr integrator_ = nullptr;
    //SamplerPtr sampler_ = nullptr; // false sharingが起きているのでなんとかする
    int32_t totalTaskNum_ = 0;
    // トーンマッピングされた画像
    ImageLDR tonemmappedImage_;
    // レンダーのタイムアウト(秒)
    int32_t timeout_;
    // プログレッシブ実行するか
    bool isProgressive_;
    // スナップショットの間隔(秒)
    int32_t snapshotInterval_;
    // スナップショット時にデノイズを掛けるか
    bool snapshotDenoise_;
    // インターバル毎のspp
    int32_t sppPerInterval_ = 0;
    // denoise用の一時バッファ
    Image denoiseBuffer_;
};
typedef std::shared_ptr<Scene> ScenePtr;
