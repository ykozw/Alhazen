#include "pch.hpp"
#include "core/unittest.hpp"
#include "core/math.hpp"
#include "core/logging.hpp"

namespace {
    /*
    -------------------------------------------------
    テスト用のほぼ等しいかを判定
    -------------------------------------------------
    */
    const auto sameV = [](float v0, float v1)
    {
        const float eps = 0.00001f;
        return std::fabsf(v0 - v1) < eps;
    };
    //
    const float NaN = std::numeric_limits<float>::quiet_NaN();
    const float INF = std::numeric_limits<float>::infinity();
}

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
        BoolInVec v(_mm_set1_epi32(2));
        AL_ASSERT_ALWAYS((bool)v);
    }
    // 他のレーンを汚した状態での真偽チェック
    {
        BoolInVec v(_mm_set_epi32(3, 2, 1, 0));
        AL_ASSERT_ALWAYS(!(bool)v);
    }
    {
        BoolInVec v(_mm_set_epi32(3, 2, 1, 1));
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
        FloatInVec v(_mm_set_ps(2.0f, 3.0f, 4.0f, 5.0f));
        AL_ASSERT_ALWAYS(float(v) == 5.0f);
    }
    {
        FloatInVec v(_mm_set_ps(2.0f, 3.0f, 4.0f, nan));
        AL_ASSERT_ALWAYS(v.isNan());
    }
    {
        FloatInVec v(_mm_set_ps(2.0f, 3.0f, nan, 1.0f));
        AL_ASSERT_ALWAYS(!v.isNan());
    }
    // TODO: floatを直接乗算できるようにする
}

/*
 -------------------------------------------------
 Vec2に関するテスト
 -------------------------------------------------
 */
AL_TEST(Math, Vec2)
{
    {
        Vec2 v(2.0f, 3.0f);
        v.setX(5.0f);
        AL_ASSERT_ALWAYS(v.x() == 5.0f);
        AL_ASSERT_ALWAYS(v.y() == 3.0f);
    }
    {
        Vec2 v(2.0f, 3.0f);
        v.setY(5.0f);
        AL_ASSERT_ALWAYS(v.x() == 2.0f);
        AL_ASSERT_ALWAYS(v.y() == 5.0f);
    }
    // 全要素同じ設定をしての初期化
    {
        const Vec2 v(1.0f);
        AL_ASSERT_ALWAYS(v.x() == 1.0f);
        AL_ASSERT_ALWAYS(v.y() == 1.0f);
    }
    // 配列からの生成
    {
        const float arr[] = { 1.0f,2.0f };
        const Vec3 v(arr);
        AL_ASSERT_ALWAYS(v.x() == 1.0f);
        AL_ASSERT_ALWAYS(v.y() == 2.0f);
    }
    // 同値判定
    {
        // 同値
        AL_ASSERT_ALWAYS(Vec2(1.0f,2.0f) ==
                         Vec2(1.0f,2.0f));
        // 非同値
        AL_ASSERT_ALWAYS(Vec2(0.0f,2.0f) !=
                         Vec2(1.0f,2.0f));
        AL_ASSERT_ALWAYS(Vec2(1.0f,0.0f) !=
                         Vec2(1.0f,2.0f));
        // zwは関係がないので同値になる
        AL_ASSERT_ALWAYS(Vec2(_mm_set_ps(0.0f, 3.0f, 2.0f, 1.0f)) ==
                         Vec2(_mm_set_ps(0.0f, 0.0f, 2.0f, 1.0f)));
        AL_ASSERT_ALWAYS(Vec2(_mm_set_ps(4.0f, 0.0f, 2.0f, 1.0f)) ==
                         Vec2(_mm_set_ps(0.0f, 0.0f, 2.0f, 1.0f)));
        
    }
    // normalize/length
    {
        Vec2 v0 = Vec2(0.1f,0.0f);
        const Vec2 v1 = v0.normalized();
        AL_ASSERT_ALWAYS(sameV(v0.x(), 0.1f));
        AL_ASSERT_ALWAYS(sameV(v0.y(), 0.0f));
        AL_ASSERT_ALWAYS(sameV(v1.length(),1.0f));
        AL_ASSERT_ALWAYS(sameV(v1.x(), 1.0f));
        AL_ASSERT_ALWAYS(sameV(v1.y(), 0.0f));
        v0.normalize();
        AL_ASSERT_ALWAYS(sameV(v0.length(),1.0f));
        //
        const Vec2 v2 = Vec2(12.0f,34.0f).normalized();
        AL_ASSERT_ALWAYS(sameV(v2.length(), 1.0f));
    }
    // any
    {
        AL_ASSERT_ALWAYS(Vec2(0.0f,1.0f).any());
        AL_ASSERT_ALWAYS(Vec2(1.0f,0.1f).any());
        AL_ASSERT_ALWAYS(!Vec2(0.0f,0.0f).any());
        AL_ASSERT_ALWAYS(!Vec2(_mm_set_ps(0.0f,1.0f,0.0f,0.0f)).any());
        AL_ASSERT_ALWAYS(!Vec2(_mm_set_ps(1.0f,0.0f,0.0f,0.0f)).any());
    }
    // all
    {
        AL_ASSERT_ALWAYS(!Vec2(0.0f,0.0f).all());
        AL_ASSERT_ALWAYS(!Vec2(0.0f,1.0f).all());
        AL_ASSERT_ALWAYS(Vec2(1.0f,1.0f).all());
        AL_ASSERT_ALWAYS(Vec2(_mm_set_ps(0.0f,0.0f,1.0f,1.0f)).all());
        AL_ASSERT_ALWAYS(!Vec2(_mm_set_ps(0.0f,1.0f,0.0f,0.0f)).all());
        AL_ASSERT_ALWAYS(!Vec2(_mm_set_ps(1.0f,0.0f,0.0f,0.0f)).all());
    }
    // hasNaN
    {
        AL_ASSERT_ALWAYS(Vec2(NaN,0.0f).hasNaN());
        AL_ASSERT_ALWAYS(Vec2(0.0f,NaN).hasNaN());
        AL_ASSERT_ALWAYS(!Vec2(0.0f,INF).hasNaN());
    }
    // dot
    {
        sameV(Vec2::dot(Vec2(1.0f,2.0f),Vec2(1.0f,2.0f)), 5.0f);
    }
    // min
    {
        const Vec2 mn = Vec2::min(Vec2(1.0f,3.0f),Vec2(2.0f,2.0f));
        sameV(mn.x(), 1.0f);
        sameV(mn.y(), 2.0f);
    }
    // max
    {
        const Vec2 mx = Vec2::max(Vec2(1.0f,3.0f),Vec2(2.0f,2.0f));
        sameV(mx.x(), 2.0f);
        sameV(mx.y(), 3.0f);
    }
}
/*
-------------------------------------------------
 Vec3に関するテスト
-------------------------------------------------
*/
AL_TEST(Math, Vec3)
{
    // コンストラクタと設定
    {
        ALIGN16 const float e[3] = {2.0f, 3.0f, 4.0f};
        Vec3 v(e);
        AL_ASSERT_ALWAYS(v.x() == 2.0f);
        AL_ASSERT_ALWAYS(v.y() == 3.0f);
        AL_ASSERT_ALWAYS(v.z() == 4.0f);
    }
    {
        Vec3 v(2.0f,3.0f,4.0f);
        AL_ASSERT_ALWAYS(v.x() == 2.0f);
        AL_ASSERT_ALWAYS(v.y() == 3.0f);
        AL_ASSERT_ALWAYS(v.z() == 4.0f);
    }
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
        AL_ASSERT_ALWAYS(Vec3(NaN, v, v).hasNan());
        AL_ASSERT_ALWAYS(Vec3(v, NaN, v).hasNan());
        AL_ASSERT_ALWAYS(Vec3(v, NaN, NaN).hasNan());
        // w成分は関係がないのでNaNになってはいけない
        AL_ASSERT_ALWAYS(!Vec3(_mm_set_ps(NaN, v, v, v)).hasNan());
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
    // normalize(1)
    {
        Vec3 v0(1.0f, 2.0f, 3.0f);
        const Vec3 v1 = v0.normalized();
        v0.normalize();
        AL_ASSERT_ALWAYS(v1 == v0);
        sameV(v0.length(), 1.0f);
        sameV(v1.length(), 1.0f);
        AL_ASSERT_ALWAYS(v0.isNormalized());
        AL_ASSERT_ALWAYS(v1.isNormalized());
        sameV(v0.length(), 1.0f);
        sameV(v0.lengthSq(), 1.0f);
        sameV(v1.length(), 1.0f);
        sameV(v1.lengthSq(), 1.0f);
    }
    // normalize(2)
    {
        const Vec3 v0(_mm_set_ps(1.0f,0.0f,0.0f,1.0f));
        const Vec3 v1 = v0.normalized();
        AL_ASSERT_ALWAYS(sameV(v1.x(),1.0f) && (v1.y() == 0.0f) && (v1.z() == 0.0f));
        AL_ASSERT_ALWAYS(v1.isNormalized());
    }
    // scale
    {
        Vec3 v0(1.0f,2.0f,3.0f);
        v0.scale(2.0f);
        sameV(v0.x(),2.0f);
        sameV(v0.y(),3.0f);
        sameV(v0.z(),6.0f);
    }
    // inverted/invertedSafe
    {
        Vec3 v0(1.0f,2.0f,3.0f);
        const Vec3 v1 = v0.inverted();
        v0.invert();
        AL_ASSERT_ALWAYS(v0 == v1);
        sameV(v0.x(),1.0f/1.0f);
        sameV(v0.y(),1.0f/2.0f);
        sameV(v0.z(),1.0f/3.0f);
        //
        Vec3 v2(0.0f,1.0f,2.0f);
        v2 = v2.invertedSafe(123.0f);
        sameV(v2.x(),123.0f);
        sameV(v2.y(),1.0f/2.0f);
        sameV(v2.z(),1.0f/3.0f);
    }
    // reflect
    {
        const Vec3 v0(1.0f,-1.0f,0.0f);
        const Vec3 v1(0.0f,1.0f,0.0f);
        const Vec3 v2 = v0.reflect(v1);
        sameV(v2.x(),1.0f);
        sameV(v2.y(),1.0f);
        sameV(v2.z(),0.0f);
    }
    // operator[]
    {
        const Vec3 v0(1.0f,2.0f,3.0f);
        sameV(v0[0],1.0f);
        sameV(v0[1],2.0f);
        sameV(v0[2],3.0f);
    }
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
AL_TEST(Math, Matrix3x3_next)
{
    // transpose
    {
        const Matrix3x3 m0(
            Vec3(0.0f, 1.0f, 2.0f),
            Vec3(3.0f, 4.0f, 5.0f),
            Vec3(6.0f, 7.0f, 8.0f));
        // Matrix3x3 m1 = m0.transposed();
        // TODO: 値を取り出せるようにする
        // TODO: Matrix3x3_nextの同値を実装する
    }
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
    // 長さ
    {
        const float x = 1.0f;
        const float y = 2.0f;
        const float z = 3.0f;
        const float w = 4.0f;
        Vec4 v(x, y, z, w);
        const float lenSq =x*x + y*y + z*z + w*w;
        const float len = std::sqrtf(lenSq);
        sameV(v.lengthSq(),lenSq);
        sameV(v.length(), len);
    }
    // swizzleのテストコード
    {
        const float x = 1.0f;
        const float y = 2.0f;
        const float z = 3.0f;
        const float w = 4.0f;
        const Vec4 v(x, y, z, w);
        //
#if 0
        AL_ASSERT_ALWAYS(v.xxxx() == Vec4(x, x, x, x));
        AL_ASSERT_ALWAYS(v.yxxx() == Vec4(y, x, x, x));
        AL_ASSERT_ALWAYS(v.zxxx() == Vec4(z, x, x, x));
        AL_ASSERT_ALWAYS(v.wxxx() == Vec4(w, x, x, x));
        AL_ASSERT_ALWAYS(v.xyxx() == Vec4(x, y, x, x));
        AL_ASSERT_ALWAYS(v.yyxx() == Vec4(y, y, x, x));
        AL_ASSERT_ALWAYS(v.zyxx() == Vec4(z, y, x, x));
        AL_ASSERT_ALWAYS(v.wyxx() == Vec4(w, y, x, x));
        AL_ASSERT_ALWAYS(v.xzxx() == Vec4(x, z, x, x));
        AL_ASSERT_ALWAYS(v.yzxx() == Vec4(y, z, x, x));
        AL_ASSERT_ALWAYS(v.zzxx() == Vec4(z, z, x, x));
        AL_ASSERT_ALWAYS(v.wzxx() == Vec4(w, z, x, x));
        AL_ASSERT_ALWAYS(v.xwxx() == Vec4(x, w, x, x));
        AL_ASSERT_ALWAYS(v.ywxx() == Vec4(y, w, x, x));
        AL_ASSERT_ALWAYS(v.zwxx() == Vec4(z, w, x, x));
        AL_ASSERT_ALWAYS(v.wwxx() == Vec4(w, w, x, x));
        //
        AL_ASSERT_ALWAYS(v.xxyx() == Vec4(x, x, y, x));
        AL_ASSERT_ALWAYS(v.yxyx() == Vec4(y, x, y, x));
        AL_ASSERT_ALWAYS(v.zxyx() == Vec4(z, x, y, x));
        AL_ASSERT_ALWAYS(v.wxyx() == Vec4(w, x, y, x));
        AL_ASSERT_ALWAYS(v.xyyx() == Vec4(x, y, y, x));
        AL_ASSERT_ALWAYS(v.yyyx() == Vec4(y, y, y, x));
        AL_ASSERT_ALWAYS(v.zyyx() == Vec4(z, y, y, x));
        AL_ASSERT_ALWAYS(v.wyyx() == Vec4(w, y, y, x));
        AL_ASSERT_ALWAYS(v.xzyx() == Vec4(x, z, y, x));
        AL_ASSERT_ALWAYS(v.yzyx() == Vec4(y, z, y, x));
        AL_ASSERT_ALWAYS(v.zzyx() == Vec4(z, z, y, x));
        AL_ASSERT_ALWAYS(v.wzyx() == Vec4(w, z, y, x));
        AL_ASSERT_ALWAYS(v.xwyx() == Vec4(x, w, y, x));
        AL_ASSERT_ALWAYS(v.ywyx() == Vec4(y, w, y, x));
        AL_ASSERT_ALWAYS(v.zwyx() == Vec4(z, w, y, x));
        AL_ASSERT_ALWAYS(v.wwyx() == Vec4(w, w, y, x));
        //
        AL_ASSERT_ALWAYS(v.xxxy() == Vec4(x, x, x, y));
        AL_ASSERT_ALWAYS(v.yxxy() == Vec4(y, x, x, y));
        AL_ASSERT_ALWAYS(v.zxxy() == Vec4(z, x, x, y));
        AL_ASSERT_ALWAYS(v.wxxy() == Vec4(w, x, x, y));
        AL_ASSERT_ALWAYS(v.xyxy() == Vec4(x, y, x, y));
        AL_ASSERT_ALWAYS(v.yyxy() == Vec4(y, y, x, y));
        AL_ASSERT_ALWAYS(v.zyxy() == Vec4(z, y, x, y));
        AL_ASSERT_ALWAYS(v.wyxy() == Vec4(w, y, x, y));
        AL_ASSERT_ALWAYS(v.xzxy() == Vec4(x, z, x, y));
        AL_ASSERT_ALWAYS(v.yzxy() == Vec4(y, z, x, y));
        AL_ASSERT_ALWAYS(v.zzxy() == Vec4(z, z, x, y));
        AL_ASSERT_ALWAYS(v.wzxy() == Vec4(w, z, x, y));
        AL_ASSERT_ALWAYS(v.xwxy() == Vec4(x, w, x, y));
        AL_ASSERT_ALWAYS(v.ywxy() == Vec4(y, w, x, y));
        AL_ASSERT_ALWAYS(v.zwxy() == Vec4(z, w, x, y));
        AL_ASSERT_ALWAYS(v.wwxy() == Vec4(w, w, x, y));
#endif
    }
}
