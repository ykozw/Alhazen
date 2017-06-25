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
このインスタンスはスレッド毎にshallow copyが生成されます。
Samplerなど、スレッド毎に持っていた方が良いものは、新しく生成されます。
-------------------------------------------------
*/
class Scene
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
    void renderDebug(int32_t px, int32_t py);
    Spectrum renderPixel(int32_t px, int32_t py);
    void developLDR(const std::string& filmName, bool isFinal, bool isPreview);
    void dumpHDR(const std::string& fileName);
private:
    SceneGeometory geometory_;
    SensorPtr sensor_;
    TonemapperPtr tonemapper_;
    DenoiserPtr denoiser_;
    SurfaceIntegratorPtr integrator_;
    int32_t totalTaskNum_;
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
    // denoise用の一時バッファ
    Image denoiseBuffer_;
};
typedef std::shared_ptr<Scene> ScenePtr;
