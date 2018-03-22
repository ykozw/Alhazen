#pragma once

#include "pch.hpp"
#include "core/math.hpp"
#include "core/ray.hpp"
#include "core/bounding.hpp"
#include "core/refarray.hpp"
#include "shape/mesh.hpp"
#include "shape/shape.hpp"

/*
-------------------------------------------------
三角形メッシュ専用のBVH
-------------------------------------------------
*/
class BVHBase
{
public:
    BVHBase() {}
    virtual ~BVHBase() {}
    virtual bool construct(const std::vector<Vec3>& vs,
                           const std::vector<Vec3>& ns,
                           const std::vector<Vec2>& ts,
                           const std::vector<MeshFace>& fs) = 0;
    virtual AABB aabb() const = 0;
    virtual int32_t numVerts() const { return 0; }
    virtual int32_t numFaces() const { return 0; }
    virtual int32_t maxDepth() const = 0;
    virtual bool intersect(const Ray& ray, Intersect* isect) const = 0;
    virtual bool intersectCheck(const Ray& ray) const = 0;
};
typedef std::shared_ptr<BVHBase> BVHAcceleratorPtr;

/*
-------------------------------------------------
-------------------------------------------------
*/
class BruteForceBVH : public BVHBase
{
public:
    BruteForceBVH();
    virtual ~BruteForceBVH() {}
    bool construct(const std::vector<Vec3>& vs,
                   const std::vector<Vec3>& ns,
                   const std::vector<Vec2>& ts,
                   const std::vector<MeshFace>& fs) override;
    AABB aabb() const override;
    int32_t maxDepth() const override;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;

private:
    std::vector<Vec3> vs_;
    std::vector<Vec3> ns_;
    std::vector<Vec2> ts_;
    std::vector<MeshFace> fs_;
    AABB aabb_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class SimpleBVH : public BVHBase
{
public:
    SimpleBVH()
      : maxDepth_(0)
    {}
    virtual ~SimpleBVH() {}
    bool construct(const std::vector<Vec3>& vs,
                   const std::vector<Vec3>& ns,
                   const std::vector<Vec2>& ts,
                   const std::vector<MeshFace>& fs) override;
    AABB aabb() const override;
    int32_t maxDepth() const override;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;

private:
    struct MeshTriangle
    {
        // 頂点位置
        std::array<Vec3, 3> v;
        // 法線
        std::array<Vec3, 3> n;
        // UV
        std::array<Vec2, 3> t;
        // AABB
        AABB aabb;
        // マテリアルID
        // MaterialId materialId;
    };
    // TODO:
    // 節であっても全てにv,n,tのデータがありフットプリントを逼迫しているので直す
    struct Node
    {
        // 枝であった場合の子のノードインデックス。葉の場合は全て-1が格納されている。
        int32_t childlen[2];
        // AABB
        AABB aabb;
        // 葉であった場合の頂点座標。枝の場合は無効な値。
        std::array<Vec3, 3> v;
        // 葉であった場合の法線座標。枝の場合は無効な値。
        std::array<Vec3, 3> n;
        // 葉であった場合のUV座標。枝の場合は無効な値。
        std::array<Vec2, 3> t;
        // 葉であった場合のマテリアルID。枝の場合は無効な値。
        // MaterialId m;
    };

private:
    void constructNode(int32_t nodeIndex,
                       MeshTriangle* triangles,
                       int32_t numTriangle,
                       int32_t depth);
    bool intersectSub(int32_t nodeIndex,
                      const Ray& ray,
                      Intersect* isect) const;

private:
    std::vector<Vec3> vs_;
    std::vector<MeshFace> fs_;
    // 各三角形のAABB
    std::vector<AABB> nodeAABBs_; // numTriangle x1
    // ノード
    std::vector<Node> nodes_;
    // 一時データ TODO: 削除できるなら削除する
    std::vector<MeshTriangle> triangles;
#if 0 // 速度を比較するためにSoA化したデータ群。一通り動いたら使うようにする。
    // BVHの各ノードが持つデータ群
    std::vector<AABB> nodeAABBs_; // numNode x1
    std::vector<uint32_t> nodeTriangleIndexs_; // numNode x3
    std::vector<uint32_t> nodeChildren_; // numNode x2
#endif
    int32_t maxDepth_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
struct QBVHNode
{
    // 含まれるAABBx4の最小と最大。節用。
    // (mnx,mnx,mnx,mnx),(mny,mny,mny,mny),(mnz,mnz,mnz,mnz),
    // (mxx,mxx,mxx,mxx),(mxy,mxy,mxy,mxy),(mxz,mxz,mxz,mxz)
    __m128 aabbs[2][3];
    // 分割の軸。節用。
    uint8_t axisTop;
    uint8_t axisLeft;
    uint8_t axisRight;
    //
    struct Index
    {
        // 0:節 1:葉
        bool isLeaf : 1;
        // 節か葉のインデックス
        int32_t index : 31;
    };
    std::array<Index, 4> childIndex;
    //
};

/*
-------------------------------------------------
-------------------------------------------------
*/
struct QBVHLeaf
{
    // SIMD化された頂点
    __m128 x[3]; // (t1x1, t2x1, t3x1, t4x1) (t1x2, t2x2, t3x2, t4x2) (t1x3,
                 // t2x3, t3x3, t4x3)
    __m128 y[3]; // (t1y1, t2y1, t3y1, t4y1) (t1y2, t2y2, t3y2, t4y2) (t1y3,
                 // t2y3, t3y3, t4y3)
    __m128 z[3]; // (t1z1, t2z1, t3z1, t4z1) (t1z2, t2z2, t3z2, t4z2) (t1z3,
                 // t2z3, t3z3, t4z3)
    // 頂点位置 (t1x1, t1x2, t1x3) (t2x1, t2x2, t2x3) (t3x1, t3x2, t3x3)
    std::array<std::array<Vec3, 3>, 4> v;
    // 法線 (t1x1, t1x2, t1x3) (t2x1, t2x2, t2x3) (t3x1, t3x2, t3x3) (t4x1,
    // t4x2, t4x3)
    std::array<std::array<Vec3, 3>, 4> n;
    // UV (t1u, t1v) (t2u, t2v) (t3u, t3v) (t4u, t4v)
    std::array<std::array<Vec2, 3>, 4> t;
    // マテリアルID m1, m2, m3, m4
    std::array<int8_t, 4> m;

    QBVHLeaf()
    {
        m[0] = -1;
        m[1] = -1;
        m[2] = -1;
        m[3] = -1;
    }
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class QBVH
{
public:
    QBVH()
      : maxDepth_(0)
    {}
    ~QBVH() {}
    bool construct(const std::vector<Vec3>& vs,
                   const std::vector<Vec3>& ns,
                   const std::vector<Vec2>& ts,
                   const std::vector<MeshFace>& fs);
    int32_t numVerts() const;
    int32_t numFaces() const;
    int32_t maxDepth() const;
    AABB aabb() const;
    bool intersect(const Ray& ray,
                   Intersect* isect,
                   int8_t* materialId) const;
    bool intersectCheck(const Ray& ray) const;

private:
    /*
    -------------------------------------------------
    -------------------------------------------------
    */
    struct QBVHBuildTriangle
    {
        // 頂点位置
        std::array<Vec3, 3> v;
        // 法線
        std::array<Vec3, 3> n;
        // UV
        std::array<Vec2, 3> t;
        // マテリアルID
        int8_t m;
        // AABB
        AABB aabb;
    };
    enum class ConstructMethod
    {
        CM_MEDIAN,
        CM_SAH,
    };

private:
    void constructNodeMedian(int32_t nodeIndex,
                             RefArray<QBVHBuildTriangle>& tris,
                             int32_t depth,
                             int32_t& progress);
    void constructNodeSAH(int32_t nodeIndex,
                          RefArray<QBVHBuildTriangle>& tris,
                          int32_t depth,
                          int32_t& progress);

private:
    std::vector<Vec3> vs_;
    std::vector<MeshFace> fs_;
    std::vector<AABB> nodeAABBs_;
    // ノード
    std::vector<QBVHNode> nodes_;
    // 葉
    std::vector<QBVHLeaf> leafs_;
    //
    AABB aabb_;
    //
    int32_t maxDepth_;
    // マテリアル名とマテリアルIDの対応表
    std::unordered_map<std::string, int32_t> materialNameIds_;
};

//
// typedef BruteForceBVH BVH;
typedef QBVH BVH;

/*
-------------------------------------------------
AABBを返すもの専用のBVH
-------------------------------------------------
*/
class ShapeBVH
{
public:
    typedef std::vector<ShapePtr>::iterator ShapeListIte;

public:
    void construct(const std::vector<ShapePtr>& shapes);
    bool intersect(const Ray& ray, Intersect* isect) const;
    bool intersectSub(int32_t nodeIndex,
                      const Ray& ray,
                      Intersect* isect) const;

private:
    struct Node
    {
        // 枝であった場合の子のノードインデックス。葉の場合は全て-1が格納されている。
        std::array<int32_t, 2> childlen = { -1, -1 };
        // 葉であった場合の指し示すShape
        ShapePtr shape;
        // AABB
        AABB aabb;
    };

private:
    void constructSub(ShapeListIte begin,
                      ShapeListIte end,
                      std::vector<Node>& nodes,
                      int32_t nodeIndex);

private:
    std::vector<Node> nodes_;
};

#include "bvh.inl"
