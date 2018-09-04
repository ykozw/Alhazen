#pragma once

/*
Embree化のためのTODOs
- IsectEngineのラッパーを作成する
 -> ライトなどは交差エンジンの知るものではないのでその上の層でなんとかする
 -> 
- bvh専用のIntersectにしてprimIdxを返すようにする
- そもそも交差のShapeを直接返す必要があるのか？
- rtcGetGeometry()で得たgeomはどうやって元の幾何情報を得られるのか？
- 意図しないコピーが発生しないようにする
- https://embree.github.io/api.html
rtcSetGeometryBuildQuality()から先のドキュメントを読む
*/
#include "fwd.hpp"
#include "shape/shape.hpp"
#include "accelerator/bvh.hpp"
#include "core/math.hpp"
#include "core/object.hpp"
#include "core/ray.hpp"
#include "light/light.hpp"
#include "intersect/intersectengine.hpp"

/*
-------------------------------------------------
シーンの幾何情報
-------------------------------------------------
*/
class SceneGeom
{
public:
    SceneGeom();
    bool
        intersect(const Ray& ray, bool skipLight, Intersect* isect) const;
    bool intersectCheck(const Ray& ray, bool skipLight) const;
    const std::vector<LightPtr>& lights() const;
    bool isVisible(const Vec3& p0, const Vec3& p1, bool skipLight) const;
    void addShape(ShapePtr shape);
    void addLight(LightPtr light);
    void buildScene();
    AABB aabb() const;

private:
    // 交差エンジン
    std::unique_ptr<IsectEngine> isectEngine_;
    // 交差世界
    std::unique_ptr<IsectScene> geometory_;
    // Lightの集合
    std::vector<LightPtr> lights_;
    std::vector<bool> isInBvh_;
    LightBVH lightsBVH_;
    //TODO: 補間のための法線などをここに持たせる
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectEngine
{
public:
    IsectEngine() = default;
    virtual ~IsectEngine() = default;
    virtual std::unique_ptr<IsectScene> createScene() = 0;
public:
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectScene
{
public:
    struct Interpolated
    {
        // 座標
        Vec3 position;
        // シェーディング法線
        Vec3 ns;
        // ジオメトリ法線
        Vec3 ng;
        // テクスチャUV
        Vec2 uv;
    };
    typedef std::function<Interpolated(int32_t primId, Vec2 biuv)> InterpolateFun;
public:
    virtual void addShape(ShapePtr shape) = 0;
    //
    virtual void addMesh(
        int32_t numVtx,
        int32_t numFace,
        const std::function<Vec3(int32_t idx)>& getVtx,
        const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace,
        const InterpolateFun& intepolate) = 0;
    //
    virtual void buildScene() = 0;
    virtual bool
    intersect(const Ray& ray, Intersect* isect) const = 0;
    virtual bool intersectCheck(const Ray& ray) const = 0;
    virtual bool
    isVisible(const Vec3& p0, const Vec3& p1) const = 0;
    virtual AABB aabb() const = 0;

private:
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectEngineBasic
    :public IsectEngine
{
public:
    IsectEngineBasic() = default;
    virtual ~IsectEngineBasic() = default;
    std::unique_ptr<IsectScene> createScene() override;
public:
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class IntersectSceneBasic :public IsectScene
{
public:
    void addShape(ShapePtr shape);
    void addMesh(
        int32_t numVtx,
        int32_t numFace,
        const std::function<Vec3(int32_t vi)>& getVtx,
        const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace,
        const InterpolateFun& interpolateFun);
    void buildScene();
    bool intersect(const Ray& ray,
        Intersect* isect) const;

    bool intersectCheck(const Ray& ray) const;
    bool isVisible(const Vec3& p0, const Vec3& p1) const;
    AABB aabb() const;

private:
    struct MeshInfo
    {
        BVH bvh_;
        InterpolateFun interpolateFun;
    };
    std::vector<MeshInfo> meshInfos_;;
    // Shape
    std::vector<ShapePtr> shapes_;
    ShapeBVH shapeBvh_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectEngineEmbreeV3 : public IsectEngine
{
public:
    IsectEngineEmbreeV3();
    ~IsectEngineEmbreeV3();
    std::unique_ptr<IsectScene> createScene() override;

public:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
struct IsectSceneEmbree : public IsectScene
{
public:
    IsectSceneEmbree();
    ~IsectSceneEmbree();
    void addMesh(
        int32_t numVtx,
        int32_t numFace,
        const std::function<Vec3(int32_t idx)>& getVtx,
        const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace,
        const InterpolateFun& interpolate) override;
    //
    void addShape(ShapePtr shape) override { AL_ASSERT_ALWAYS(false); }
    void buildScene() override;
    bool
    intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;
    bool
    isVisible(const Vec3& p0, const Vec3& p1) const override;
    AABB aabb() const override;

public:
    class Impl;
    std::unique_ptr<Impl> impl_;
};