#include "pch.hpp"
#include "core/unittest.hpp"
#include "core/math.hpp"

/*
-------------------------------------------------
BoolInVecに関するテストコード
-------------------------------------------------
*/
AL_TEST_IMM(Math, BoolInVec)
{
    // 基本的な真偽の設定
    {
        BoolInVec v(false);
        AL_ASSERT_ALWAYS(!v.value());
        AL_ASSERT_ALWAYS(!(bool)v);
    }
    {
        BoolInVec v(true);
        AL_ASSERT_ALWAYS(v.value());
        AL_ASSERT_ALWAYS((bool)v);
    }
    // 各種代g入
    {
        BoolInVec v0(false);
        BoolInVec v1(v0);
        AL_ASSERT_ALWAYS(!(bool)v1);
    }
    {
        BoolInVec v0(true);
        BoolInVec v1(v0);
        AL_ASSERT_ALWAYS((bool)v1);
    }
    {
        BoolInVec v0(false);
        BoolInVec v1(v0.v);
        AL_ASSERT_DEBUG(!(bool)v1);
    }
    {
        BoolInVec v0(true);
        BoolInVec v1(v0.v);
        AL_ASSERT_DEBUG((bool)v1);
    }
    // 非ゼロのtrueがちゃんと判定されるかを入れる
    {
        BoolInVec v;
        v.v = _mm_set1_epi32(2);
        AL_ASSERT_ALWAYS((bool)v);
    }
    // 他のレーンを汚した状態での真偽チェック
    {
        BoolInVec v;
        v.v = _mm_set_epi32(3,2,1,0);
        AL_ASSERT_ALWAYS(!(bool)v);
    }
    {
        BoolInVec v;
        v.v = _mm_set_epi32(3,2,1,1);
        AL_ASSERT_ALWAYS((bool)v);
    }
}

/*
-------------------------------------------------
FloatInVecに関するテスト
-------------------------------------------------
*/
AL_TEST_IMM(Math, FloatInVec)
{
    // 基本的な設定
    {
        FloatInVec v(1.0f);
        AL_ASSERT_ALWAYS(v.value() == 1.0f);
        AL_ASSERT_ALWAYS((float)v == 1.0f);
    }
}

/*
-------------------------------------------------
 Vec3に関するテスト
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

// Matrix3x3に関するテスト
// Matrix4x4に関するテスト
// AABBに関するテスト
// Vec4に関するテスト
/*
 -------------------------------------------------
 Vec4に関するテスト
 -------------------------------------------------
 */
AL_TEST_IMM(Math,V4)
{
    // 初期設定テスト
    {
        const Vec4 v(1.0f);
        AL_TEST_CHECK(v.x == 1.0f);
        AL_TEST_CHECK(v.y == 1.0f);
        AL_TEST_CHECK(v.z == 1.0f);
        AL_TEST_CHECK(v.w == 1.0f);
    }
    {
        const Vec4 v(0.0f,1.0f,2.0f,3.0f);
        AL_TEST_CHECK(v.x == 0.0f);
        AL_TEST_CHECK(v.y == 1.0f);
        AL_TEST_CHECK(v.z == 2.0f);
        AL_TEST_CHECK(v.w == 3.0f);
    }
#if 0
    // 長さ
    {
        Vec4 v(1.0f,2.0f,3.0f,4.0f);
        AL_TEST_CHECK(v.lengthSq() == 30.0f);
        AL_TEST_CHECK(std::fabsf(v.length()-5.477225575f));
    }
#endif
    // TODO: swizzleのテストコード
    {
        const float x = 1.0f;
        const float y = 2.0f;
        const float z = 3.0f;
        const float w = 4.0f;
        const Vec4 v(x,y,z,w);
        AL_TEST_CHECK(v.xxxx() == Vec4(x,x,x,x));
        AL_TEST_CHECK(v.yxxx() == Vec4(y,x,x,x));
        AL_TEST_CHECK(v.zxxx() == Vec4(z,x,x,x));
        AL_TEST_CHECK(v.wxxx() == Vec4(y,x,x,x));
        AL_TEST_CHECK(v.xyxx() == Vec4(x,y,x,x));
        AL_TEST_CHECK(v.yyxx() == Vec4(y,y,x,x));
        AL_TEST_CHECK(v.zyxx() == Vec4(z,y,x,x));
        AL_TEST_CHECK(v.wyxx() == Vec4(y,y,x,x));
        AL_TEST_CHECK(v.xzxx() == Vec4(x,z,x,x));
        AL_TEST_CHECK(v.yzxx() == Vec4(y,z,x,x));
        AL_TEST_CHECK(v.zzxx() == Vec4(z,z,x,x));
        AL_TEST_CHECK(v.wzxx() == Vec4(y,z,x,x));
        AL_TEST_CHECK(v.xwxx() == Vec4(x,w,x,x));
        AL_TEST_CHECK(v.ywxx() == Vec4(y,w,x,x));
        AL_TEST_CHECK(v.zwxx() == Vec4(z,w,x,x));
        AL_TEST_CHECK(v.wwxx() == Vec4(y,w,x,x));
        
        // TODO: 参列目をyにするところから続き
        AL_TEST_CHECK(v.xxxx() == Vec4(x,x,x,x));
        AL_TEST_CHECK(v.yxxx() == Vec4(y,x,x,x));
        AL_TEST_CHECK(v.zxxx() == Vec4(z,x,x,x));
        AL_TEST_CHECK(v.wxxx() == Vec4(y,x,x,x));
        AL_TEST_CHECK(v.xyxx() == Vec4(x,y,x,x));
        AL_TEST_CHECK(v.yyxx() == Vec4(y,y,x,x));
        AL_TEST_CHECK(v.zyxx() == Vec4(z,y,x,x));
        AL_TEST_CHECK(v.wyxx() == Vec4(y,y,x,x));
        AL_TEST_CHECK(v.xzxx() == Vec4(x,z,x,x));
        AL_TEST_CHECK(v.yzxx() == Vec4(y,z,x,x));
        AL_TEST_CHECK(v.zzxx() == Vec4(z,z,x,x));
        AL_TEST_CHECK(v.wzxx() == Vec4(y,z,x,x));
        AL_TEST_CHECK(v.xwxx() == Vec4(x,w,x,x));
        AL_TEST_CHECK(v.ywxx() == Vec4(y,w,x,x));
        AL_TEST_CHECK(v.zwxx() == Vec4(z,w,x,x));
        AL_TEST_CHECK(v.wwxx() == Vec4(y,w,x,x));
    }
}












