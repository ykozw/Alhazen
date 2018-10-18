//
#include "catch2/catch.hpp"
//
#include "core/errfloat.hpp"
#include "core/unittest.hpp"
#include "core/rng.hpp"

TEST_CASE("ErrFloat:: Basic0", "ErrFloat")
{
    XorShift128 rng(0x123);
    for (int32_t i=0;i<1024;++i)
    {
        const auto genEF = [&]()
        {
            const float v = rng.nextFloat() * 2.0f - 1.0f;
            const float e = rng.nextFloat() * 0.1f;
            return ErrFloat(v, e);
        };
        const auto check = [](ErrFloat& ef, long double ref) -> bool
        {
            return ((long double)(ef.low()) <= ref) && (ref <= (long double)(ef.low()));
        };
        ErrFloat e0 = genEF();
        ErrFloat e1 = genEF();
        long double el0 = double(float(e0));
        long double el1 = double(float(e1));
        REQUIRE(check(e0 + e1, el0 + el1));
        REQUIRE(check(e0 - e1, el0 - el1));
        REQUIRE(check(e0 * e1, el0 * el1));
        REQUIRE(check(e0 / e1, el0 / el1));
    }
}
