#include "bsdf/autoimportance.hpp"
#include "core/image.hpp"
#include "core/logging.hpp"
/*
TODOs
- "An importance sampling method for arbitrary BRDFs(2009)"をちゃんと実装する
-
*/

/*
-------------------------------------------------
-------------------------------------------------
*/
AutoImportance::AutoImportance() {}

/*
-------------------------------------------------
-------------------------------------------------
*/
void AutoImportance::setBRDF(const BSDF* bsdf)
{
    XorShift128 rng;
    distributions_.clear();
    const int32_t numWo = 16;
    for (int32_t i = 0; i < numWo; ++i)
    {
        const float coso = (float)(i + 1) / (float)numWo;
        Distribution2D dist;
        genDistribution(coso, bsdf, rng, &dist);
        distributions_.push_back(std::make_pair(coso, dist));
    }

#if 0 // PDF画像出力
    Distribution2D& dst2d = distributions_[2].second;
    FILE* file = NULL;
    file = fopen("test.csv","wt");
    for (int32_t i = 0; i < 1024*16; ++i)
    {
        float pdf;
        Vec2 pos;
        dst2d.sample(rng.nextFloat(), rng.nextFloat(), &pdf, &pos);
        fprintf(file, "%f,%f\n", pos.x, pos.y);
    }
    fclose(file);

    //
    Image testImage;
    testImage.resize(dst2d.column(), dst2d.row());
    for (int32_t y = 0; y < dst2d.row(); ++y)
    {
        for (int32_t x = 0; x < dst2d.column(); ++x)
        {
            const float pdf = dst2d.pdf(x, y);
            /*const float pdf1 = dst2d.pdf(64, 0);
            const float pdf2 = dst2d.pdf(64, 64);*/
            //const float pdf = dst2d.pdf(64, 64);
            const float c = pdf * 0.5f;
            auto& p = testImage.pixel(x, y);
            p.r = c;
            p.g = c;
            p.b = c;
        }
    }
#if defined(WINDOWS)
    ImageIO::writeBmp("heat.bmp", testImage);
#else
    AL_ASSERT_ALWAYS(false);
#endif
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void AutoImportance::genDistribution(float coso,
                                     const BSDF* bsdf,
                                     const XorShift128& rng,
                                     Distribution2D* distribution)
{
    // phiは0固定で考える
    const float sin0 = sqrtf(1.0f - coso * coso);
    const Vec3 wo(sin0, 0.0f, coso);
    const int32_t GRID_SIZE = 64; // TODO: パラメーター化
    const int32_t GRID_SIZE_HALF = GRID_SIZE / 2;
    const int32_t SAMPLE_NUM = 16; // TODO: パラメーター化
    const float invSampleNum = 1.0f / (float)SAMPLE_NUM;
    std::vector<std::vector<float>> values;

    // TODO: サンプリングパターンを生成する

    //
    for (int32_t y = 0; y < GRID_SIZE; ++y)
    {
        //
        std::vector<float> rowValues;
        for (int32_t x = 0; x < GRID_SIZE; ++x)
        {
#if 0
            // 少なくとも円に含まれているかを判定
            const float xmn = (float)x / (float)(GRID_SIZE_HALF)-1.0f;
            const float ymn = (float)y / (float)(GRID_SIZE_HALF)-1.0f;
            const float xmx = (float)(x + 1) / (float)(GRID_SIZE_HALF)-1.0f;
            const float ymx = (float)(y + 1) / (float)(GRID_SIZE_HALF)-1.0f;
            const Vec2 v0(xmn, ymn);
            const Vec2 v1(xmx, ymn);
            const Vec2 v2(xmn, ymx);
            const Vec2 v3(xmx, ymx);
            const float minLenSq = alMin(alMin(v0.lengthSq(), v1.lengthSq()), alMin(v2.lengthSq(), v3.lengthSq()));
            const bool isInsideCiecle = minLenSq <= 1.0f;
#endif
            //
            //

            //
            float mean = 0.0f;
            for (int32_t sn = 0; sn < SAMPLE_NUM; ++sn)
            {
                // 微妙に摂動させる
                const float fx =
                    ((float)x + rng.nextFloat()) / (float)(GRID_SIZE_HALF)-1.0f;
                const float fy =
                    ((float)y + rng.nextFloat()) / (float)(GRID_SIZE_HALF)-1.0f;
                const float fz2 = 1.0f - (fx * fx + fy * fy);
                if (fz2 > 0.0f)
                {
                    const float fz = sqrtf(fz2);
                    const Vec3 wi(fx, fy, fz);
                    const Spectrum reflectance = bsdf->bsdf(wo, wi);
                    mean += invSampleNum * reflectance.luminance();
                }
                else
                {
                    // 何もしない
                    mean += 0.0f;
                }
            }
            rowValues.push_back(mean);
        }
        values.push_back(rowValues);
    }
    //
    distribution->construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void AutoImportance::sample(Vec3 localWo,
                            XorShift128& rng,
                            Vec3* localWi,
                            float* pdf) const
{
    const float coso = localWo.z();
    auto ite = std::lower_bound(distributions_.begin(),
                                distributions_.end(),
                                coso,
                                [](const DistributionWo& lhs, float v) -> bool {
                                    return lhs.first > v;
                                });
    if (ite == distributions_.end())
    {
        ite = distributions_.begin();
    }
    const Distribution2D& dist2d = ite->second;
    Vec2 samplePointOrig;
    int32_t offsetU;
    int32_t offsetV;
    dist2d.sample(rng.nextFloat(),
                  rng.nextFloat(),
                  pdf,
                  &samplePointOrig,
                  &offsetU,
                  &offsetV);
    Vec2 samplePoint;
    samplePoint.setX(samplePointOrig.x() - 0.5f);
    samplePoint.setY(samplePointOrig.y() - 0.5f);
    samplePoint.setX(samplePoint.x() * 2.0f);
    samplePoint.setY(samplePoint.y() * 2.0f);
#if 1
    // セル内で摂動
    samplePoint.setX(samplePoint.x() +
                     (rng.nextFloat() - 0.5f) * dist2d.invRow());
    samplePoint.setY(samplePoint.y() +
                     (rng.nextFloat() - 0.5f) * dist2d.invColumn());
#endif
    // HACK: どうしても円から外れる場合があり得るため正規化
    // samplePoint.normalize();
    //
    const float phi = atan2f(localWo.y(), localWo.x());
    const float cosft = cosf(phi);
    const float sinft = sinf(phi);
    const float x = samplePoint.x() * cosft - samplePoint.x() * sinft;
    const float y = samplePoint.y() * sinft + samplePoint.y() * cosft;
    // 1を超えることはあり得るため少し甘めに見える
    // const float lenSq = x*x + y*y;
    // AL_ASSERT_DEBUG(lenSq  <= 1.02f);
    // HACK: この定数は非常にまずい
    const float z = sqrtf(alMax(1.0f - (x * x + y * y), 0.0001f));
    *localWi = Vec3(x, y, z);
    //
    AL_ASSERT_DEBUG(!localWi->hasNan());
    // HACK: ここでも正規化
    localWi->normalize();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float AutoImportance::pdf(Vec3 localWo, Vec3 localWi) const
{
    const float coso = localWo.z();
    auto ite = std::lower_bound(distributions_.begin(),
                                distributions_.end(),
                                coso,
                                [](const DistributionWo& lhs, float v) -> bool {
                                    return lhs.first > v;
                                });
    if (ite == distributions_.end())
    {
        ite = distributions_.begin();
    }
    const Distribution2D& dist2d = ite->second;
    //
    const float phi = -atan2f(localWo.y(), localWo.x());
    const float cosft = cosf(phi);
    const float sinft = sinf(phi);
    const float x = localWi.x() * cosft - localWi.y() * sinft;
    const float y = localWi.x() * sinft + localWi.y() * cosft;
    //
    const int32_t row = dist2d.row();
    const int32_t col = dist2d.column();
    const int32_t xi = alMin((int32_t)(row * (x * 0.5f + 0.5f)), row - 1);
    const int32_t yi = alMin((int32_t)(col * (y * 0.5f + 0.5f)), col - 1);
    return dist2d.pdf(xi, yi);
}

