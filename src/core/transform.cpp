#include "pch.hpp"
#include "core/transform.hpp"

//-------------------------------------------------
// Transform
//-------------------------------------------------
Transform::Transform()
{
    toWorld_.identity();
    toWorldDir_.identity();
}

//-------------------------------------------------
// Transform
//-------------------------------------------------
Transform::Transform(const ObjectProp& objectProp)
{
    // toWorld_行列の作成
    toWorld_ = constructToWorldMatrix(objectProp);
    // 法線の回転用に逆行列の転地を求める
    toWorldDir_ = Matrix4x4(toWorld_.extract3x3().inversed().transposed());
    // それぞれの逆行列を作成する
    toLocal_ = toWorld_.inversed();
    toLocalDir_ = toWorldDir_.inversed();
}

//-------------------------------------------------
// constructToWorldMatrix
//-------------------------------------------------
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
            const Vec3 xyz = child.asXYZ(Vec3(1.0f, 0.0f, 0.0f));
            const float angleInDegree = child.asAngle(0.0f);
            Matrix4x4 mat;
            mat.constructAsRotation(xyz, DEG2RAD(angleInDegree));
            toWorld = Matrix4x4::mul(toWorld, mat);
        }
        else if (tag == "lookat")
        {
            // HACK: 旧仕様用。近いうちに削除する。
            origin_ = child.attribute("origin").asVec3(Vec3(0.0f));
            target_ = child.attribute("target").asVec3(Vec3(1.0f));
            up_ = child.attribute("up").asVec3(Vec3(0.0f, 1.0f, 0.0f));

            // 
            const Vec3 origin = child.attribute("origin").asVec3(Vec3(0.0f, 0.0f, 0.0f));
            const Vec3 target = child.attribute("target").asVec3(Vec3(1.0f, 1.0f, 1.0f));
            const Vec3 up = child.attribute("up").asVec3(Vec3(0.0f, 1.0f, 0.0f));
            Matrix4x4 viewMatrix;
            viewMatrix.constructAsViewMatrix(origin, target, up);
            toWorld = Matrix4x4::mul(toWorld, viewMatrix);
        }
        else
        {
            assert(false);
        }
    }
    return toWorld;
}

//-------------------------------------------------
// toWorld
//-------------------------------------------------
const Vec3 Transform::toWorld(const Vec3& pos) const
{
    return toWorld_.transform(pos);
}

//-------------------------------------------------
// toLocal
//-------------------------------------------------
const Vec3 Transform::toLocal(const Vec3& pos) const
{
    return toLocal_.transform(pos);
}

//-------------------------------------------------
// toWorldDir
//-------------------------------------------------
const Vec3 Transform::toWorldDir(const Vec3& dir) const
{
    return toWorldDir_.transform(dir);
}

//-------------------------------------------------
// toLocalDir
//-------------------------------------------------
const Vec3 Transform::toLocalDir(const Vec3& dir) const
{
    return toLocalDir_.transform(dir);
}

//-------------------------------------------------
// origin
//-------------------------------------------------
const Vec3& Transform::cameraOrigin() const
{
    return origin_;
}

//-------------------------------------------------
// target
//-------------------------------------------------
const Vec3& Transform::cameraTarget() const
{
    return target_;
}

//-------------------------------------------------
// up
//-------------------------------------------------
const Vec3& Transform::cameraUp() const
{
    return up_;
}
