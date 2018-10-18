#include "catch2/catch.hpp"
#include "light/light.hpp"
#include "core/floatstreamstats.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("ConstantLight0", "Light")
{
    /*
    ライトに照らされている完全各三面の平面を考える
    */
    ConstantLight light;
    light.init(Spectrum::White);
    // TODO: ランダムにサンプル
    SamplerIndepent sampler;
    sampler.setHash(0x01);
    FloatStreamStats<> stats;
    for (int32_t sn = 0; sn < 1024 * 16; ++sn)
    {
        sampler.startSample(sn);
        float pdf;
        const Vec3 dir = sampler.getHemisphere(&pdf);
        Intersect isect;
        isect.clear();
        const bool hit =
            light.intersect(Ray(Vec3(0.0f, 0.0f, 0.0f), dir), &isect);
        if (hit)
        {
            const float reflection = 1.0f / PI;
            stats.add(dir.z() * 2.0f * PI * reflection);
        }
        else
        {
            stats.add(0.0f);
        }
    }
    //
    FloatStreamStats<> stats2;
    // 明示的にサンプル
    for (int32_t sn = 0; sn < 1024 * 128; ++sn)
    {
        sampler.startSample(sn);
        Vec3 samplePos;
        float pdf;
        const Spectrum e =
            light.sampleLe(&sampler, Vec3(0.0f, 0.0f, 0.0f), &samplePos, &pdf);
        if (samplePos.z() < 0.0f)
        {
            stats2.add(0.0f);
        }
        else
        {
            const Vec3 wi = samplePos.normalized();
            const float reflection = 1.0f / PI;
            stats2.add(e.r * wi.z() * reflection / pdf);
        }
    }
    // 真値が同じ範囲に入っているかチェック
    //AL_ASSERT_ALWAYS(FloatStreamStats::maybeSameMean(stats, stats2));
}


/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("RectangleLight0", "Light")
{
    // HACK: 止まらないが非常に遅い
    return;
    /*
    1x1のRectangleLightが地平面に対してある場合
    */
    RectangleLight light;
    light.init(Spectrum::White,
        Vec3(0.0f, 0.0f, 1.0f),
        Vec3(1.0f, 0.0f, 0.0f),
        Vec3(0.0f, 1.0f, 0.0f));
    // TODO: ランダムにサンプル
    SamplerIndepent sampler;
    sampler.setHash(0x01);
    FloatStreamStats<> stats;
    for (int32_t sn = 0; sn < 1024 * 1024; ++sn)
    {
        sampler.startSample(sn);
        float pdfSphere;
        const Vec3 dir = sampler.getHemisphere(&pdfSphere);
        Intersect isect;
        isect.clear();
        const bool hit =
            light.intersect(Ray(Vec3(0.0f, 0.0f, 0.0f), dir), &isect);
        if (hit)
        {
            // stats.add(dir.z);
            stats.add(1.0f * 2.0f * PI);
        }
        else
        {
            stats.add(0.0f);
        }
    }
    //
    FloatStreamStats<> stats2;
    // 明示的にサンプル
    for (int32_t sn = 0; sn < 1024; ++sn)
    {
        sampler.startSample(sn);
        Vec3 samplePos;
        float pdf;
        const Spectrum e =
            light.sampleLe(&sampler, Vec3(0.0f, 0.0f, 0.0f), &samplePos, &pdf);
        stats2.add(e.r / pdf);
    }
    // 真値が同じ範囲に入っているかチェック
    //AL_ASSERT_ALWAYS(FloatStreamStats::maybeSameMean(stats, stats2));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("RectangleLight1", "Light")
{
    return;
    /*
    1x1のRectangleLightが地平面に対してある場合
    */
    RectangleLight light;
    light.init(Spectrum::White,
        Vec3(0.0f, 0.0f, 0.2f),
        Vec3(1000.0f, 0.0f, 0.0f),
        Vec3(0.0f, 1000.0f, 0.0f));
    // TODO: ランダムにサンプル
    SamplerIndepent sampler;
    sampler.setHash(0x01);
    FloatStreamStats<> stats;
    for (int32_t sn = 0; sn < 1024 * 16; ++sn)
    {
        sampler.startSample(sn);
        float pdfSphere;
        const Vec3 dir = sampler.getHemisphere(&pdfSphere);
        Intersect isect;
        isect.clear();
        const bool hit =
            light.intersect(Ray(Vec3(0.0f, 0.0f, 0.0f), dir), &isect);
        if (hit)
        {
            const float reflection = 1.0f / PI;
            stats.add(dir.z() * 2.0f * PI * reflection);
        }
        else
        {
            stats.add(0.0f);
        }
    }
    //
    FloatStreamStats<> stats2;
    // 明示的にサンプル
    for (int32_t sn = 0; sn < 1024 * 128; ++sn)
    {
        sampler.startSample(sn);
        Vec3 samplePos;
        float pdf;
        const Spectrum e =
            light.sampleLe(&sampler, Vec3(0.0f, 0.0f, 0.0f), &samplePos, &pdf);
        const Vec3 wi = samplePos.normalized();
        stats2.add(e.r * wi.z() / pdf);
    }
    // 真値が同じ範囲に入っているかチェック
    //AL_ASSERT_ALWAYS(FloatStreamStats::maybeSameMean(stats, stats2));
}


/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("SphereLight1", "Light")
{
    /*
    1x1のRectangleLightが地平面に対してある場合
    */
    SphereLight light;
    light.init(Vec3(0.0f, 0.0f, 0.2f), 0.1f, Spectrum::White);
    // TODO: ランダムにサンプル
    SamplerIndepent sampler;
    sampler.setHash(0x01);
    FloatStreamStats<> stats;
    for (int32_t sn = 0; sn < 1024 * 16; ++sn)
    {
        sampler.startSample(sn);
        float pdfSphere;
        const Vec3 dir = sampler.getHemisphere(&pdfSphere);
        Intersect isect;
        isect.clear();
        const bool hit =
            light.intersect(Ray(Vec3(0.0f, 0.0f, 0.0f), dir), &isect);
        if (hit)
        {
            const float reflection = 1.0f / PI;
            stats.add(dir.z() * 2.0f * PI * reflection);
        }
        else
        {
            stats.add(0.0f);
        }
    }
    //
    FloatStreamStats<> stats2;
    // 明示的にサンプル
    for (int32_t sn = 0; sn < 1024 * 128; ++sn)
    {
        sampler.startSample(sn);
        Vec3 samplePos;
        float pdf;
        const Spectrum e =
            light.sampleLe(&sampler, Vec3(0.0f, 0.0f, 0.0f), &samplePos, &pdf);
        const Vec3 wi = samplePos.normalized();
        stats2.add(e.r * wi.z() / pdf);
    }
    // 真値が同じ範囲に入っているかチェック
    //AL_ASSERT_ALWAYS(FloatStreamStats::maybeSameMean(stats, stats2));
}


#if 0
/*
-------------------------------------------------
-------------------------------------------------
*/
std::vector<MedianCutBoxInfo> EnviromentLight::medianCut(int32_t cutNo) const
{
    // SATの計算
    const auto computeSAT = [](const Image& src, Image& satImage) ->void
    {
        const int32_t w = src.width();
        const int32_t h = src.height();
        satImage.resize(w, h);
        for (int32_t y = 0; y < h; ++y)
        {
            for (int32_t x = 0; x < w; ++x)
            {
                auto p = src.pixel(x, y);
                if (x != 0)
                {
                    p += satImage.pixel(x - 1, y);
                }
                if (y != 0)
                {
                    p += satImage.pixel(x, y - 1);
                }
                if (x != 0 && y != 0)
                {
                    p -= satImage.pixel(x - 1, y - 1);
                }
                auto& s = satImage.pixel(x, y);
                s = p;
            }
        }
    };
    // AreaSumを返す。開始点を含んだ範囲内の和を計算するために-1を入れる場合があるため、
    // 負数もありえるため、その対処もしている。
    const auto areaSum = [](const Image& image, int x0, int y0, int x1, int y1)-> Spectrum
    {
        Spectrum r(0.0f);
        r += image.pixel(x1, y1);
        r -= (x0 < 0) ? Spectrum(0.0f) : image.pixel(x0, y1);
        r -= (y0 < 0) ? Spectrum(0.0f) : image.pixel(x1, y0);
        r += (x0 < 0 || y0 < 0) ? Spectrum(0.0f) : image.pixel(x0, y0);
        return r;
    };

    const int32_t width = image_.width();
    const int32_t height = image_.height();
    const float widthf = (float)width;
    const float heightf = (float)height;

    // TODO: cutNoが2の累乗かをチェック

    // Cos項がかかかった輝度値と輝度値のSAT
    Image monoBuffer;
    Image monoSAT;
    monoBuffer.resize(width, height);
    monoSAT.resize(width, height);
    // Cos項がかからないカラー値のSAT
    Image colorSAT;

    // 輝度値を入れる
    for (int32_t y = 0; y < height; ++y)
    {
        const float phi = ((float)y / (float)image_.height() - 0.5f) * PI;
        // const float cosTerm = fabsf(cosf(phi));
        const float cosTerm = 1.0f;
        for (int32_t x = 0; x < width; ++x)
        {
            const auto& p = image_.pixel(x, y);
            auto& d = monoBuffer.pixel(x, y);
            const float l = p.luminance() * cosTerm;
            d = Spectrum(l);
        }
    }

    // 各SATの計算
    computeSAT(monoBuffer, monoSAT);
    computeSAT(image_, colorSAT);

    //
    std::deque<AABB2D> boxs;
    // 全体を一つのボックスに入れる
    AABB2D rootBox;
    rootBox.addPoint(Vec2(0.0f, 0.0f));
    rootBox.addPoint(Vec2(widthf - 1.0f, heightf - 1.0f));
    boxs.push_back(rootBox);
    //
    while (!boxs.empty() && boxs.size() < cutNo)
    {
        // 先頭のボックスを取り出す
        auto rootBox = boxs.front();
        boxs.pop_front();
        // ボックスの中央
        const Vec2 center = rootBox.center();
        const Vec2 min = rootBox.min();
        const Vec2 max = rootBox.max();
        // ボックスの長軸を出す。Y方向はcosTerm分補正される(極付近であれば短くする)。
        const float phi = (center.y / (float)height - 0.5f) * PI;
        const float cosTerm = fabsf(cosf(phi));
        // ボックスの長軸を判定する。0:x 1:y
        const bool isAxisIsX = cosTerm * rootBox.width() > rootBox.height();
        // 分割点を探す
        int32_t splitIndex = 0;
        float minimuDif = std::numeric_limits<float>::max();
        if (isAxisIsX)
        {
            for (int32_t i = (int32_t)min.x; i < (int32_t)max.x; ++i)
            {
                const int32_t sx = (int32_t)min.x;
                const int32_t x = i;
                const int32_t ex = (int32_t)max.x;
                const int32_t yu = (int32_t)min.y;;
                const int32_t yd = (int32_t)max.y;;
                auto p0 = areaSum(monoSAT, sx - 1, yu - 1, x, yd);
                auto p1 = areaSum(monoSAT, x - 1, yu - 1, ex, yd);
                // エネルギーの差の計算
                const float energyDif = Spectrum::energyDif(p0, p1);
                if (energyDif < minimuDif)
                {
                    minimuDif = energyDif;
                    splitIndex = i;
                }
            }
            // 新しいボックスを作成する
            AABB2D leftBox;
            AABB2D rightBox;
            leftBox.addPoint(min);
            leftBox.addPoint(Vec2((float)(splitIndex - 1), max.y));
            rightBox.addPoint(Vec2((float)splitIndex, min.y));
            rightBox.addPoint(max);
            //
            boxs.push_back(leftBox);
            boxs.push_back(rightBox);
        }
        else
        {
            for (int32_t i = (int32_t)min.y; i < (int32_t)max.y; ++i)
            {
                const int32_t lx = (int32_t)min.x;
                const int32_t rx = (int32_t)max.x;
                const int32_t sy = (int32_t)min.y;
                const int32_t y = i;
                const int32_t ey = (int32_t)max.y;
                auto p0 = areaSum(monoSAT, lx - 1, sy - 1, rx, y);
                auto p1 = areaSum(monoSAT, lx - 1, y - 1, rx, ey);
                // エネルギーの差の計算
                const float energyDif = Spectrum::energyDif(p0, p1);
                if (energyDif < minimuDif)
                {
                    minimuDif = energyDif;
                    splitIndex = i;
                }
            }
            // 新しいボックスを作成する
            AABB2D upBox;
            AABB2D downBox;
            upBox.addPoint(min);
            upBox.addPoint(Vec2(max.x, (float)(splitIndex - 1)));
            downBox.addPoint(Vec2(min.x, (float)splitIndex));
            downBox.addPoint(max);
            //
            boxs.push_back(upBox);
            boxs.push_back(downBox);
        }
    }

    //
    std::vector<MedianCutBoxInfo> result;
    for (const auto& box : boxs)
    {
        // 各ボックスの輝度重心を計算する
        const auto& min = box.min();
        const auto& max = box.max();

        // HACK: ちゃんと対応するが、とりあえず潰れてるboxはスキップする
        if (min.x < 0 || min.y < 0 || min.x == max.x || min.y == max.y)
        {
            //   continue;
        }

        // 小さな値を大量に加算したSATでは無視できないレベルの誤差が発生するため、
        // regionSumはSATからではなくこの場で計算する。
        float regionSum = 0.0f;
        Vec2 centroid(0.0f);
        for (int32_t x = (int32_t)min.x; x <= (int32_t)max.x; ++x)
        {
            for (int32_t y = (int32_t)min.y; y <= (int32_t)max.y; ++y)
            {
                // HACK: 何かがおかしい
                y = alMax(0, y);
                x = alMax(0, x);
                //
                auto p = monoBuffer.pixel(x, y).luminance();
                regionSum += p;
                centroid += p * Vec2((float)x - min.x, (float)y - min.y);
            }
        }
        loggingWarningIf(regionSum <= 0.0f, "regionSum <= 0.0");
        centroid = centroid / regionSum;
        centroid += min;
        AL_ASSERT_DEBUG(min.x <= centroid.x && centroid.x <= max.x);
        AL_ASSERT_DEBUG(min.y <= centroid.y && centroid.y <= max.y);

        // 重心輝度を求める
        const Spectrum intensity = areaSum(colorSAT, (int32_t)min.x - 1, (int32_t)min.y - 1, (int32_t)max.x, (int32_t)max.y) / (widthf*heightf);
        // theta/phiを求める
        const float theta = centroid.x / widthf * 2.0f * PI;
        const float phi = (centroid.y / heightf - 0.5f) * PI;
        const Vec3 dir(cosf(phi)*cosf(theta), cosf(phi)*sinf(theta), sinf(phi));
        // 出力情報としてまとめる
        MedianCutBoxInfo boxInfo;
        boxInfo.intensity = intensity;
        boxInfo.dir = dir;
        boxInfo.centroidInLatLongSpace = centroid;
        boxInfo.theta = theta;
        boxInfo.phi = phi;
        boxInfo.minx = (int32_t)box.min().x;
        boxInfo.miny = (int32_t)box.min().y;
        boxInfo.maxx = (int32_t)box.max().x;
        boxInfo.maxy = (int32_t)box.max().y;
        result.push_back(boxInfo);
    }
    //
    return result;
}
#endif

#if 0
/*
-------------------------------------------------
-------------------------------------------------
*/
static bool TestEnviroment()
{
    //worldDir = { x = -1.35296619 y = -0.940121114 z = -1.05976379 }
    /*Vec2 uv;
    const Vec3 worldDir(-1.35296619f, -0.940121114f, -1.05976379f);
    dir2tex(worldDir, &uv);
    AL_ASSERT_DEBUG(!uv.hasNan());*/
#if 0
    // ブルートフォースで輝度平均を得る
    {
        XorShift128 rng;
        ObjectProp prop;
        prop.createFromString(
            "<emitter type = \"envmap\">"\
            "<string name = \"filename\" value = \"../asset/scene/ldr.bmp\" />"\
            "<float samplingWeight = \"1.0\" />"\
            "</emitter>");
        LightPtr light2 = createObjectOld<Light>("envmap", prop);
        EnviromentLight* light = dynamic_cast<EnviromentLight*>(light2.get());
        const Spectrum meanRadiance = light->meanRadiance();
        const int32_t NUM_SAMPLE_BF = 1024 * 1024;
        Spectrum bruteTotal;
        bruteTotal.clear();
        float weight = 0.0f;
        for (int32_t i = 0; i < NUM_SAMPLE_BF; ++i)
        {
            Vec3 dir;
            uniformInSphere(rng.nextFloat(), rng.nextFloat(), &dir);
            Ray ray(Vec3(0.0f), dir);
            const float c = Vec3::dot(dir, Vec3(0.0f, 0.0f, 1.0f));
            const Spectrum spectrum = light->emission(ray);
            bruteTotal += spectrum;
        }
        bruteTotal /= (float)NUM_SAMPLE_BF;
        const float rate = bruteTotal.r / meanRadiance.r;
        AL_ASSERT_DEBUG(fabs(rate - 1.0f) < 0.01f);
    }
#endif

#if 0
    // ブルートフォースで照度(Z-up)を得る
    {
        XorShift128 rng;
        ObjectProp prop;
        prop.createFromString(
            "<emitter type = \"envmap\">"\
            "<string name = \"filename\" value = \"../asset/scene/ldr_white.bmp\" />"\
            "<float samplingWeight = \"1.0\" />"\
            "</emitter>");
        LightPtr light2 = createObjectOld<Light>("envmap", prop);
        EnviromentLight* light = dynamic_cast<EnviromentLight*>(light2.get());
        const Spectrum irradianceZup = light->irradianceZUp();
        const int32_t NUM_SAMPLE_BF = 1024 * 1024;
        Spectrum bruteTotal;
        bruteTotal.clear();
        float weight = 0.0f;
        for (int32_t i = 0; i < NUM_SAMPLE_BF; ++i)
        {
            const Vec3 dir = remapSquareToHemisphere(Vec2(rng.nextFloat(), rng.nextFloat()));
            Ray ray(Vec3(0.0f), dir);
            const float c = Vec3::dot(dir, Vec3(0.0f, 0.0f, 1.0f));
            const Spectrum spectrum = light->emission(ray) * c;
            bruteTotal += spectrum;
        }
        bruteTotal /= (float)NUM_SAMPLE_BF;
        bruteTotal *= 2.0f * PI;
        const float rate = bruteTotal.r / irradianceZup.r;
        AL_ASSERT_DEBUG(rate < 0.01f);
    }
#endif

#if 0
    // ImportanceSamplingで輝度平均を得る
    {
        XorShift128 rng;
        ObjectProp prop;
        prop.createFromString(
            "<emitter type = \"envmap\">"\
            "<string name = \"filename\" value = \"../asset/scene/ldr.bmp\" />"\
            "<float samplingWeight = \"1.0\" />"\
            "</emitter>");
        LightPtr light2 = createObjectOld<Light>("envmap", prop);
        EnviromentLight* light = dynamic_cast<EnviromentLight*>(light2.get());
        const auto fillFunc = [](float theta, float phi)
        {
            //return Spectrum(1.0f);
            //return Spectrum(alMax(sinf(theta*2.0f), 0.0f));
            //return Spectrum(alMax(cosf(theta*2.0f), 0.0f));
            return Spectrum(alMax(theta*theta, 0.0f));
        };
        light->fill(128, 128, fillFunc);
        const Spectrum meanRadiance = light->meanRadiance();
        float importanceSampleTotal = 0.0f;
        const int32_t NUM_SAMPLE = 1024;
        for (int32_t i = 0; i < NUM_SAMPLE; ++i)
        {
            Vec3 dir;
            Spectrum spectrum;
            float pdf;
            light->sample(Vec3(0.0f), rng, &spectrum, &dir, &pdf);
            importanceSampleTotal += spectrum.r / pdf;
        }
        importanceSampleTotal /= (float)NUM_SAMPLE;
        const float rate = fabsf(1.0f - meanRadiance.r / importanceSampleTotal);
        AL_ASSERT_DEBUG(rate < 0.01f);
    }
#endif

#if 0
    // ImportanceSamplingで照度(Z-up)を得る
    {
        XorShift128 rng;
        ObjectProp prop;
        prop.createFromString(
            "<emitter type = \"envmap\">"\
            "<string name = \"filename\" value = \"../asset/test_envmap/Mans_Outside_2k.hdr\" />"\
            "<float samplingWeight = \"1.0\" />"\
            "</emitter>");
        LightPtr light2 = createObjectOld<Light>("envmap", prop);
        EnviromentLight* light = dynamic_cast<EnviromentLight*>(light2.get());
        const auto fillFunc = [](float theta, float phi)
        {
            // TOOD: ここで Spectrum(1.0f)以外、正しい結果にならない。原因を考える
            //return Spectrum(1.0f);
            return Spectrum(alMax(sinf(theta*2.0f), 0.0f));
            //return Spectrum(alMax(cosf(theta*2.0f), 0.0f));
            //return Spectrum(alMax(theta*theta, 0.0f));
        };
        //light->fill(128, 128, fillFunc);
        const Image& image = light->image();

        float importanceSampleTotal = 0.0f;
        const int32_t NUM_SAMPLE = 1024 * 1024 * 16;
        const float invNumSample = 1.0f / (float)NUM_SAMPLE;
        Spectrum randomAve, importanceAve;
        randomAve.clear();
        importanceAve.clear();
        for (int32_t i = 0; i < NUM_SAMPLE; ++i)
        {
            Vec3 dir;
            Spectrum spectrum;
            float pdf;
            light->sampleDir(Vec3(0.0f), rng, &spectrum, &dir, &pdf);
            importanceAve += spectrum / pdf * invNumSample;
            //
            Vec3 randomDir;
            uniformInSphere(rng.nextFloat(), rng.nextFloat(), &randomDir);
            randomAve += light->emission(Ray(Vec3(0.0f), randomDir)) * invNumSample;
        }
        const float rateR = randomAve.r / importanceAve.r;
        const float rateG = randomAve.g / importanceAve.g;
        const float rateB = randomAve.b / importanceAve.b;
        AL_ASSERT_DEBUG(rateR < 0.01f);
    }
#endif
    return true;
}
#endif
// TEST(Enviroment, Basic, TestEnviroment);
