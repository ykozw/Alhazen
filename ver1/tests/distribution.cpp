#include "catch2/catch.hpp"
#include "core/distribution1d.hpp"
#include "core/distribution2d.hpp"
#include "core/rng.hpp"
#include "core/math.hpp"
#include "core/floatstreamstats.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("OneDimention", "Distribution")
{
    // 推定が正しいかチェック
    {
        XorShift128 rng;
        const std::vector<float> samples = { { 0.1f, 0.4f } };
        Distribution1D distribution1d(samples);
        const int32_t NUM_SAMPLE = 1000;
        float total = 0.0f;
        for (int32_t i = 0; i < NUM_SAMPLE + 1; ++i)
        {
            float pdf;
            const float samplePoint =
                distribution1d.sample(rng.nextFloat(), &pdf);
            const float value = alLerp(samples[0], samples[1], samplePoint);
            total += value / pdf;
        }
        total /= (float)NUM_SAMPLE;
        if (fabsf(total / 0.25f - 1.0f) > 0.01f)
        {
            return;
        }
    }
    return;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("OneDimention2", "Distribution")
{

    /*
    TODOs
    - 速度テスト
    - χ2テスト
    */
    std::mt19937 rng(0x123);
    std::uniform_real_distribution<> distUnif;
    std::vector<float> elements;
    //
    SECTION("little ements")
    {
        // 長さ0～4で動作するかテスト
        for (int32_t numElements = 0; numElements <= 4; ++numElements)
        {
            elements.clear();
            for (size_t i = 0; i < numElements; ++i)
            {
                elements.push_back((float)distUnif(rng));
            }
            Distribution1D_AliasMethod dist(elements);
            for (int32_t i = 0; i < 128; ++i)
            {
                float pdf;
                dist.sample((float)distUnif(rng), &pdf);
            }
        }
    }
    SECTION("strange input")
    {
        // 要素がない場合
        {
            Distribution1D_AliasMethod dist({});
            for (int32_t i = 0; i < 16; ++i)
            {
                float pdf;
                const float offset = dist.sample((float)distUnif(rng), &pdf);
                CHECK(pdf == 1.0f);
                CHECK(offset == 0.0f);
            }
        }
        // 一つだけある要素が0の場合
        {
            Distribution1D_AliasMethod dist({ 0.0 });
            for (int32_t i = 0; i < 16; ++i)
            {
                float pdf;
                int32_t offset;
                dist.sample((float)distUnif(rng), &pdf, &offset);
                CHECK(pdf == 1.0f);
                CHECK(offset == 0);
            }
        }
        // 全ての要素が0の場合
        {
            Distribution1D_AliasMethod dist({ 0.0, 0.0f, 0.0f });
            for (int32_t i = 0; i < 16; ++i)
            {
                float pdf;
                dist.sample((float)distUnif(rng), &pdf);
                CHECK(pdf == 1.0f);
            }
        }
        // 負の要素がある場合
        {
            Distribution1D_AliasMethod dist({ -1.0f, 0.0f, 1.0f });
            for (int32_t i = 0; i < 16; ++i)
            {
                float pdf;
                int32_t offset;
                dist.sample((float)distUnif(rng), &pdf, &offset);
                CHECK(pdf == 3.0f);
                CHECK(offset == 2);
            }
        }
    }
    // ちゃんと確率密度が正しい値が返ってるかをテスト
    SECTION("basic1")
    {
        FloatStreamStats fs;
        const std::vector<float> values = {4.0f,1.0f,1.0f};
        Distribution1D_AliasMethod dist(values);
        for (int32_t i = 0; i < 1024; ++i)
        {
            float pdf;
            int32_t offset0;
            const float u0 = (float)distUnif(rng);
            const float offsetF = dist.sample(u0, &pdf, &offset0);
            const int32_t offset1 = int32_t(offsetF);
            // オフセットは同じ
            CHECK(offset0 == offset1);
            const float value = values[offset0] / pdf;
            // 全て2にならないといけない
            CHECK(value == Approx(2.0f).epsilon(0.001f));
        }
    }

    return;
}


/*
-------------------------------------------------
TestNaiveAndOptimized
Distribution1D_Optimizedが、Naive版と同じ挙動になっているかのチェック
-------------------------------------------------
*/
TEST_CASE("Optimized", "Distribution")
{
    // データの作成
    std::vector<float> datas;
    std::vector<float> samples;
    RNG rng;
    for (int32_t i = 0; i < 57; ++i)
    {
        const float v = (float)rand() / (float)RAND_MAX * 100;
        datas.push_back(v);
    }
    for (int32_t i = 0; i < 1000; ++i)
    {
        samples.push_back(rng.nextFloat());
    }
    //
    Distribution1D_Naive naive(datas);
    Distribution1D_Optimized optimized(datas);
    //
    for (size_t i = 0, n = samples.size(); i < n; ++i)
    {
#if 0
        float pdf0;
        float pdf1;
        volatile float v0 = naive.sample(samples[i], &pdf0);
        volatile float v1 = optimized.sample(samples[i], &pdf1);
        AL_ASSERT_DEBUG(v0 == v1);
        AL_ASSERT_DEBUG(pdf0 == pdf1);
#endif
    }
    return;
}


/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("Dist:: Basic0", "Distribution2d")
{
    // 推定が正しいかチェック
    {
        XorShift128 rng;
        std::vector<std::vector<float>> samples(
            { { { { 0.1f, 0.6f } },{ { 0.6f, 1.1f } } } });
        Distribution2D d2(samples);
        const int32_t NUM_SAMPLE = 1000;
        float total = 0.0f;
        for (int32_t i = 0; i < NUM_SAMPLE; ++i)
        {
            //
            float pdf;
            Vec2 sp;
            d2.sample(rng.nextFloat(), rng.nextFloat(), &pdf, &sp);
            const float v0 = alLerp(samples[0][0], samples[0][1], sp.x());
            const float v1 = alLerp(samples[1][0], samples[1][1], sp.x());
            const float v = alLerp(v0, v1, sp.y());
            total += v / pdf;
        }
        total /= (float)NUM_SAMPLE;
        const float rate = total / 0.6f;
        if (fabsf(rate - 1.0f) > 0.02f)
        {
            return;
        }
    }
    return;
}
