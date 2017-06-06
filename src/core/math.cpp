#include "pch.hpp"
#include "core/unittest.hpp"
#include "core/math.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(Math, Vec3)
{
    // 
    {
        const Vec3 v(1.0f);
        AL_TEST_CHECK(v.x == 1.0f);
        AL_TEST_CHECK(v.y == 1.0f);
        AL_TEST_CHECK(v.z == 1.0f);
    }
    // 
    {
        const float arr[] = { 1.0f,2.0f,3.0f };
        const Vec3 v(arr);
        AL_TEST_CHECK(v.x == 1.0f);
        AL_TEST_CHECK(v.y == 2.0f);
        AL_TEST_CHECK(v.z == 3.0f);
    }
    // zero
    {
        Vec3 v;
        v.zero();
        AL_TEST_CHECK(v.x == 0.0f);
        AL_TEST_CHECK(v.y == 0.0f);
        AL_TEST_CHECK(v.z == 0.0f);
    }
    // swizzle
    {
        const float x = 0.0f;
        const float y = 1.0f;
        const float z = 2.0f;
        const Vec3 v(x, y, z);
        AL_TEST_CHECK(v.xxx() == Vec3(x, x, x));
        AL_TEST_CHECK(v.xxy() == Vec3(x, x, y));
        AL_TEST_CHECK(v.xxz() == Vec3(x, x, z));
        AL_TEST_CHECK(v.xyx() == Vec3(x, y, x));
        AL_TEST_CHECK(v.xyy() == Vec3(x, y, y));
        //AL_TEST_CHECK(v.xyz() == Vec3(x, x, z));
        AL_TEST_CHECK(v.xzx() == Vec3(x, z, x));
        AL_TEST_CHECK(v.xzy() == Vec3(x, z, y));
        AL_TEST_CHECK(v.xzz() == Vec3(x, z, z));
        AL_TEST_CHECK(v.yxx() == Vec3(y, x, x));
        AL_TEST_CHECK(v.yxy() == Vec3(y, x, y));
        AL_TEST_CHECK(v.yxz() == Vec3(y, x, z));
        AL_TEST_CHECK(v.yyx() == Vec3(y, y, x));
        AL_TEST_CHECK(v.yyy() == Vec3(y, y, y));
        AL_TEST_CHECK(v.yyz() == Vec3(y, y, z));
        AL_TEST_CHECK(v.yzx() == Vec3(y, z, x));
        AL_TEST_CHECK(v.yzy() == Vec3(y, z, y));
        AL_TEST_CHECK(v.yzz() == Vec3(y, z, z));
        AL_TEST_CHECK(v.zxx() == Vec3(z, x, x));
        AL_TEST_CHECK(v.zxy() == Vec3(z, x, y));
        AL_TEST_CHECK(v.zxz() == Vec3(z, x, z));
        AL_TEST_CHECK(v.zyx() == Vec3(z, y, x));
        AL_TEST_CHECK(v.zyy() == Vec3(z, y, y));
        AL_TEST_CHECK(v.zyz() == Vec3(z, y, z));
        AL_TEST_CHECK(v.zzx() == Vec3(z, z, x));
        AL_TEST_CHECK(v.zzy() == Vec3(z, z, y));
        AL_TEST_CHECK(v.zzz() == Vec3(z, z, z));
    }
}
