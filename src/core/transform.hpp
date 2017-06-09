#pragma once

#include "pch.hpp"
#include "core/object.hpp"
#include "math.hpp"

//-------------------------------------------------
// Transform
// WorldとLocalの間での全ての線形変換を扱う
// WorldとLocalの間で回転だけを扱いたい場合はOrthonormalBasisクラスを使う
//-------------------------------------------------
class Transform
{
public:
	Transform();
	Transform(const ObjectProp& objectProp);
	const Vec3 toWorld(const Vec3& pos) const;
    const Vec3 toLocal(const Vec3& pos) const;
    const Vec3 toWorldDir(const Vec3& dir) const;
    const Vec3 toLocalDir(const Vec3& dir) const;

	// TODO: カメラ用メンバ。matrix_から取得できるようにする？
	const Vec3& cameraOrigin() const;
	const Vec3& cameraTarget() const;
	const Vec3& cameraUp() const;
private:
    Matrix4x4 constructToWorldMatrix(const ObjectProp& objectProp) const;
public: // HACK: 一部内部パラメーターを触っている箇所があるのでそれが解消されるまで開いておく
    Matrix4x4 toWorld_;
    Matrix4x4 toLocal_;
    Matrix4x4 toWorldDir_;
    Matrix4x4 toLocalDir_;

	// lookat用のパラメーター HACK: matrix_に集約して削除する
	mutable Vec3 origin_;
    mutable Vec3 target_;
    mutable Vec3 up_;
};
