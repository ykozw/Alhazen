/*
TODO list
- このファイルの内容の大半をlensフォルダに移動させる
*/

#include "pch.hpp"
#include "core/transform.hpp"
#include "core/object.hpp"
#include "core/util.hpp"
#include "core/intersect.hpp"
#include "core/orthonormalbasis.hpp"
#include "core/bounding.hpp"
#include "sensor/sensor.hpp"
#include "sensor/sensorutil.hpp"
#include "sampler/sampler.hpp"

/*
-------------------------------------------------
refract()
Snellの法則を使って透過方向を計算する
全反射が発生してしまう場合はfalseが戻る
-------------------------------------------------
*/
inline bool refract(Vec3 wi, Vec3 n, float eta, Vec3 *wt)
{
    // Snellの法則
    const float ctIn = Vec3::dot(n, wi);
    const float stIn2 = alMax(0.0f, 1.0f - ctIn * ctIn);
    const float stTr2 = eta * eta * stIn2;
    // 完全反射をしていないかチェック
    if (stTr2 >= 1.0f)
    {
        return false;
    }
    //
    const float cosThetaT = sqrtf(1.0f - stTr2);
    *wt = eta * -wi + (eta * ctIn - cosThetaT) * n;
    return true;
}

/*
-------------------------------------------------
RealIOR
硝材と波長からIORを計算する
異常分散の硝材は取り扱わない
-------------------------------------------------
*/
class RealIOR AL_FINAL
{
public:
    RealIOR();
    RealIOR(const std::string &glassName);
    float ior(const float waveLength = WAVE_LENGTH_D) const;

private:
    void init(float abbe, float nd);

private:
    std::string glassName_;
    float a_;
    float b_;
};

/*
-------------------------------------------------
RealIOR()
空気として初期化する
-------------------------------------------------
*/
RealIOR::RealIOR()
{
    const float nd = 1.0f;
    const float vd = std::numeric_limits<float>::max();
    init(nd, vd);
}

/*
-------------------------------------------------
RealIOR()
硝材を指定して初期化する
-------------------------------------------------
*/
RealIOR::RealIOR(const std::string &glassName) : glassName_(glassName)
{
    float nd, vd;
    getGlassSpec(glassName, nd, vd);
    init(nd, vd);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float RealIOR::ior(const float waveLength) const
{
    const float iw = 1.0f / waveLength;
    const float iw2 = iw * iw;
    const float ior = a_ + iw2 * b_;
    return ior;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void RealIOR::init(float nd, float vd)
{
    const float tmp = (nd - 1.0f) / vd;
    a_ = nd + 1.5168f * tmp;
    b_ = 0.523655f * tmp;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
struct LensSurface AL_FINAL
{
public:
    // ストップ面(絞り面)か
    bool isStop;
    // 開き
    float apeX;
    float apeY;
    // 球面半径
    float invRadius;
    float radius;
    float radius2;
    // 面間距離
    float distance;
    // IOR計算
    RealIOR realIor;
    // 撮像素子からの距離
    float z;

public:
    LensSurface() : isStop(false), radius(1.0f), radius2(1.0f), distance(1.0f)
    {
    }
};
typedef std::vector<LensSurface> LensSet;

/*
-------------------------------------------------
loadOtx()
Optalixファイルをロードする
-------------------------------------------------
*/
bool loadOtx(const std::string &fileName, LensSet &lensSet)
{
    lensSet.clear();
    std::ifstream file;
    file.open(fileName);
    AL_ASSERT_DEBUG(file);
    std::string buffer;
    // int32_t surfNo = 0;
    LensSurface *curLensSurface = NULL;
    //
    while (std::getline(file, buffer))
    {
        char tag[0xff + 1] = {'\0'};
        char value[0xff + 1] = {'\0'};
        //
        if (sscanf(buffer.c_str(), "%255s %255s", tag, value) < 1)
        {
            continue;
        }
        /*
        -------------------------------------------------
        面周り以外
        -------------------------------------------------
        */
        // 無視するタグ一覧
        const char *ignoreTags =
            "!|VERS|FILE|NRD|RAIM|RAIC|PIM|PSFPATH|FNO|WL|WTW|REF|OSPF|"
            "FTYP|NFLD|FLD|MFR|MFRF|MTFAVG|AFR|DEF|KLDR|"
            "PLSC|PLSC2|AF|TRPLANE|TRXLAM|REM";
        if (strstr(ignoreTags, tag) != NULL)
        {
            continue;
        }
        /*
        -------------------------------------------------
        ゴースト周り
        -------------------------------------------------
        */
        else if (strcmp(tag, "GHO") == 0)
        {
            // とりあえず現状全て無視
            continue;
        }
        /*
        -------------------------------------------------
        面周り
        -------------------------------------------------
        */
        // 面の交代
        else if (strcmp(tag, "SUR") == 0)
        {
            lensSet.push_back(LensSurface());
            curLensSurface = &lensSet[lensSet.size() - 1];
        }
        // ストップ面の場合
        else if (strcmp(tag, "STO") == 0)
        {
            curLensSurface->isStop = true;
        }
        // 半径(の逆数)
        else if (strcmp(tag, "CUY") == 0)
        {
            const float invRadius = static_cast<float>(atof(value));
            curLensSurface->invRadius = invRadius;
            const float inf = std::numeric_limits<float>::infinity();
            const float radius = (invRadius == 0.0f) ? inf : 1.0f / invRadius;
            curLensSurface->radius = radius;
            curLensSurface->radius2 = radius * radius;
        }
        // 硝材
        else if (strcmp(tag, "GLA") == 0)
        {
            curLensSurface->realIor = RealIOR(value);
        }
        // 曲面タイプ
        else if (strcmp(tag, "SUT") == 0)
        {
            // "S"(球面)以外の場合は受け付けない
            AL_ASSERT_DEBUG(strcmp(value, "S") == 0);
        }
        // 面間距離
        else if (strcmp(tag, "THI") == 0)
        {
            curLensSurface->distance = static_cast<float>(atof(value));
        }
        // 口径
        else if (strcmp(tag, "APE") == 0)
        {
            int32_t numApe;
            float apeX;
            float apeY;
            std::array<float, 3> apes;
            std::array<int32_t, 3> dummys;
            if (sscanf(buffer.c_str(),
                       "  APE %d %f %f %f %f %f %d %d %d",
                       &numApe,
                       &apeX,
                       &apeY,
                       &apes[0],
                       &apes[1],
                       &apes[2],
                       &dummys[0],
                       &dummys[1],
                       &dummys[2]) != 9)
            {
                AL_ASSERT_DEBUG(false);
            }
            AL_ASSERT_DEBUG(numApe == 1);
            curLensSurface->apeX = apeX;
            curLensSurface->apeY = apeY;
        }
        //
        else if (strcmp(tag, "FH") == 0)
        {
        }
        else
        {
            AL_ASSERT_DEBUG(false);
        }
    }
    // 物体側の最初のはレンズではないので削除
    lensSet.erase(lensSet.begin());

    // 最終面(像面側)のレンズは曲面でないことを確認
    AL_ASSERT_DEBUG(lensSet.back().invRadius == 0.0f);

    // 最終面も特に使わないので削除
    lensSet.erase(lensSet.begin() + lensSet.size() - 1);

    // 後ろ玉に相当するものに無意味な面が一枚付いている場合がある。その場合も削除
    while (lensSet.back().invRadius == 0.0f)
    {
        // さらに一つ奥のdistanceを調整しておく
        lensSet[lensSet.size() - 2].distance += lensSet.back().distance;
        // 削除
        lensSet.erase(lensSet.begin() + lensSet.size() - 1);
    }

    // TODO:
    // 曲率が強すぎて、撮像素子から見えない点があるような状態がないかのチェック

    // 一番後ろの撮像素子のzが0になるようにして座標系を設定
    // 同時にレンズの硝材データのインデックスを一つずらす(撮像素子側からトレースを行うため)
    float z = 0.0f;
    for (int32_t i = static_cast<int32_t>(lensSet.size()) - 1; i >= 0; --i)
    {
        auto &lens = lensSet[i];
        z += lens.distance;
        lens.z = z;
        // IORデータをずらす
        if (i > 0)
        {
            auto &lensNext = lensSet[i - 1];
            lens.realIor = lensNext.realIor;
        }
        // 前玉前面は常に空気
        else
        {
            lens.realIor = RealIOR();
        }
    }

    //
    return true;
}

/*
-------------------------------------------------
RealSensor
Optalixファイルを使用する
-------------------------------------------------
*/
class RealSensor AL_FINAL : public Sensor
{
public:
    RealSensor();
    RealSensor(const ObjectProp &objectProp);
    Ray generateRay(float imageX, float imageY, float &pdf) const override;
    FilmPtr film() const override;

private:
    void calcExitPupile();
    void calcPrecomputeTable();
    void dumpLensSpecImage() const;
    bool lensTrace(float xOnFilmInMM,
                   float yOnFilmInMM,
                   Vec3 dir,
                   float waveLength,
                   float imageSensorOffset,
                   std::vector<Vec3> *points,
                   Ray *outRay) const;
    float workingDistance(float imageSensorOffset) const;
    void autoFocus(float distance);
    float sensorOffsetRangeUpperBound() const;
    float lensPower() const;

private:
    FilmPtr film_;
    float screenWidth_;
    float screenHeight_;
    float screenAspect_;
    float invScreenWidth_;
    float invScreenHeight_;
    Vec3 perspectivePos_;
    Vec3 rightDir_;
    Vec3 upDir_;
    Vec3 forwardDir_;
    float perspectiveFovy_;
    OrthonormalBasis<> lcoal_;
    //
    std::vector<LensSurface> surfs_;
    // SamplerIndepent lensSampler_;
    //
    std::vector<AABB2D> exitPupils_;
    float scale_;
    //
    float imageSensorOffset_;
    //
    float filmWidthInMM_;
    float filmHeightInMM_;
    //
    mutable int32_t sampleNo_ = 0;
};
REGISTER_OBJECT(Sensor, RealSensor);

/*
-------------------------------------------------
RealSensor
-------------------------------------------------
*/
RealSensor::RealSensor() {}

/*
-------------------------------------------------
RealSensor
-------------------------------------------------
*/
RealSensor::RealSensor(const ObjectProp &objectProp)
{
    //
    const Transform transform(objectProp.findChildByTag("transform"));
    perspectivePos_ = transform.cameraOrigin();
    upDir_ = transform.cameraUp();
    forwardDir_ = transform.cameraDir();
    rightDir_ = Vec3::cross(forwardDir_, upDir_);
    upDir_ = Vec3::cross(rightDir_, forwardDir_);
    rightDir_.normalize();
    upDir_.normalize();
    forwardDir_.normalize();

    //
    lcoal_.set(forwardDir_, upDir_);
    // TODO: kc
    // const auto& focalLengthProp = objectProp.findChildByTag("focalLength");
#if 0
    const float focalLength = focalLengthProp.findChildBy("name", "fov").asFloat(50.0f * 0.0001f); // [0,1]
#endif
    perspectiveFovy_ = DEG2RAD(objectProp.findChildBy("name", "fov")
                                   .asFloat(90.0f)); // TODO: デフォルト値の調査
    scale_ = objectProp.findChildBy("name", "scale").asFloat(0.001f);
    AL_ASSERT_DEBUG(scale_ != 0.0f);
    //
    film_ = std::make_shared<Film>(objectProp.findChildByTag("film"));
    const auto &image = film_->image();
    screenWidth_ = static_cast<float>(image.width());
    screenHeight_ = static_cast<float>(image.height());
    filmWidthInMM_ = film_->filmWidthInMM();
    filmHeightInMM_ = film_->filmHeightInMM();
    // HACK: 現状でイメージセンサーと同じアスペく比だけに対応する
    AL_ASSERT_DEBUG(screenWidth_ * 2 == screenHeight_ * 3);
    screenAspect_ = screenWidth_ / screenHeight_;
    invScreenWidth_ = 1.0f / screenWidth_;
    invScreenHeight_ = 1.0f / screenHeight_;
    const float tf = tan(perspectiveFovy_ * 0.5f);
    const float tfAspect = tf * screenAspect_;
    rightDir_ *= tfAspect;
    upDir_ *= tf;
    // レンズの作成
    const std::string lensFileName =
        objectProp.findChildBy("name", "otxfile").asString("test.otx");
    loadOtx(lensFileName, surfs_);
    // オートフォーカス
    const float aimWorkingDistance =
        objectProp.findChildBy("name", "workingDistance").asFloat(1000.0f);
    autoFocus(aimWorkingDistance);
    // 入射瞳サイズの計算
    calcExitPupile();
    // 事前計算テーブルの作成
    calcPrecomputeTable();
    // レンズスペックのダンプ
    dumpLensSpecImage();
}

float fract(float v) { return v - floor(v); }
//
Vec3 HSV2RGB(const Vec3 &hsv)
{
    const float h = fract(hsv.x());
    const float s = hsv.y();
    const float v = hsv.z();
    const float hueF = h * 6.0f;
    const int hueI = static_cast<int>(hueF);
    const float fr = hueF - hueI;
    const float m = v * (1.0f - s);
    const float n = v * (1.0f - s * fr);
    const float p = v * (1.0f - s * (1.0f - fr));
    //
    float r, g, b;
    switch (hueI)
    {
    case 0:
        r = v;
        g = p;
        b = m;
        break;
    case 1:
        r = n;
        g = v;
        b = m;
        break;
    case 2:
        r = m;
        g = v;
        b = p;
        break;
    case 3:
        r = m;
        g = n;
        b = v;
        break;
    case 4:
        r = p;
        g = m;
        b = v;
        break;
    default:
        r = v;
        g = m;
        b = n;
        break;
    }
    return Vec3(r, g, b);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void RealSensor::calcExitPupile()
{
    //
    const auto &img = film_->image();
    // 全ピクセル上の入射瞳のサイズを事前計算しておく
    const float backLensDist = surfs_.back().distance;
    const float backLensApeX = surfs_.back().apeX;
    const float backLensApeY = surfs_.back().apeY;
    logging("Start calc exit pupil...");
#if 0
    for (int32_t py = 0, h = img.height(); py < h; py += pixelSkip)
    {
        for (int32_t px = 0, w = img.width(); px < w; px += pixelSkip)
        {
            const float xOnFilm = ((float)px / (float)w - 0.5f) * filmWidthInMM_;
            const float yOnFilm = ((float)py / (float)h - 0.5f) * filmHeightInMM_;
            //
            AABB2D exitPupil;
            int32_t lastXL = 0;
            int32_t lastXR = gridSize;
            for (int32_t gy = 0; gy < gridSize; ++gy)
            {
                // TODO: まずは通過する場所を探索する
                lastXL

                for (int32_t gx = 0; gx < gridSize; ++gx)
                {
                }
            }
        }
    }
#else // 力技
    const int32_t sampleNumRowColumn = 16 + 1;
    // const int32_t gridSize = 64;
    const int32_t gridSize = 128;
    std::vector<AABB2D> exitPupilsCoarse;
    const int32_t pixelSkipX = img.width() / (sampleNumRowColumn - 1);
    const int32_t pixelSkipY = img.height() / (sampleNumRowColumn - 1);
    AL_ASSERT_DEBUG(img.width() % (sampleNumRowColumn - 1) == 0);
    AL_ASSERT_DEBUG(img.height() % (sampleNumRowColumn - 1) == 0);
    exitPupilsCoarse.resize(sampleNumRowColumn * sampleNumRowColumn);
    // 左上1/4だけを見ればよい
    for (int32_t py = 0, h = img.height(); py <= h / 2; py += pixelSkipY)
    {
        for (int32_t px = 0, w = img.width(); px <= w / 2; px += pixelSkipX)
        {
            const float xOnFilmInMM =
                ((float)px / (float)(w - 1) - 0.5f) * filmWidthInMM_;
            const float yOnFilmInMM =
                ((float)py / (float)(h - 1) - 0.5f) * filmHeightInMM_;
            AABB2D exitPupil;
            for (int32_t gy = 0; gy < gridSize; ++gy)
            {
#pragma omp parallel for schedule(dynamic, 1)
                for (int32_t gx = 0; gx < gridSize; ++gx)
                {
                    // レンズトレースがうまくいく場合は入射瞳を拡張する
                    const float blx =
                        backLensApeX *
                        (2.0f * (float)gx / (float)(gridSize - 1) - 1.0f);
                    const float bly =
                        backLensApeY *
                        (2.0f * (float)gy / (float)(gridSize - 1) - 1.0f);
                    const float blz = backLensDist - imageSensorOffset_;
                    Vec3 rayDir(blx - xOnFilmInMM, bly - yOnFilmInMM, blz);
                    rayDir.normalize();
                    Ray outRay;
                    // D線のみで入射瞳は計算される
                    if (!lensTrace(xOnFilmInMM,
                                   yOnFilmInMM,
                                   rayDir,
                                   WAVE_LENGTH_D,
                                   imageSensorOffset_,
                                   NULL,
                                   &outRay))
                    {
                        continue;
                    }
#pragma omp critical
                    {
                        exitPupil.addPoint(Vec2(blx, bly));
                    }
                }
            }
            //
            const int32_t cw = sampleNumRowColumn;
            const int32_t ch = sampleNumRowColumn;
            const int32_t cw1 = cw - 1;
            const int32_t ch1 = ch - 1;
            const int32_t cx = px / pixelSkipX;
            const int32_t cy = py / pixelSkipY;
            //
            const Vec2 mn = exitPupil.min();
            const Vec2 mx = exitPupil.max();
            // そのまま使える
            exitPupilsCoarse[cx + (cy * cw)] = exitPupil;
            // X反転
            exitPupilsCoarse[(cw1 - cx) + (cy * cw)] =
                AABB2D(-mx.x(), mn.y(), -mn.x(), mx.y());
            // Y反転
            exitPupilsCoarse[cx + ((ch1 - cy) * cw)] =
                AABB2D(mn.x(), -mx.y(), mx.x(), -mn.y());
            // XY反転
            exitPupilsCoarse[(cw1 - cx) + ((ch1 - cy) * cw)] =
                AABB2D(-mx.x(), -mx.y(), -mn.x(), -mn.y());
        }
    }

    // 詳細なものに変換する。単純にlerp。
    exitPupils_.resize(img.width() * img.height());
    for (int32_t py = 0, h = img.height(); py < h; ++py)
    {
        for (int32_t px = 0, w = img.width(); px < w; ++px)
        {
            auto &dst = exitPupils_[px + py * w];
            const int32_t x0 = px / pixelSkipX;
            const int32_t x1 = alMin(x0 + 1, sampleNumRowColumn - 1);
            const int32_t y0 = py / pixelSkipY;
            const int32_t y1 = alMin(y0 + 1, sampleNumRowColumn - 1);
            const float fx = (float)(px % pixelSkipX) / (float)pixelSkipX;
            const float fy = (float)(py % pixelSkipY) / (float)pixelSkipY;
            // 近い4つをバイリニアサンプリング
            const auto &lu = exitPupilsCoarse[x0 + y0 * sampleNumRowColumn];
            const auto &ru = exitPupilsCoarse[x1 + y0 * sampleNumRowColumn];
            const auto &ld = exitPupilsCoarse[x0 + y1 * sampleNumRowColumn];
            const auto &rd = exitPupilsCoarse[x1 + y1 * sampleNumRowColumn];
            const AABB2D u = AABB2D::lerp(lu, ru, fx);
            const AABB2D d = AABB2D::lerp(ld, rd, fx);
            const AABB2D src = AABB2D::lerp(u, d, fy);
            dst = src;
        }
    }
#endif

    // TODO: 入射瞳の最大を算出
    AABB2D exitPupilMax;
    for (const auto &exitPupil : exitPupils_)
    {
        exitPupilMax.add(exitPupil);
    }

    logging("Done calc exit pupil...");
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void RealSensor::calcPrecomputeTable()
{
    // TODO: まずは要素を洗い出す
    /*
    入力
    - フィルム上の位置(自由度2)
    - 射出方向(自由度2)
    - 波長(自由度1)
    出力
    - レンズ上の位置(自由度2)
    - 出力方向(自由度2)
    */
    //
    //
    const auto &img = film_->image();
    const int32_t width = img.width();
    const int32_t height = img.height();
    // exitPupils_.resize(img.width()*img.height());
    for (int32_t filmX = 0; filmX < width; ++filmX)
    {
        for (int32_t filmY = 0; filmY < height; ++filmY)
        {
            // const AABB2D& aabb = exitPupils_[filmX + filmY*width];
            //
            for (int32_t filmY = 0; filmY < 512; ++filmY)
            {
                for (int32_t filmY = 0; filmY < 512; ++filmY)
                {
                }
            }
        }
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void RealSensor::dumpLensSpecImage() const
{
#if 0
    // 最も高いレンズの高さを出す
    float apeYmax = 0.0f;
    for (const auto& surf : surfs_)
    {
        apeYmax = alMax(surf.apeY, apeYmax);
    }

    // 写す範囲を設定
    const float farZ = (surfs_[0].z + 10.0f);
    const float nearZ = imageSensorOffset_ - 10.0f;
    const float minY = alMax(apeYmax * 1.2f, filmHeightInMM_ * 1.0f);
    const float maxY = -minY;

    //
    SVGWriter svg("lens.svg", -farZ, maxY, -nearZ, minY);
    const Image& img = film_->image();

    // 背景をグレーにしておく
    svg.writeLine("\t\t\t<svg:rect x=\"0\" y=\"0\" width=\"10000\" height=\"10000\" fill=\"gray\" />\n");

    // 方眼紙の描画
    const int32_t squareSiz = 10;
    const int32_t numY = 32;
    for (int32_t yn = 0; yn < numY; ++yn)
    {
        const float y = (float)((yn - numY / 2) * squareSiz);
        svg.beginPath(-farZ, y)
        .setStrokeColor("black")
        .setStrokeWidth(0.1f)
        .addLine(-nearZ, y)
        .endPath();
    }
    const int32_t numX = 256;
    for (int32_t xn = 0; xn < numX; ++xn)
    {
        const float z = xn * squareSiz - 10.0f;;
        svg.beginPath(-z, -100.0f)
        .setStrokeColor("black")
        .setStrokeWidth(0.1f)
        .addLine(-z, 100.0f)
        .endPath();
    }

    // 光軸の描画
    svg.beginPath(-farZ, 0.0f)
    .setStrokeColor("black")
    .setStrokeWidth(0.3f)
    .addLine(-imageSensorOffset_, 0.0f)
    .endPath();

    // 結像面の描画
    svg.beginPath(-imageSensorOffset_, -filmHeightInMM_*0.5f - 5.0f)
    .setStrokeColor("black")
    .setStrokeWidth(0.5f)
    .addLine(-imageSensorOffset_, filmHeightInMM_*0.5f + 5.0f)
    .endPath();

    // レンズの描画
    float prevZ = 0.0f;
    float prevH = 0.0f;
    for (const auto& surf : surfs_)
    {
        // レンズの描画
        if (!surf.isStop)
        {
            const float h = surf.apeY;
            const float r = surf.radius;
            const float d = sqrtf(r*r - h*h);
            const float cz = surf.z - surf.radius;
            const float arcZ = (r < 0.0f) ? cz - d : cz + d;
            const bool isClockWise = (r < 0.0f);
            svg.beginPath(-arcZ, h)
            .setStrokeColor("black")
            .setStrokeWidth(0.4f)
            .addArc(fabsf(r), -arcZ, -h, isClockWise)
            .endPath();

            // 囲みの線を描画
            const bool isAir = (surf.realIor.ior() == 1.0f);
            if (prevH != 0.0f && !isAir)
            {
                svg.beginPath(-prevZ, prevH)
                .setStrokeColor("black")
                .setStrokeWidth(0.4f)
                .addLine(-arcZ, h)
                .endPath();
                svg.beginPath(-prevZ, -prevH)
                .setStrokeColor("black")
                .setStrokeWidth(0.4f)
                .addLine(-arcZ, -h)
                .endPath();
            }
            //
            prevZ = arcZ;
            prevH = h;
        }
        // STOP面の描画
        else
        {
            const float apeY = surf.apeY;
            const float z = surf.z;
            const float scale = 2.0f;
            svg.beginPath(-z, apeY*scale)
            .setStrokeColor("black")
            .setStrokeWidth(0.6f)
            .addLine(-z, apeY)
            .endPath();
            svg.beginPath(-z, -(apeY*scale))
            .setStrokeColor("black")
            .setStrokeWidth(0.6f)
            .addLine(-z, -apeY)
            .endPath();
            //
            prevZ = 0.0f;
            prevH = 0.0f;
        }
    }

    // サジタル面の出力
    const int32_t numOrig = 6;
    const int32_t numRaysPerOrig = 6;
    const int32_t width = img.width();
    const int32_t halfWidth = width / 2;
    const int32_t height = img.height();
    const int32_t heightStep = height / (numOrig - 1);
    for (int32_t on = 0; on < numOrig; ++on)
    {
        //const int32_t index = heightStep * on * width + halfWidth;
        const int32_t yIndex = alMin(heightStep * on, height - 1);
        const int32_t index = yIndex * width + halfWidth;
        const AABB2D& exitPupil = exitPupils_[index];
        // エッジ付近のレイは外れる場合があるので少しだけ内側にする
        const float hackyScale = 0.99f;
        const float mny = exitPupil.min().y * hackyScale;
        const float mxy = exitPupil.max().y * hackyScale;
        const float step = (mxy - mny) / (float)(numRaysPerOrig - 1);
        // レイの色の計算
        const Vec3 rayColor  = HSV2RGB(Vec3((float)on / (float)numOrig, 1.0f, 1.0f));
        const std::string rayColorStr = "rgb(" +
                                        std::to_string((int32_t)(rayColor.x()*255.0f)) + "," +
                                        std::to_string((int32_t)(rayColor.y()*255.0f)) + "," +
                                        std::to_string((int32_t)(rayColor.z()*255.0f)) + ")";
        //
        for (int32_t rn = 0; rn < numRaysPerOrig; ++rn)
        {
            // レイの作成
            const Vec3 imageXYZInMM(0.0f, filmHeightInMM_ * (on / (float)(numOrig - 1) - 0.5f), imageSensorOffset_);
            const float pupilY = alLerp(mny, mxy, (float)rn / (float)(numRaysPerOrig - 1));
            const Vec3 pupilXYZInMM(0.0f, pupilY, surfs_.back().distance);
            const Vec3 rayDir = (pupilXYZInMM - imageXYZInMM).normalize();
            //
            Ray outRay;
            std::vector<Vec3> tracePoints;
            if (!lensTrace(imageXYZInMM.x(), imageXYZInMM.y(), rayDir, WAVE_LENGTH_D, imageSensorOffset_, &tracePoints, &outRay))
            {
                // HACK: 本当であれば必ず成功するはず
                //continue;
            }
            //
            auto& path = svg.beginPath(-tracePoints[0].z(), tracePoints[0].y());
            path.setStrokeWidth(0.2f);
            path.setStrokeColor(rayColorStr );
            for (int32_t pn = 1; pn < tracePoints.size(); ++pn)
            {
                auto& p = tracePoints[pn];
                path.addLine(-p.z(), p.y());
            }
            path.endPath();
        }
    }
#else
    AL_ASSERT_ALWAYS(false);
#endif
}

/*
-------------------------------------------------
IntersectLens
Intersectのレンズ用簡易版
-------------------------------------------------
*/
struct IntersectLens AL_FINAL
{
public:
    float t;
    Vec3 normal;
    Vec3 position;
};

/*
-------------------------------------------------
intersectSphereShape()
rは負の値も受け付ける
-------------------------------------------------
*/
INLINE bool
intersectSphereAsLens(const Ray &ray, float oz, float r, IntersectLens &isect)
{
    float t;
    Vec3 rs = ray.o - Vec3(0.0f, 0.0f, oz);
    // 球中心からレイ原点までの距離二乗
    const float rs2 = Vec3::dot(rs, rs);
    // 半径二乗
    const float radius2 = r * r; // TODO: 事前計算するようにする
    const float lhs = Vec3::dot(ray.d, rs);
    // 交点がない場合は失敗
    const float D = lhs * lhs - rs2 + radius2;
    if (D < 0.0f)
    {
        return false;
    }
    //
    const float t0 = -lhs - sqrtf(D);
    const float t1 = -lhs + sqrtf(D);
    // 両方ともレイの後ろの場合は失敗
    if (t0 < 0.0f && t1 < 0.0f)
    {
        return false;
    }
    // 半径が負の場合は中心よりも手前の点を選択する
    if (r < 0.0f)
    {
        t = t0;
    }
    // 半径が正の場合は中心よりも奥の点を選択する
    else
    {
        t = t1;
    }
    // レンズでの使い方であれば常に正になるはず
    AL_ASSERT_DEBUG(t >= 0.0f);
    //
    isect.position = ray.o + ray.d * t;
    isect.normal = (isect.position - Vec3(0.0f, 0.0f, oz));
    isect.normal.normalize();
    // 半径が正の場合は法線が反転する
    if (r >= 0.0f)
    {
        isect.normal = -isect.normal;
    }
    isect.t = t;
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void intersectPlaneAsStopPlane(Vec3 rayOrig,
                                      Vec3 rayDir,
                                      const float dist,
                                      Vec3 *hitPos)
{
    // 平面は常に(0,0,1)を向いているので、dot(rayDir,planeNormal) = rayDir.z
    const float nDotD = rayDir.z();
    // 平行なレイは出ていない前提
    AL_ASSERT_DEBUG(fabsf(nDotD) != 0.0f);
    // 距離の算出
    const float t = (dist - rayOrig.z()) / nDotD;
    // レイ原点の背後には存在しない前提
    AL_ASSERT_DEBUG(t >= 0.0f);
    //
    *hitPos = rayOrig + rayDir * t;
    return;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
static bool isInsideElapse(float x, float y, float rx, float ry)
{
    const float x2 = x * x;
    const float y2 = y * y;
    const float rx2 = rx * rx;
    const float ry2 = ry * ry;
    return (x2 * ry2 + y2 * rx2) <= rx2 * ry2;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool RealSensor::lensTrace(float xOnFilmInMM,
                           float yOnFilmInMM,
                           Vec3 dir,
                           float waveLength,
                           float imageSensorOffset,
                           std::vector<Vec3> *points,
                           Ray *outRay) const
{
    AL_ASSERT_DEBUG(dir.isNormalized());
    const Vec3 ro(xOnFilmInMM, yOnFilmInMM, imageSensorOffset);
    Ray ray(ro, dir);
    // 最後のNd
    float lastNd = 1.0f;
    // 全てのレンズを見ていく
    const int32_t lensNumM1 = static_cast<int32_t>(surfs_.size()) - 1;
    for (int32_t i = lensNumM1; i >= 0; --i)
    {
        //
        if (points)
        {
            points->push_back(ray.o);
        }
        //
        const auto &lens = surfs_[i];
        // ストップ面の場合は、口径を通過するかを判定
        if (lens.isStop)
        {
            Vec3 hitPos;
            intersectPlaneAsStopPlane(ray.o, ray.d, lens.z, &hitPos);
            //
            if (!isInsideElapse(hitPos.x(), hitPos.y(), lens.apeX, lens.apeY))
            {
                // 最初から外しているのは何かがまずい
                // AL_ASSERT_DEBUG(i != surfs_.size() - 1);
                return false;
            }
        }
        // レンズの場合
        else
        {
            IntersectLens isect;
            // 球面の場合
            if (lens.invRadius != 0.0f)
            {
                // レンズに交差しない場合は失敗
                if (!intersectSphereAsLens(
                        ray, lens.z - lens.radius, lens.radius, isect))
                {
                    return false;
                }
            }
            // 平面の表面の場合
            else
            {
                AL_ASSERT_DEBUG(false);
            }
            // レンズ口径以上の場所に交差した場合も失敗
            const Vec3 p = isect.position;
            if (!isInsideElapse(p.x(), p.y(), lens.apeX, lens.apeY))
            {
                // 最初から外しているのは何かがまずい
                // AL_ASSERT_DEBUG(i != surfs_.size() - 1);
                return false;
            }
            // HACK: とりあえずNdだけを見るようにする
            const float nd = lens.realIor.ior(waveLength);
            Vec3 refractDir;
            // 全反射してしまった場合は失敗とする
            if (!refract(-ray.d, isect.normal, lastNd / nd, &refractDir))
            {
                return false;
            }
            lastNd = nd;
            //
            ray = Ray(isect.position, refractDir);
        }
    }
    //
    if (points)
    {
        points->push_back(ray.o);
        points->push_back(ray.o + ray.d * 30.0f);
    }
    //
    *outRay = ray;
    //
    return true;
}

/*
-------------------------------------------------
workingDistance()
現在の設定でのピント位置からレンズ先端までの距離を返す(mm単位)
-------------------------------------------------
*/
float RealSensor::workingDistance(const float imageSensorOffset) const
{
    // 結像地点を算出
    const float smallup = 0.00001f;
    Vec3 rayDir(0.0f, smallup, 1.0f);
    rayDir.normalize();
    Ray outRay;
    if (!lensTrace(0.0f,
                   0.0f,
                   rayDir,
                   WAVE_LENGTH_D,
                   imageSensorOffset,
                   NULL,
                   &outRay))
    {
        AL_ASSERT_DEBUG(false);
    }
    const float wd = -outRay.o.y() * outRay.d.z() / outRay.d.y();
    return wd;
}

/*
-------------------------------------------------
autoFocus()
指定した焦点距離に合わせるためにイメージプレーンを動かす
-------------------------------------------------
*/
void RealSensor::autoFocus(float aimWorkingDistance)
{
    //
    AL_ASSERT_DEBUG(!surfs_.empty());
    logging("Start AF");
#if 0 // 近軸近似版
    const float power =  lensPower();
    const float fff = 1.0f / power;
    const float imageDistance = -(1.0f / (power - 1.0f / objectDistance));
    const LensSurface& lastSurf = surfs_[surfs_.size() - 1];
    imageSensorOffset_ = -(imageDistance - lastSurf.distance);
    imageSensorOffset_;
    logging("Finish AF");
#else // バイナリサーチ版
    // const float smallup = 0.00001f;
    const LensSurface &lastSurf = surfs_.back();
    float sensorOffsetLow = -lastSurf.distance * 2.0f;
    float sensorOffsetHigh = sensorOffsetRangeUpperBound();
    // フォーカス位置を探す
    for (int32_t loopNo = 0; loopNo < 32; ++loopNo)
    {
        const float mid = (sensorOffsetHigh + sensorOffsetLow) * 0.5f;
        //
        const float imageSensorOffsetTmp = mid;
        const float wd = workingDistance(imageSensorOffsetTmp);
        // 十分近づけば終了
        if (fabsf(1.0f - wd / aimWorkingDistance) < 0.0001f)
        {
            imageSensorOffset_ = imageSensorOffsetTmp;
            logging("Finish AF");
            return;
        }
        // イメージプレーンを前に進める場合
        if (wd < aimWorkingDistance)
        {
            sensorOffsetLow = mid;
        }
        // イメージプレーンを後ろに進める場合
        else
        {
            sensorOffsetHigh = mid;
        }
    }
    loggingError("AF faild.");
#endif
}

/*
-------------------------------------------------
sensorOffsetRange()
センサー位置の上限と下限を算出する
-------------------------------------------------
*/
float RealSensor::sensorOffsetRangeUpperBound() const
{
    // const float smallup = 0.00001f;
    const LensSurface &lastSurf = surfs_.back();
    // float base = 0.0f;
    // HACK: 下限の開始位置が適当。
    float sensorOffsetLow = -lastSurf.distance * 2.0f;
    float sensorOffsetHigh = lastSurf.distance * 0.999f;
    // WorkingDistanceが無限遠になるイメージセンサー位置を探す
    for (int32_t loopNo = 0; loopNo < 32; ++loopNo)
    {
        const float mid = (sensorOffsetHigh + sensorOffsetLow) * 0.5f;
        //
        const float imageSensorOffsetTmp = mid;
        const float wd = workingDistance(imageSensorOffsetTmp);
        // 十分近づけば終了
        if (fabsf(1.0f - sensorOffsetHigh / sensorOffsetLow) < 0.0001f &&
            wd > 0.0f)
        {
            return imageSensorOffsetTmp;
        }
        // WorkingDistanceが負(収束しない)の場合は、進め過ぎたので後ろにする
        if (wd < 0.0f)
        {
            sensorOffsetHigh = mid;
        }
        else
        {
            sensorOffsetLow = mid;
        }
    }
    //
    loggingError("sensorOffsetRangeUpperBound() faild.");
    return 0.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float RealSensor::lensPower() const
{
    float prevIOR = 1.0f;
    float power = 0.0f;

    for (int32_t sn = 0; sn < surfs_.size() - 1; ++sn)
    {
        if (surfs_[sn].isStop)
        {
            prevIOR = 1.0f;
            continue;
        }
        const float curIor = surfs_[sn + 1].realIor.ior();

        const float n1 = prevIOR;
        const float n2 = curIor;
        const float invR1 = surfs_[sn].invRadius;
        const float curPower = (n1 - n2) * invR1;
        power += curPower;
        prevIOR = curIor;
    }
    return power;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Ray RealSensor::generateRay(float imageX, float imageY, float &pdf) const
{
    //
    const Vec2 lensSample(
        0.0f, 0.0f); // TODO: RealLensにSamplerが引数で渡されるのを待つ
    ++sampleNo_; // TOOD: サンプル番号は呼び出し元からもらうようにする。
    const float xOnFilmInMM =
        (0.5f - imageX * invScreenWidth_) * filmWidthInMM_;
    const float yOnFilmInMM =
        (imageY * invScreenHeight_ - 0.5f) * filmHeightInMM_;
    /*
    フィルム上から後玉上の射出瞳上へのレイのサンプル位置を決定する
    HACK:
    一様に飛ばした上でpdfなどは用意していないが、本当はこの方法は正しくない。
    とりあえず絵が出るまではこの方法を使う
    */
    // Vec2 sampleOnLens = remapSquareToDiscConcentric(lensSample);
    Vec2 sampleOnLens = Vec2(0.0f, 0.0f);
    const LensSurface &lastLensSurf = surfs_.back();
    const float sampleOnLensZ = lastLensSurf.distance;
#if 1
    sampleOnLens.setX(sampleOnLens.x() * lastLensSurf.apeX);
    sampleOnLens.setY(sampleOnLens.y() * lastLensSurf.apeY);
#else
    const int32_t epIndex = (int32_t)imageX + (int32_t)imageY * imageWidth;
    const auto &ep = exitPupils_[epIndex];
    // sampleOnLensは[-1,1]で、onCoord()は[0,1]を期待している
    CONST Vec2 samplePoint = ep.onCoord(sampleOnLens * 0.5f + Vec2(0.5f, 0.5f));
    sampleOnLens.x = samplePoint.x;
    sampleOnLens.y = samplePoint.y;
#endif

    // HACK: 光学系だけでも波長を見る予定だったがうまくいかないのでD線固定
    Vec3 rayDir(sampleOnLens.x() - xOnFilmInMM,
                sampleOnLens.y() - yOnFilmInMM,
                sampleOnLensZ);
    rayDir.normalize();
    Ray outRay;
    if (!lensTrace(xOnFilmInMM,
                   yOnFilmInMM,
                   rayDir,
                   WAVE_LENGTH_D,
                   imageSensorOffset_,
                   NULL,
                   &outRay))
    {
        pdf = 0.0f;
        return Ray();
    }
    // HACK: とりあえず常に1
    pdf = 1.0f;

    // zはoffsetを0に。
    outRay.o.setZ(0.0f);
    // xyは単位をスケールする
    outRay.o.setX(outRay.o.x() * scale_);
    outRay.o.setY(outRay.o.y() * scale_);
    // ワールド空間に変換
    const Vec3 worldDir = lcoal_.local2world(outRay.d);
    Vec3 worldOrig = lcoal_.local2world(outRay.o);
    //
    worldOrig += perspectivePos_;
    return Ray(worldOrig, worldDir);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
FilmPtr RealSensor::film() const { return film_; }
