#include "catch2/catch.hpp"
#include "core/floatstreamstats.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("FloatStreamStats::Basic0", "FloatStreamStats")
{
    {
        FloatStreamStats<> stats;
        stats.add(13.0f);
        stats.add(23.0f);
        stats.add(12.0f);
        stats.add(44.0f);
        stats.add(55.0f);
        AL_ASSERT_ALWAYS(fabsf(stats.mu() / 29.4f - 1.0f) < 0.1f);
        AL_ASSERT_ALWAYS(fabsf(stats.var() / 370.3f - 1.0f) < 0.1f);
        AL_ASSERT_ALWAYS(fabsf(stats.sigma() / 19.2f - 1.0f) < 0.1f);
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TEST_CASE("FloatStreamStats::Basic1", "FloatStreamStats")
{
    FloatStreamStats<float, FSS_MomentLevel::MuVarSkewKurt> stats;
    stats.add(2.0f);
    stats.add(30.0f);
    stats.add(51.0f);
    stats.add(72.0f);
    const float t0 = stats.mu();
    const float t1 = stats.var();
    const float t2 = stats.skew();
    const float t3 = stats.kurt();

    AL_ASSERT_ALWAYS(fabsf((t0 / +38.75f) - 1.0f) < 0.1f);
    AL_ASSERT_ALWAYS(fabsf((t1 / +894.25f) - 1.0f) < 0.1f);
    AL_ASSERT_ALWAYS(fabsf((t2 / -0.1685f) - 1.0f) < 0.1f);
    AL_ASSERT_ALWAYS(fabsf((t3 / -1.29f) - 1.0f) < 0.1f);

}
