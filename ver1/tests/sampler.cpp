#include "catch2/catch.hpp"
#include "sampler/sampler.hpp"
#include "util/mathmaticautil.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("RadicalInverse", "Sampler")
{
    // radicalInverseSlow()とradicalInverseFast()が同じ結果を返すかチェック
    {
        bool fail = false;
        for (int32_t i = 0; i < 1188; ++i)
        {
            for (int32_t j = 0; j < 1024; ++j)
            {
                const float v0 = radicalInverseSlow(i, j);
                const float v1 = radicalInverseFast(i, j);
                fail |= (fabsf(v0 - v1) > 0.0001f);
            }
        }
        AL_ASSERT_ALWAYS(!fail);
    }
    /*
    ちゃんと一様に分布しているかをプロット
    (実際には高次元の近い次元は次元はかなり相関してくる)
    */
    {
        for (int32_t pi = 0; pi < 5; pi += 1)
        {
            std::vector<Vec2> v;
            for (int32_t i = 0; i < 1000; ++i)
            {
                const float v0 = radicalInverseFast(pi + 0, i);
                const float v1 = radicalInverseFast(pi + 1, i);
                v.push_back(Vec2(v0, v1));
            }
            const std::string fileName = "radical_inverse_" +
                std::to_string(pi + 0) +
                std::to_string(pi + 1) + ".m";
            MathmaticaUtil::writePlotPoint2D(fileName, v);
        }
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("SamplerIndepent", "Sampler")
{
    /*
    ちゃんと一様に分布しているかをプロット
    (実際には高次元の近い次元は次元はかなり相関してくる)
    */
    {
        SamplerIndepent sampler;
        for (uint32_t si = 0; si < 5; ++si)
        {
            std::vector<Vec2> v;
            sampler.setHash(Hash::hash(si + 1));
            for (int32_t i = 0; i < 1000; ++i)
            {
                v.push_back(sampler.get2d());
            }
            const std::string fileName =
                "sampler_independent_" + std::to_string(si) + ".m";
            MathmaticaUtil::writePlotPoint2D(fileName, v);
        }
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("SamplerHalton", "Sampler")
{
    /*
    ちゃんと一様に分布しているかをプロット
    (実際には高次元の近い次元は次元はかなり相関してくる)
    */
    {
        SamplerHalton sampler;
        sampler.setHash(Hash::hash(uint32_t(25)));
        for (uint32_t si = 0; si < 5; ++si)
        {
            std::vector<Vec2> v;
            for (int32_t i = 0; i < 1000; ++i)
            {
                sampler.startSample(i);
                sampler.setDimention(si);
                v.push_back(sampler.get2d());
            }
            const std::string fileName =
                "sampler_halton_" + std::to_string(si + 0) + ".m";
            MathmaticaUtil::writePlotPoint2D(fileName, v);
        }
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("SphericalFibonacci", "Sampler")
{
    /*
    ちゃんと一様に分布しているかをプロット
    (実際には高次元の近い次元は次元はかなり相関してくる)
    */
    {
        ;
        for (uint32_t si = 0; si < 5; ++si)
        {
            std::vector<Vec3> v;
            SphericalFibonacci sampler(Hash::hash(si + 1));
            sampler.prepare(1000);
            for (int32_t i = 0; i < 1000; ++i)
            {
                v.push_back(sampler.sample(i));
            }
            const std::string fileName =
                "sampler_fibonacci_" + std::to_string(si + 0) + ".m";
            MathmaticaUtil::writePlotPoint3D(fileName, v);
        }
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("RadicalInverse2", "Sampler") {}

#if 0
/*
-------------------------------------------------
-------------------------------------------------
*/
template <typename SamplerType> void perfSampler()
{
    SamplerType sampler;
    sampler.setHash(0x123);
    Vec2 dummy(0.0f);
    const auto start = g_timeUtil.elapseTimeInMs();
    for (int32_t sn = 0; sn < 1024 * 32; ++sn)
    {
        sampler.startSample(sn);
        for (int32_t dim = 0; dim < 512; ++dim)
        {
            dummy += sampler.get2d();
        }
    }
    const auto elapse = g_timeUtil.elapseTimeInMs() - start;
    printf("Done %d ms\n", elapse);
}
TEST_CASE(Sampler, perf)
{
    perfSampler<SamplerIndepent>();
    perfSampler<SamplerHalton>();
}
#endif


/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("Halton", "Sampler")
{
#if 0
    std::array<FloatStreamStats, 8> stats;
    SamplerHalton sampler;
    sampler.setHash(1);
    for (int32_t sn = 0; sn<1024; ++sn)
    {
        sampler.startSample(sn);
        // 4次元の球の体積を求める
        const auto isInside = [](Vec2 v0, Vec2 v1) -> float
        {
            const float d0 = std::fabsf(v0.x() - 0.5f);
            const float d1 = std::fabsf(v0.y() - 0.5f);
            const float d2 = std::fabsf(v1.x() - 0.5f);
            const float d3 = std::fabsf(v1.y() - 0.5f);
            const float lenSq = std::sqrtf(d0 * d0 + d1 * d1 + d2 * d2/* + d3 * d3*/);
            const bool isInside = lenSq < 0.5f;
            return isInside ? 8.0f : 0.0f;
        };
        for (auto& stat : stats)
        {
            stat.add(isInside(sampler.get2d(), sampler.get2d()));
        }
    }
    //
    for (auto& stat : stats)
    {
        const float sig = stat.sigma();
        const float mean = stat.mean(); // TODO: sigmaが怪しい気がする
                                        //const float goal = PI * PI * 0.5f;
        const float goal = (4.0f / 3.0f * PI);
        AL_ASSERT_ALWAYS(fabsf(mean / goal - 1.0f) < 0.1f);
    }
#endif
}