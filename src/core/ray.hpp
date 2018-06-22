#pragma once

#include "core/math.hpp"
#include "bsdf/bsdf.hpp"
//#include "shape/shape.hpp"

/*
-------------------------------------------------
Ray
TODO: 一度作ったらconstなのでそれを明言するために全体をconstにしたい
-------------------------------------------------
*/
struct Ray AL_FINAL
{
public:
    Vec3 o = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 d = Vec3(1.0f, 0.0f, 0.0);
    Vec3 dinv = Vec3(0.0f, 0.0f, 0.0);
    float mint = 0.0f;
    float maxt = std::numeric_limits<float>::max();
    std::array<int32_t, 3> sign = { { 0, 0, 0 } };

public:
    Ray();
    Ray(Vec3 pos,
        Vec3 dir,
        float mint = 0.0f,
        float maxt = std::numeric_limits<float>::max());
    Ray(const Ray& other);
};

/*
-------------------------------------------------
Intersect
-------------------------------------------------
*/
struct Intersect AL_FINAL
{
public:
    /*
    「org + dir * t」のt
    Ray::tは最初に設定したら更新はしないが、このtは交差判定を繰り返すことで更新される
    「無限遠のライトと交差した」ことを表すためにinfを使うことに注意
    このメンバーを見て交差をしたか否かを判定することはできない
    */
    float t = std::numeric_limits<float>::infinity();
    float rayEpsilon = 0.0f;
    Vec3 normal = Vec3(1.0f, 0.0f, 0.0);
    Vec3 position = Vec3(0.0f);
    // 重心座標系のUV
    Vec2 uvBicentric = Vec2(0.0f, 0.0f);
    //
    Vec2 uv = Vec2(0.0f, 0.0f);
    BSDF* bsdf = nullptr;
    // 交差点でのEmission
    Spectrum emission = Spectrum::Black;
    // 交差したオブジェクト
    const SceneObject* sceneObject = nullptr;
    //
    int32_t materialId = 0;

public:
    Intersect();
    void clear();
    //
    Vec3 uppserSideOrigin() const;
    Vec3 belowSideOrigin() const;
    //
    void setHit(bool hit);
    bool isHit() const;
    bool isHitWithInf() const;

private:
    bool isHit_ = false;
};

#include "ray.inl"
