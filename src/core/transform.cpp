#include "core/transform.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
Transform::Transform()
{
    toWorld_.identity();
    toWorldDir_.identity();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Transform::Transform(const ObjectProp& objectProp)
{
    // toWorld_行列の作成
    toWorld_ = constructToWorldMatrix(objectProp);
    // 法線の回転用に逆行列の転地を求める
    toWorldDir_ = toWorld_.extract3x3().inversed().transposed();
    // それぞれの逆行列を作成する
    toLocal_ = toWorld_.inversed();
    toLocalDir_ = toWorldDir_.inversed();
    //
    AL_ASSERT_DEBUG(!toWorld_.hasNan());
    AL_ASSERT_DEBUG(!toLocal_.hasNan());
    AL_ASSERT_DEBUG(!toWorldDir_.hasNan());
    AL_ASSERT_DEBUG(!toLocalDir_.hasNan());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Matrix4x4 Transform::constructToWorldMatrix(const ObjectProp& objectProp) const
{
    Matrix4x4 toWorld;
    // 空であれば、Identityのままで終了
    toWorld.identity();
    if (objectProp.isEmpty())
    {
        return toWorld;
    }
    // カテゴリー名が"transform"かチェック
    AL_ASSERT_DEBUG(objectProp.tag() == "transform");

    // 全てのpropを巡回する
    for (const auto& child : objectProp.childProps())
    {
        const std::string tag = child.tag();
        if (tag == "translate")
        {
            Matrix4x4 mat;
            mat.constructAsTranslation(child.asXYZ(Vec3(0.0f, 0.0f, 0.0f)));
            toWorld = Matrix4x4::mul(toWorld, mat);
        }
        else if (tag == "scale")
        {
            /*
            <scale value = "5" / > <!--uniform scale-->
            <scale x = "2" y = "1" z = "-1" / > <!--non - unform scale-->
            の二つの形式があるが、二つ目の形式にのみ対応している
            */
            Matrix4x4 mat;
            mat.constructAsScale(child.asXYZ(Vec3(1.0f, 1.0f, 1.0f)));
            toWorld = Matrix4x4::mul(toWorld, mat);
        }
        else if (tag == "rotate")
        {
            /*
            <rotate x = "0.701" y = "0.701" z = "0" angle = "180" / >
            の形式。
            */
            const Vec3 axis = child.asXYZ(Vec3(1.0f, 0.0f, 0.0f)).normalized();
            const float angleInDegree = child.asAngle(0.0f);
            Matrix4x4 mat;
            mat.constructAsRotation(axis, DEG2RAD(angleInDegree));
            toWorld = Matrix4x4::mul(toWorld, mat);
        }
        else if (tag == "lookat")
        {
            const Vec3 origin =
                child.attribute("origin").asVec3(Vec3(0.0f, 0.0f, 0.0f));
            const Vec3 target =
                child.attribute("target").asVec3(Vec3(1.0f, 1.0f, 1.0f));
            const Vec3 up =
                child.attribute("up").asVec3(Vec3(0.0f, 1.0f, 0.0f));
            Matrix4x4 viewMatrix;
            viewMatrix.constructAsViewMatrix(origin, target, up);
            toWorld = viewMatrix;
        }
        else
        {
            assert(false);
        }
    }
    return toWorld;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const Vec3 Transform::toWorld(Vec3 pos) const
{
    return toWorld_.transform(pos);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const Vec3 Transform::toLocal(Vec3 pos) const
{
    return toLocal_.transform(pos);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const Vec3 Transform::toWorldDir(Vec3 dir) const
{
    return toWorldDir_.transform(dir);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const Vec3 Transform::toLocalDir(Vec3 dir) const
{
    return toLocalDir_.transform(dir);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Vec3 Transform::cameraOrigin() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const Vec3 xa = cameraRight();
    const Vec3 ya = cameraUp();
    const Vec3 za = cameraDir();
    const auto& m = toWorld_;
    const Vec3 org = xa * m.e41 + ya * m.e42 + za * m.e43;
    return -org;
#else
    const Vec3 xa = cameraRight();
    const Vec3 ya = cameraUp();
    const Vec3 za = cameraDir();
    const auto& m = toWorld_;
    const Vec3 org = xa * _mm_extract_ps_fast<0>(m.row3) +
                     ya * _mm_extract_ps_fast<1>(m.row3) +
                     za * _mm_extract_ps_fast<2>(m.row3);
    return -org;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Vec3 Transform::cameraUp() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const auto& m = toWorld_;
    return Vec3(m.e12, m.e22, m.e32);
#else
    const auto& m = toWorld_;
    return Vec3(_mm_extract_ps_fast<1>(m.row0),
                _mm_extract_ps_fast<1>(m.row1),
                _mm_extract_ps_fast<1>(m.row2));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Vec3 Transform::cameraDir() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const auto& m = toWorld_;
    return Vec3(m.e13, m.e23, m.e33);
#else
    const auto& m = toWorld_;
    return Vec3(_mm_extract_ps_fast<2>(m.row0),
                _mm_extract_ps_fast<2>(m.row1),
                _mm_extract_ps_fast<2>(m.row2));
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Vec3 Transform::cameraRight() const
{
#if defined(AL_MATH_USE_NO_SIMD)
    const auto& m = toWorld_;
    return Vec3(m.e11, m.e21, m.e31);
#else
    const auto& m = toWorld_;
    return Vec3(_mm_extract_ps_fast<0>(m.row0),
                _mm_extract_ps_fast<0>(m.row1),
                _mm_extract_ps_fast<0>(m.row2));
#endif
}
