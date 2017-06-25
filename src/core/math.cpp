#include "pch.hpp"
#include "core/unittest.hpp"
#include "core/math.hpp"
#include "core/logging.hpp"

/*
-------------------------------------------------
BoolInVecに関するテストコード
-------------------------------------------------
*/
AL_TEST(Math, BoolInVec)
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
    // 各種代入
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
        v.v = _mm_set_epi32(3, 2, 1, 0);
        AL_ASSERT_ALWAYS(!(bool)v);
    }
    {
        BoolInVec v;
        v.v = _mm_set_epi32(3, 2, 1, 1);
        AL_ASSERT_ALWAYS((bool)v);
    }
}

/*
-------------------------------------------------
FloatInVecに関するテスト
-------------------------------------------------
*/
AL_TEST(Math, FloatInVec)
{
    const float nan = std::numeric_limits<float>::quiet_NaN();
    // 基本的な設定
    {
        FloatInVec v(1.0f);
        AL_ASSERT_ALWAYS(v.value() == 1.0f);
        AL_ASSERT_ALWAYS((float)v == 1.0f);
    }
    // 他のレーンを汚染しても大丈夫かチェック
    {
        FloatInVec v;
        v.v = _mm_set_ps(2.0f, 3.0f, 4.0f, 5.0f);
        AL_ASSERT_ALWAYS(float(v) == 5.0f);
    }
    {
        FloatInVec v;
        v.v = _mm_set_ps(2.0f, 3.0f, 4.0f, nan);
        AL_ASSERT_ALWAYS(v.isNan());
    }
    {
        FloatInVec v;
        v.v = _mm_set_ps(2.0f, 3.0f, nan, 1.0f);
        AL_ASSERT_ALWAYS(!v.isNan());
    }
    // TODO: floatを直接乗算できるようにする
}

/*
-------------------------------------------------
 Vec3に関するテスト
-------------------------------------------------
*/
AL_TEST(Math, Vec3)
{
    {
        Vec3 v(2.0f, 3.0f, 4.0f);
        v.setX(5.0f);
        AL_ASSERT_ALWAYS(v.x() == 5.0f);
        AL_ASSERT_ALWAYS(v.y() == 3.0f);
        AL_ASSERT_ALWAYS(v.z() == 4.0f);
    }
    {
        Vec3 v(2.0f, 3.0f, 4.0f);
        v.setY(5.0f);
        AL_ASSERT_ALWAYS(v.x() == 2.0f);
        AL_ASSERT_ALWAYS(v.y() == 5.0f);
        AL_ASSERT_ALWAYS(v.z() == 4.0f);
    }
    {
        Vec3 v(2.0f, 3.0f, 4.0f);
        v.setZ(5.0f);
        AL_ASSERT_ALWAYS(v.x() == 2.0f);
        AL_ASSERT_ALWAYS(v.y() == 3.0f);
        AL_ASSERT_ALWAYS(v.z() == 5.0f);
    }
    // 全要素同じ設定をしての初期化
    {
        const Vec3 v(1.0f);
        AL_ASSERT_ALWAYS(v.x() == 1.0f);
        AL_ASSERT_ALWAYS(v.y() == 1.0f);
        AL_ASSERT_ALWAYS(v.z() == 1.0f);
    }
    // 配列からの生成
    {
        const float arr[] = { 1.0f,2.0f,3.0f };
        const Vec3 v(arr);
        AL_ASSERT_ALWAYS(v.x() == 1.0f);
        AL_ASSERT_ALWAYS(v.y() == 2.0f);
        AL_ASSERT_ALWAYS(v.z() == 3.0f);
    }
    // 同値判定
    {
        // 同値
        AL_ASSERT_ALWAYS(Vec3(1.0f,2.0f,3.0f) ==
                         Vec3(1.0f,2.0f,3.0f));
        // 非同値
        AL_ASSERT_ALWAYS(Vec3(0.0f,2.0f,3.0f) !=
                         Vec3(1.0f,2.0f,3.0f));
        AL_ASSERT_ALWAYS(Vec3(1.0f,0.0f,3.0f) !=
                         Vec3(1.0f,2.0f,3.0f));
        AL_ASSERT_ALWAYS(Vec3(1.0f,2.0f,0.0f) !=
                         Vec3(1.0f,2.0f,3.0f));
        // wは関係がないので同値になる
        AL_ASSERT_ALWAYS(Vec3(_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)) ==
                         Vec3(_mm_set_ps(0.0f, 0.0f, 0.0f, 0.0f)));
    }
    // zero
    {
        Vec3 v;
        v.zero();
        AL_ASSERT_ALWAYS(v.x() == 0.0f);
        AL_ASSERT_ALWAYS(v.y() == 0.0f);
        AL_ASSERT_ALWAYS(v.z() == 0.0f);
    }
    // swizzle
    {
        const float x = 0.0f;
        const float y = 1.0f;
        const float z = 2.0f;
        const Vec3 v(x, y, z);
        AL_ASSERT_ALWAYS(v.xxx() == Vec3(x, x, x));
        AL_ASSERT_ALWAYS(v.zyx() == Vec3(z, y, x));
    }
    // hasNan
    {
        const float v = 1.0f;
        const float n = std::numeric_limits<float>::quiet_NaN();
        AL_ASSERT_ALWAYS(Vec3(n, v, v).hasNan());
        AL_ASSERT_ALWAYS(Vec3(v, n, v).hasNan());
        AL_ASSERT_ALWAYS(Vec3(v, v, n).hasNan());
        // w成分は関係がないのでNaNになってはいけない
        AL_ASSERT_ALWAYS(!Vec3(_mm_set_ps(n, v, v, v)).hasNan());
    }
    // any
    {
        AL_ASSERT_ALWAYS(!Vec3(0.0f, 0.0f, 0.0f).any());
        AL_ASSERT_ALWAYS(Vec3(0.0f, 1.0f, 0.0f).any());
        AL_ASSERT_ALWAYS(Vec3(0.0f, 0.0f, 1.0f).any());
        // w成分は関係がないのでany()の結果には影響を与えてはいけない
        AL_ASSERT_ALWAYS(!Vec3(_mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f)).any());
    }
    // all
    {
        AL_ASSERT_ALWAYS(!Vec3(0.0f, 0.0f, 0.0f).all());
        AL_ASSERT_ALWAYS(!Vec3(1.0f, 0.0f, 0.0f).all());
        AL_ASSERT_ALWAYS(!Vec3(1.0f, 1.0f, 0.0f).all());
        AL_ASSERT_ALWAYS(!Vec3(0.0f, 1.0f, 1.0f).all());
        AL_ASSERT_ALWAYS(Vec3(1.0f, 1.0f, 1.0f).all());
        // w成分は関係がないのでall()の結果には影響を与えてはいけない
        AL_ASSERT_ALWAYS(Vec3(_mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f)).all());
    }
    const auto sameV = [](float v0, float v1)
    {
        const float eps = 0.00001f;
        return std::fabsf(v0 - v1) < eps;
    };
    const auto same1 = [&sameV](float v)
    {
        return sameV(v, 1.0f);
    };
    // normalize(1)
    {
        
        Vec3 v0(1.0f, 2.0f, 3.0f);
        const Vec3 v1 = v0.normalized();
        v0.normalize();
        AL_ASSERT_ALWAYS(v1 == v0);
        AL_ASSERT_ALWAYS(same1(v0.length()));
        AL_ASSERT_ALWAYS(same1(v1.length()));
        AL_ASSERT_ALWAYS(v0.isNormalized());
        AL_ASSERT_ALWAYS(v1.isNormalized());
        AL_ASSERT_ALWAYS(same1(v0.length()));
        AL_ASSERT_ALWAYS(same1(v0.lengthSq()));
        AL_ASSERT_ALWAYS(same1(v1.length()));
        AL_ASSERT_ALWAYS(same1(v1.lengthSq()));
    }
    // normalize(2)
    {
        const Vec3 v0(_mm_set_ps(1.0f,0.0f,0.0f,1.0f));
        const Vec3 v1 = v0.normalized();
        AL_ASSERT_ALWAYS(same1(v1.x()) && (v1.y() == 0.0f) && (v1.z() == 0.0f));
        AL_ASSERT_ALWAYS(v1.isNormalized());
    }
    // scale
    {
        
    }
    // TODO: inverted/invertedSafe
    // TODO: reflect
    // TODO: operator[]
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
AL_TEST(Math, Matrix3x3)
{
    // TODO: 実装
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(Math, Matrix4x4)
{
    // TODO: 実装
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
AL_TEST(Math, AABB)
{
    // TODO: 実装
}

/*
 -------------------------------------------------
 Vec4に関するテスト
 -------------------------------------------------
 */
AL_TEST(Math, V4)
{
    // 初期設定テスト
    {
        const Vec4 v(1.0f);
        AL_ASSERT_ALWAYS(v.x() == 1.0f);
        AL_ASSERT_ALWAYS(v.y() == 1.0f);
        AL_ASSERT_ALWAYS(v.z() == 1.0f);
        AL_ASSERT_ALWAYS(v.w() == 1.0f);
    }
    {
        const Vec4 v(0.0f, 1.0f, 2.0f, 3.0f);
        AL_ASSERT_ALWAYS(v.x() == 0.0f);
        AL_ASSERT_ALWAYS(v.y() == 1.0f);
        AL_ASSERT_ALWAYS(v.z() == 2.0f);
        AL_ASSERT_ALWAYS(v.w() == 3.0f);
    }
#if 0
    // 長さ
    {
        Vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
        AL_ASSERT_ALWAYS(v.lengthSq() == 30.0f);
        AL_ASSERT_ALWAYS(std::fabsf(v.length() - 5.477225575f));
    }
#endif
    // TODO: swizzleのテストコード
    {
        const float x = 1.0f;
        const float y = 2.0f;
        const float z = 3.0f;
        const float w = 4.0f;
        const Vec4 v(x, y, z, w);
#if 0
        AL_ASSERT_ALWAYS(v.xxxx() == Vec4(x, x, x, x));
        AL_ASSERT_ALWAYS(v.yxxx() == Vec4(y, x, x, x));
        AL_ASSERT_ALWAYS(v.zxxx() == Vec4(z, x, x, x));
        AL_ASSERT_ALWAYS(v.wxxx() == Vec4(y, x, x, x));
        AL_ASSERT_ALWAYS(v.xyxx() == Vec4(x, y, x, x));
        AL_ASSERT_ALWAYS(v.yyxx() == Vec4(y, y, x, x));
        AL_ASSERT_ALWAYS(v.zyxx() == Vec4(z, y, x, x));
        AL_ASSERT_ALWAYS(v.wyxx() == Vec4(y, y, x, x));
        AL_ASSERT_ALWAYS(v.xzxx() == Vec4(x, z, x, x));
        AL_ASSERT_ALWAYS(v.yzxx() == Vec4(y, z, x, x));
        AL_ASSERT_ALWAYS(v.zzxx() == Vec4(z, z, x, x));
        AL_ASSERT_ALWAYS(v.wzxx() == Vec4(y, z, x, x));
        AL_ASSERT_ALWAYS(v.xwxx() == Vec4(x, w, x, x));
        AL_ASSERT_ALWAYS(v.ywxx() == Vec4(y, w, x, x));
        AL_ASSERT_ALWAYS(v.zwxx() == Vec4(z, w, x, x));
        AL_ASSERT_ALWAYS(v.wwxx() == Vec4(y, w, x, x));

        // TODO: 参列目をyにするところから続き
        AL_ASSERT_ALWAYS(v.xxxx() == Vec4(x, x, x, x));
        AL_ASSERT_ALWAYS(v.yxxx() == Vec4(y, x, x, x));
        AL_ASSERT_ALWAYS(v.zxxx() == Vec4(z, x, x, x));
        AL_ASSERT_ALWAYS(v.wxxx() == Vec4(y, x, x, x));
        AL_ASSERT_ALWAYS(v.xyxx() == Vec4(x, y, x, x));
        AL_ASSERT_ALWAYS(v.yyxx() == Vec4(y, y, x, x));
        AL_ASSERT_ALWAYS(v.zyxx() == Vec4(z, y, x, x));
        AL_ASSERT_ALWAYS(v.wyxx() == Vec4(y, y, x, x));
        AL_ASSERT_ALWAYS(v.xzxx() == Vec4(x, z, x, x));
        AL_ASSERT_ALWAYS(v.yzxx() == Vec4(y, z, x, x));
        AL_ASSERT_ALWAYS(v.zzxx() == Vec4(z, z, x, x));
        AL_ASSERT_ALWAYS(v.wzxx() == Vec4(y, z, x, x));
        AL_ASSERT_ALWAYS(v.xwxx() == Vec4(x, w, x, x));
        AL_ASSERT_ALWAYS(v.ywxx() == Vec4(y, w, x, x));
        AL_ASSERT_ALWAYS(v.zwxx() == Vec4(z, w, x, x));
        AL_ASSERT_ALWAYS(v.wwxx() == Vec4(y, w, x, x));
#endif
    }
}
