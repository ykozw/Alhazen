#include "pch.hpp"
#include "core/scene.hpp"
#include "core/object.hpp"
#include "core/image.hpp"
#include "core/util.hpp"
#include "sensor/sensor.hpp"
#include "core/stats.hpp"
//
STATS_COUNTER("NumSample", g_numSample, "Samples");

/*
-------------------------------------------------
-------------------------------------------------
*/
Scene::Scene() {}

/*
-------------------------------------------------
-------------------------------------------------
*/
Scene::Scene(const ObjectProp& objectProp)
{
    //
    integrator_ = createObject<LTEIntegrator>(objectProp);
    // センサーの作成
    sensor_ = createObject<Sensor>(objectProp);
    // トーンマッパーの作成
    tonemapper_ = createObject<Tonemapper>(objectProp);
    // トーンマッピング先のイメージの作成
    const Image& img = sensor_->film()->image();
    tonemmappedImage_.resize(img.width(), img.height());
    // デノイザーの作成
    denoiser_ = createObject<Denoiser>(objectProp);
    denoiseBuffer_.resize(img.width(), img.height());
    // サンプラーの作成
    sampler_ = createObject<Sampler>(objectProp);
    // その他プロパティ
    timeout_ = objectProp.findChildBy("name", "timeout").asInt(60);
    isProgressive_ =
      objectProp.findChildBy("name", "progressive").asBool(false);
    snapshotInterval_ =
      objectProp.findChildBy("name", "snapshotinterval").asInt(10);
    snapshotDenoise_ =
      objectProp.findChildBy("name", "snapshotdenoise").asBool(false);
    sppPerInterval_ = objectProp.findChildBy("name", "sppinterval").asInt(4);

    // BSDFの構築
    AllBSDFList bsdfs;
    for (const ObjectProp& child : objectProp.childProps()) {
        if (child.tag() == "BSDF") {
            bsdfs.add(child);
        }
    }
    // SceneGeometoryの構築
    for (const ObjectProp& child : objectProp.childProps()) {
        if (child.tag() == "Shape") {
            // Shapeのロード
            ShapePtr newShape = createObject<Shape>(child);
            // BSDFの設定
            newShape->setBSDF(bsdfs);
            // 追加
            geometory_.addShape(newShape);
        }
    }

    // TODO: Lightの追加
    for (const ObjectProp& child : objectProp.childProps()) {
        if (child.tag() != "Light") {
            continue;
        }
        // const auto& type = child.attribute("type");
        LightPtr newLight = createObject<Light>(child);
        geometory_.addLight(newLight);
    }
    //
    const AABB aabb = geometory_.aabb();
    const Vec3 mn = aabb.min();
    const Vec3 mx = aabb.max();
    logging("Scene Bounding (%2.2f,%2.2f,%2.2f) (%2.2f,%2.2f,%2.2f)",
            mn.x(),
            mn.y(),
            mn.z(),
            mx.x(),
            mx.y(),
            mx.z());

    // タスク数を出しておく
    // HACK: とりあえず一周分だけにしておく
    totalTaskNum_ = sensor_->film()->subFilmNum();
    // Integratorの事前準備
    integrator_->preRendering(geometory_, bsdfs);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Scene::~Scene() {}

/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t
Scene::totalTaskNum() const
{
    if (isProgressive_) {
        return std::numeric_limits<int32_t>::max();
    } else {
        return totalTaskNum_;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t
Scene::taskNumPerLoop() const
{
    return sensor_->film()->subFilmNum();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
uint32_t
Scene::developIntervalInMs() const
{
    return snapshotInterval_ * 1000;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
uint32_t
Scene::timeOutInMs() const
{
    return timeout_ * 1000;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
static uint64_t
calcPixelHash(int32_t x, int32_t y, int32_t width)
{
    return Hash::hash(uint64_t(x + y * width + 1));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
SubFilm&
Scene::render(int32_t taskNo)
{
    // task番号から描画するべきものを決定する
    auto film = sensor_->film();
    const int32_t subFilmNum = film->subFilmNum();
    const int32_t subFilmIndex = taskNo % subFilmNum;
    const int32_t loopNo = taskNo / subFilmNum;
    SubFilm& subFilm = film->subFilm(subFilmIndex);
    const Image& image = film->image();
    Image& subFilmImage = subFilm.image();
    // タイルの描画
    const auto& region = subFilm.region();
    for (int32_t y = region.top; y < region.bottom; ++y) {
        for (int32_t x = region.left; x < region.right; ++x) {
            //
            sampler_->setHash(calcPixelHash(x, y, image.width()));
            //
            Spectrum spectrumTotal(0.0f);
            // TODO: ちゃんとシーンファイルから取ってくるようにする
            const int32_t sppPerLoop = 4;
            const int32_t sampleBegin = (loopNo + 0) * sppPerLoop;
            const int32_t sampleEnd = (loopNo + 1) * sppPerLoop;
            // SubPixel巡回
            for (int32_t sampleNo = sampleBegin; sampleNo < sampleEnd;
                 ++sampleNo) {
                sampler_->startSample(sampleNo);
                ++g_numSample;
                // SubPixelの生成
                const Vec2 subPixelOffset = sampler_->get2d();
                // TODO: フィルターインターフェイスからweightを取ってくる
                const float weight = 1.0f;
                const float spx = x + subPixelOffset.x();
                const float spy = y + subPixelOffset.y();
                const float spWeight = weight;
                //
                float pdf = 0.0f;
                const Ray screenRay = sensor_->generateRay(spx, spy, pdf);
                if (pdf != 0.0f) {
                    Spectrum spectrum = Spectrum::Black;
                    spectrum =
                      integrator_->radiance(screenRay, geometory_, sampler_);
                    AL_ASSERT_DEBUG(!spectrum.hasNaN());
                    // TODO: pdfをちゃんと扱うようにする
                    spectrumTotal += spectrum * spWeight;
                }
            }
            spectrumTotal /= float(sppPerLoop);
            //
            const int32_t subX = x - region.left;
            const int32_t subY = y - region.top;
            subFilmImage.addPixel(subX, subY, spectrumTotal);
            // subFilmImage.addPixel(subX, subY,
            // SpectrumRGB::createFromRGB({1.0f,0.0f,0.0f}, false));
        }
    }
    subFilm.incAddNum();
    // 元のフィルムに転送
    subFilm.transferAddFilm();
    //
    return subFilm;
}

/*
-------------------------------------------------
指定のピクセルのSpectrumを取得するデバッグ用
-------------------------------------------------
*/
Spectrum
Scene::renderPixel(int32_t x, int32_t y)
{
    //
    const Image& image = sensor_->film()->image();
    //
    float pdf = 0.0f;
    const Ray screenRay = sensor_->generateRay(float(x), float(y), pdf);
    Spectrum spectrum = Spectrum::Black;
    sampler_->setHash(calcPixelHash(x, y, image.width()));
    spectrum = integrator_->radiance(screenRay, geometory_, sampler_);
    AL_ASSERT_DEBUG(!spectrum.hasNaN());
    return spectrum;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void
Scene::developLDR(const std::string& filmName, bool isFinal)
{
    // FIXME: renderが走っていないかを確認する

    // 最終スナップは必ずデノイズを掛ける
    const bool denoise = isFinal || snapshotDenoise_;
    const Image& radianceImage = sensor_->film()->image();
    // ノイズを除去する
    if (denoise) {
        //
        AL_ASSERT_DEBUG((radianceImage.width() == denoiseBuffer_.width()) &&
                        (radianceImage.height() == denoiseBuffer_.height()));
        denoiser_->denoise(radianceImage, denoiseBuffer_);
    }
    const Image& image = denoise ? denoiseBuffer_ : radianceImage;
    // Tonemappingを掛けつつ出力する
    tonemapper_->process(image, tonemmappedImage_);
    tonemmappedImage_.writePNG(filmName);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void
Scene::dumpHDR(const std::string& fileName)
{
    // 内部にあるHDRをそのまま出力する
    // const auto& image = sensor_->film()->image();
    const std::string fullPath = g_fileSystem.getOutputFolderPath() + fileName;
    // image.writeHdr(fullPath);
    // image.writeBHDR(fullPath);
}
