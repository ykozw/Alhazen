#include "pch.hpp"
#include "accelerator/bvh.hpp"
#include "shape/shape.hpp"
#include "core/logging.hpp"
#include "core/refarray.hpp"

//-------------------------------------------------
// BruteForceBVH
//-------------------------------------------------
BruteForceBVH::BruteForceBVH()
{
}

//-------------------------------------------------
// construct
//-------------------------------------------------
INLINE bool BruteForceBVH::construct(
    const std::vector<Vec3>& vs,
    const std::vector<Vec3>& ns,
    const std::vector<Vec2>& ts,
    const std::vector<MeshFace>& fs)
{
    vs_ = vs;
    ns_ = ns;
    ts_ = ts;
    fs_ = fs;

    // aabbの作成
    aabb_.clear();
    for (auto& v : vs_)
    {
        aabb_.addPoint(v);
    }

    return true;
}

//-------------------------------------------------
// aabb
//-------------------------------------------------
AABB BruteForceBVH::aabb() const
{
    return aabb_;
}

//-------------------------------------------------
// maxDepth
//-------------------------------------------------
int32_t BruteForceBVH::maxDepth() const
{
    return 0;
}

//-------------------------------------------------
// construct
//-------------------------------------------------
bool SimpleBVH::construct(
    const std::vector<Vec3>& vs,
    const std::vector<Vec3>& ns,
    const std::vector<Vec2>& ts,
    const std::vector<MeshFace>& fs)
{
    AL_ASSERT_DEBUG(vs.size() == ns.size());
    AL_ASSERT_DEBUG(fs.size() != 0);
    //
    vs_ = vs;
    fs_ = fs;
    // 全三角形のデータをまとめたものを作成する
    const int32_t faceNum = (int32_t)fs_.size();
    triangles.reserve(faceNum);
    for (int32_t faceNo = 0; faceNo < faceNum; ++faceNo)
    {
        //
        const MeshFace& mf = fs_[faceNo];
        //
        MeshTriangle tri;
        tri.v[0] = vs[mf.vi[0]];
        tri.v[1] = vs[mf.vi[1]];
        tri.v[2] = vs[mf.vi[2]];
        // 法線と頂点位置のインデックスは同じ
        tri.n[0] = ns[mf.vi[0]];
        tri.n[1] = ns[mf.vi[1]];
        tri.n[2] = ns[mf.vi[2]];
        tri.t[0] = ts[mf.ti[0]];
        tri.t[1] = ts[mf.ti[1]];
        tri.t[2] = ts[mf.ti[2]];
        // tri.materialId = mf.mi;
        tri.aabb.clear();
        tri.aabb.addPoints(tri.v.data(), 3);
        triangles.emplace_back(tri);
    }
    // TODO: (通常のBVH構築ができたら)構築中だけの一時データを保持するクラスを作成する。
    // 適当な数だけ予約しておく
    nodes_.reserve(faceNum * 16);
    nodes_.resize(1);
    constructNode(0, triangles.data(), (int32_t)triangles.size(), 0);
    nodes_.shrink_to_fit(); // TODO: 不要？
    // TODO: SoA化したデータにする
    return true;
}

//-------------------------------------------------
// constructNode
//-------------------------------------------------
void SimpleBVH::constructNode(int32_t nodeIndex, MeshTriangle* triangles, int32_t numTriangle, int32_t depth)
{
    //
    maxDepth_ = alMax(maxDepth_, depth);
    // このノードのAABBを求める
    auto& curNode = nodes_[nodeIndex];
    curNode.childlen[0] = -1;
    curNode.childlen[1] = -1;
    curNode.aabb.clear();
    for (int32_t triNo = 0; triNo < numTriangle; ++triNo)
    {
        curNode.aabb.addAABB(triangles[triNo].aabb);
    }
    // 三角形が一つしかない場合は葉
    if (numTriangle == 1)
    {
        auto& v = triangles[0].v;
        auto& n = triangles[0].n;
        auto& t = triangles[0].t;
        curNode.v[0] = v[0];
        curNode.v[1] = v[1];
        curNode.v[2] = v[2];
        curNode.n[0] = n[0];
        curNode.n[1] = n[1];
        curNode.n[2] = n[2];
        curNode.t[0] = t[0];
        curNode.t[1] = t[1];
        curNode.t[2] = t[2];
        //curNode.m = triangles[0].materialId;
        return;
    }
    // 三軸それぞれのSAHを求め最も小さいものを採用する
    int32_t bestAxis = -1;
    int32_t bestTriIndex = -1;
    float bestSAH = std::numeric_limits<float>::max();
    int32_t axis = -1;
    // 三角形ソート用
    auto sortPred = [&axis](const MeshTriangle& lhs, const MeshTriangle& rhs)
    {
        Vec3 lhsc = lhs.aabb.center();
        Vec3 rhsc = rhs.aabb.center();
        return lhsc[axis] < rhsc[axis];
    };
    // TODO: SAHをするか否かをフラグで指定するようにする
#if 0
    //
    for (axis = 0; axis < 3; ++axis)
    {
        // AABB中心座標を現在の軸に沿ってソート
        std::sort(triangles, triangles + numTriangle, sortPred);
        // 一つずつ三角形ずつ移して行き最も良いSAH値が出たら更新する
        for (int32_t triNo = 1; triNo < numTriangle; ++triNo)
        {
            AABB leftAABB;
            AABB rightAABB;
            leftAABB.clear();
            rightAABB.clear();
            // 左右のAABBの算出
            for (int32_t lTri = 0; lTri < triNo; ++lTri)
            {
                leftAABB.addAABB(triangles[lTri].aabb);
            }
            for (int32_t rTri = triNo; rTri < numTriangle; ++rTri)
            {
                rightAABB.addAABB(triangles[rTri].aabb);
            }
            // SAHの算出
            const int32_t leftTriNum = triNo;
            const int32_t rightTriNum = numTriangle - triNo;
            const float sahValue = leftAABB.surface()* leftTriNum + rightAABB.surface()* rightTriNum;
            // 最低値になっていたら更新
            if (sahValue < bestSAH)
            {
                bestAxis = axis;
                bestTriIndex = triNo;
                bestSAH = sahValue;
            }
        }
    }
#else
    // HACK: SAHをやらない版
    static int32_t axisNext = 0;
    bestAxis = (axisNext++) % 3;
    bestTriIndex = numTriangle / 2;
    bestSAH = 0.0f;
#endif
    // 再度ソート
    axis = bestAxis;
    std::sort(triangles, triangles + numTriangle, sortPred);
    //
    //logging("B2: %d", numTriangle);
    nodes_.resize(nodes_.size() + 1);
    curNode.childlen[0] = (int32_t)nodes_.size() - 1;
    //logging("B0: %d", bestTriIndex );
    constructNode(curNode.childlen[0], triangles, bestTriIndex, depth + 1);
    nodes_.resize(nodes_.size() + 1);
    curNode.childlen[1] = (int32_t)nodes_.size() - 1;
    //logging("B1: %d", numTriangle - bestTriIndex);
    constructNode(curNode.childlen[1], triangles + bestTriIndex, numTriangle - bestTriIndex, depth + 1);
}


//-------------------------------------------------
// construct
//-------------------------------------------------
bool QBVH::construct(
    const std::vector<Vec3>& vs,
    const std::vector<Vec3>& ns,
    const std::vector<Vec2>& ts,
    const std::vector<MeshFace>& fs)
{
    //AL_ASSERT_DEBUG(vs.size() == ns.size());
    AL_ASSERT_DEBUG(fs.size() != 0);
    logging("BVH: start");
    //
    logging("BVH: generate triangles");
    vs_ = vs;
    fs_ = fs;
    // 全三角形のデータをまとめたものを作成する
    std::vector<QBVHBuildTriangle> triangles;
    const int32_t faceNum = (int32_t)fs_.size();
    triangles.reserve(faceNum);
    aabb_.clear();
    for (int32_t faceNo = 0; faceNo < faceNum; ++faceNo)
    {
        //
        const MeshFace& mf = fs_[faceNo];
        //
        QBVHBuildTriangle tri;
        tri.v[0] = vs[mf.vi[0]];
        tri.v[1] = vs[mf.vi[1]];
        tri.v[2] = vs[mf.vi[2]];
        tri.n[0] = ns[mf.ni[0]];
        tri.n[1] = ns[mf.ni[1]];
        tri.n[2] = ns[mf.ni[2]];
        tri.t[0] = ts[mf.ti[0]];
        tri.t[1] = ts[mf.ti[1]];
        tri.t[2] = ts[mf.ti[2]];
        tri.m = mf.mi;
        // tri.materialId = mf.mi;
        tri.aabb.clear();
        tri.aabb.addPoints(tri.v.data(), 3);
        triangles.emplace_back(tri);
        //
        aabb_.addPoints(tri.v.data(), 3);
    }
    // TODO: (通常のBVH構築ができたら)構築中だけの一時データを保持するクラスを作成する。
    logging("BVH: start construct nodes");
    // 適当な数だけ予約しておく
    int32_t progress = 0;
    nodes_.reserve(faceNum * 16);
    //nodes_.resize(1);
    RefArray<QBVHBuildTriangle> tris(triangles.data(), (int32_t)triangles.size());
    //constructNodeMedian(0, tris, 0, progress);
    constructNodeSAH(0, tris, 0, progress);
    nodes_.shrink_to_fit(); // TODO: 不要？
    logging("BVH: Finish construct");
    // TODO: SoA化したデータにする
    return true;
}

//-------------------------------------------------
// constructNodeMedian
//-------------------------------------------------
void QBVH::constructNodeMedian(
    int32_t nodeIndex,
    RefArray<QBVHBuildTriangle>& tris,
    int32_t depth,
    int32_t& progress)
{
    // 深さ2( (2)^(2+1) = 総計8ノード)までは集計しておく
    if (depth == 2)
    {
        ++progress;
        logging("BVH: %d/16", progress);
    }
    // 深さ値を更新
    maxDepth_ = alMax(maxDepth_, depth);
    // 軸を決定する
    // 最も幅のある方向をaxisTopに、次に幅のある方向をaxisLeftRightにする
    int32_t axisTop = -1;
    int32_t axisLR = -1;
    AABB thisTriAabb;
    for (auto& tri : tris)
    {
        thisTriAabb.addAABB(tri.aabb);
    }
    const auto size = thisTriAabb.size();
    // xyz
    if ((size.x < size.y) && (size.y < size.z))
    {
        axisTop = 2;
        axisLR = 1;
    }
    // xzy
    else if ((size.x < size.z) && (size.z < size.y))
    {
        axisTop = 1;
        axisLR = 2;
    }
    // yxz
    else if ((size.y < size.x) && (size.x < size.z))
    {
        axisTop = 2;
        axisLR = 0;
    }
    // yzx
    else if ((size.y < size.z) && (size.z < size.x))
    {
        axisTop = 0;
        axisLR = 2;
    }
    // zxy
    else if ((size.z < size.x) && (size.x < size.y))
    {
        axisTop = 1;
        axisLR = 0;
    }
    // zyx
    else
    {
        axisTop = 0;
        axisLR = 1;
    }
    // TODO: 区切るところを決める
    // HACK: とりあえず全て中点にする
    const int32_t numTris = tris.size();
    const int32_t leftPivot = numTris / 4;
    const int32_t topPivot = numTris * 2 / 4;
    const int32_t rightPivot = numTris * 3 / 4;
    // 三角形ソート用
    int32_t sortAxis = -1;
    auto sortPred = [&sortAxis](const QBVHBuildTriangle& lhs, const QBVHBuildTriangle& rhs)
    {
        Vec3 lhsc = lhs.aabb.center();
        Vec3 rhsc = rhs.aabb.center();
        return lhsc[sortAxis] < rhsc[sortAxis];
    };
    float partitionValue = 0.0f;
    auto partitionPred = [&sortAxis, &partitionValue](const QBVHBuildTriangle& tri)
    {
        return tri.aabb.center()[sortAxis] < partitionValue;
    };
    // topのソート
    sortAxis = axisTop;
#if 1
    std::nth_element(tris.begin(), tris.begin() + tris.size() / 2, tris.end(), sortPred);
    partitionValue = tris[tris.size() / 2].aabb.center()[sortAxis];
    std::partition(tris.begin(), tris.end(), partitionPred);
#else
    ene    std::sort(tris.begin(), tris.end(), sortPred);
#endif
    // left/rightのソート
    RefArray<QBVHBuildTriangle> lTris = tris.subArray(0, topPivot);
    RefArray<QBVHBuildTriangle> rTris = tris.subArray(topPivot, numTris);
    sortAxis = axisLR;
#if 1
    std::nth_element(lTris.begin(), lTris.begin() + lTris.size() / 2, lTris.end(), sortPred);
    partitionValue = lTris[lTris.size() / 2].aabb.center()[sortAxis];
    std::partition(lTris.begin(), lTris.end(), partitionPred);
    std::nth_element(rTris.begin(), rTris.begin() + rTris.size() / 2, rTris.end(), sortPred);
    partitionValue = rTris[rTris.size() / 2].aabb.center()[sortAxis];
    std::partition(lTris.begin(), lTris.end(), partitionPred);
#else
    std::sort(lTris.begin(), lTris.end(), sortPred);
    std::sort(rTris.begin(), rTris.end(), sortPred);
#endif
    //
    std::array<RefArray<QBVHBuildTriangle>, 4> trisPerArea;
    trisPerArea[0] = tris.subArray(0, leftPivot);
    trisPerArea[1] = tris.subArray(leftPivot, topPivot);
    trisPerArea[2] = tris.subArray(topPivot, rightPivot);
    trisPerArea[3] = tris.subArray(rightPivot, numTris);
    //
    std::array<float, 4> minxsF;
    std::array<float, 4> minysF;
    std::array<float, 4> minzsF;
    std::array<float, 4> maxxsF;
    std::array<float, 4> maxysF;
    std::array<float, 4> maxzsF;
    //
    const float inf = std::numeric_limits<float>::max();
    minxsF.fill(inf);
    minysF.fill(inf);
    minzsF.fill(inf);
    maxxsF.fill(-inf);
    maxysF.fill(-inf);
    maxzsF.fill(-inf);
    // それぞれのmin/maxを得る
    for (int32_t areaNo = 0; areaNo < 4; ++areaNo)
    {
        const auto& targetTris = trisPerArea[areaNo];
        for (const auto& tri : targetTris)
        {
            for (const auto& v : tri.v)
            {
                minxsF[areaNo] = alMin(v.x, minxsF[areaNo]);
                minysF[areaNo] = alMin(v.y, minysF[areaNo]);
                minzsF[areaNo] = alMin(v.z, minzsF[areaNo]);
                maxxsF[areaNo] = alMax(v.x, maxxsF[areaNo]);
                maxysF[areaNo] = alMax(v.y, maxysF[areaNo]);
                maxzsF[areaNo] = alMax(v.z, maxzsF[areaNo]);
            }
        }
    }
    // QBVHNodeの作成
    assert(nodeIndex == nodes_.size());
    nodes_.push_back(QBVHNode());
    QBVHNode& node = nodes_[nodes_.size() - 1];
    node.aabbs[0][0] = _mm_loadu_ps(minxsF.data());
    node.aabbs[0][1] = _mm_loadu_ps(minysF.data());
    node.aabbs[0][2] = _mm_loadu_ps(minzsF.data());
    node.aabbs[1][0] = _mm_loadu_ps(maxxsF.data());
    node.aabbs[1][1] = _mm_loadu_ps(maxysF.data());
    node.aabbs[1][2] = _mm_loadu_ps(maxzsF.data());
    node.axisTop = axisTop;
    node.axisLeft = axisLR;
    node.axisRight = axisLR;
    // 子供の作成
    for (int32_t chNo = 0; chNo < 4; ++chNo)
    {
        // 葉の場合
        const bool isLeaf = trisPerArea[chNo].size() <= 4;
        if (isLeaf)
        {
            node.childIndex[chNo].isLeaf = true;
            // 葉の作成
            auto& targetTri = trisPerArea[chNo];
            const float inf = std::numeric_limits<float>::max();
            const Vec3 infVert(inf);
            const std::array<Vec3, 3> dummyVerts = { infVert, infVert, infVert };
            const auto& t0 = targetTri.size() >= 1 ? targetTri[0].v : dummyVerts;
            const auto& t1 = targetTri.size() >= 2 ? targetTri[1].v : dummyVerts;
            const auto& t2 = targetTri.size() >= 3 ? targetTri[2].v : dummyVerts;
            const auto& t3 = targetTri.size() >= 4 ? targetTri[3].v : dummyVerts;
            ALIGN32 float x0s[4] = { t0[0].x, t1[0].x, t2[0].x, t3[0].x };
            ALIGN32 float x1s[4] = { t0[1].x, t1[1].x, t2[1].x, t3[1].x };
            ALIGN32 float x2s[4] = { t0[2].x, t1[2].x, t2[2].x, t3[2].x };
            ALIGN32 float y0s[4] = { t0[0].y, t1[0].y, t2[0].y, t3[0].y };
            ALIGN32 float y1s[4] = { t0[1].y, t1[1].y, t2[1].y, t3[1].y };
            ALIGN32 float y2s[4] = { t0[2].y, t1[2].y, t2[2].y, t3[2].y };
            ALIGN32 float z0s[4] = { t0[0].z, t1[0].z, t2[0].z, t3[0].z };
            ALIGN32 float z1s[4] = { t0[1].z, t1[1].z, t2[1].z, t3[1].z };
            ALIGN32 float z2s[4] = { t0[2].z, t1[2].z, t2[2].z, t3[2].z };
            //
            leafs_.push_back(QBVHLeaf());
            QBVHLeaf& leaf = leafs_[leafs_.size()-1];
            leaf.x[0] = _mm_load_ps(x0s);
            leaf.x[1] = _mm_load_ps(x1s);
            leaf.x[2] = _mm_load_ps(x2s);
            leaf.y[0] = _mm_load_ps(y0s);
            leaf.y[1] = _mm_load_ps(y1s);
            leaf.y[2] = _mm_load_ps(y2s);
            leaf.z[0] = _mm_load_ps(z0s);
            leaf.z[1] = _mm_load_ps(z1s);
            leaf.z[2] = _mm_load_ps(z2s);
            //
            for (int32_t ti = 0; ti < targetTri.size(); ++ti)
            {
                leaf.v[ti] = targetTri[ti].v;
                leaf.n[ti] = targetTri[ti].n;
                leaf.t[ti] = targetTri[ti].t;
                // leaf.materialId[ti] = targetTri[ti].materialId;
            }
            // 葉のインデックス
            node.childIndex[chNo].index = (int32_t)leafs_.size() - 1;
        }
        // 節の場合
        else
        {
            // 節のインデックスの算出
            node.childIndex[chNo].isLeaf = false;
            const int32_t index = (int32_t)nodes_.size();
            node.childIndex[chNo].index = index;
            constructNodeMedian(index, trisPerArea[chNo], depth + 1, progress);
        }
    }
}

//-------------------------------------------------
// constructNodeMedian
//-------------------------------------------------
void QBVH::constructNodeSAH(
    int32_t nodeIndex,
    RefArray<QBVHBuildTriangle>& tris,
    int32_t depth,
    int32_t& progress)
{
    // 深さ2( (2)^(2+1) = 総計8ノード)までは集計しておく
    if (depth == 2)
    {
        ++progress;
        logging("BVH: %d/16", progress);
    }
    // 深さ値を更新
    maxDepth_ = alMax(maxDepth_, depth);
    // 軸を決定する
    // 最も幅のある方向をaxisTopに、次に幅のある方向をaxisLeftRightにする
    uint8_t axisTop = 0xff;
    uint8_t axisLR = 0xff;
    AABB thisTriAabb;
    for (auto& tri : tris)
    {
        thisTriAabb.addAABB(tri.aabb);
    }
    const auto size = thisTriAabb.size();
    // xyz
    if ((size.x < size.y) && (size.y < size.z))
    {
        axisTop = 2;
        axisLR = 1;
    }
    // xzy
    else if ((size.x < size.z) && (size.z < size.y))
    {
        axisTop = 1;
        axisLR = 2;
    }
    // yxz
    else if ((size.y < size.x) && (size.x < size.z))
    {
        axisTop = 2;
        axisLR = 0;
    }
    // yzx
    else if ((size.y < size.z) && (size.z < size.x))
    {
        axisTop = 0;
        axisLR = 2;
    }
    // zxy
    else if ((size.z < size.x) && (size.x < size.y))
    {
        axisTop = 1;
        axisLR = 0;
    }
    // zyx
    else
    {
        axisTop = 0;
        axisLR = 1;
    }
    // TODO: 区切るところを決める
    // HACK: とりあえず全て中点にする
    const int32_t numTris = tris.size();
    const int32_t leftPivot = numTris / 4;
    const int32_t topPivot = numTris * 2 / 4;
    const int32_t rightPivot = numTris * 3 / 4;
    // 三角形ソート用
    int32_t sortAxis = -1;
    auto sortPred = [&sortAxis](const QBVHBuildTriangle& lhs, const QBVHBuildTriangle& rhs)
    {
        Vec3 lhsc = lhs.aabb.center();
        Vec3 rhsc = rhs.aabb.center();
        return lhsc[sortAxis] < rhsc[sortAxis];
    };
    float partitionValue = 0.0f;
    auto partitionPred = [&sortAxis, &partitionValue](const QBVHBuildTriangle& tri)
    {
        return tri.aabb.center()[sortAxis] < partitionValue;
    };
    // topのソート
    sortAxis = axisTop;
#if 1
    std::nth_element(tris.begin(), tris.begin() + tris.size() / 2, tris.end(), sortPred);
    partitionValue = tris[tris.size() / 2].aabb.center()[sortAxis];
    std::partition(tris.begin(), tris.end(), partitionPred);
#else
    ene    std::sort(tris.begin(), tris.end(), sortPred);
#endif
    // left/rightのソート
    RefArray<QBVHBuildTriangle> lTris = tris.subArray(0, topPivot);
    RefArray<QBVHBuildTriangle> rTris = tris.subArray(topPivot, numTris);
    sortAxis = axisLR;
#if 1
    std::nth_element(lTris.begin(), lTris.begin() + lTris.size() / 2, lTris.end(), sortPred);
    partitionValue = lTris[lTris.size() / 2].aabb.center()[sortAxis];
    std::partition(lTris.begin(), lTris.end(), partitionPred);
    std::nth_element(rTris.begin(), rTris.begin() + rTris.size() / 2, rTris.end(), sortPred);
    partitionValue = rTris[rTris.size() / 2].aabb.center()[sortAxis];
    std::partition(lTris.begin(), lTris.end(), partitionPred);
#else
    std::sort(lTris.begin(), lTris.end(), sortPred);
    std::sort(rTris.begin(), rTris.end(), sortPred);
#endif
    //
    std::array<RefArray<QBVHBuildTriangle>, 4> trisPerArea;
    trisPerArea[0] = tris.subArray(0, leftPivot);
    trisPerArea[1] = tris.subArray(leftPivot, topPivot);
    trisPerArea[2] = tris.subArray(topPivot, rightPivot);
    trisPerArea[3] = tris.subArray(rightPivot, numTris);
    //
    std::array<float, 4> minxsF;
    std::array<float, 4> minysF;
    std::array<float, 4> minzsF;
    std::array<float, 4> maxxsF;
    std::array<float, 4> maxysF;
    std::array<float, 4> maxzsF;
    //
    const float inf = std::numeric_limits<float>::max();
    minxsF.fill(inf);
    minysF.fill(inf);
    minzsF.fill(inf);
    maxxsF.fill(-inf);
    maxysF.fill(-inf);
    maxzsF.fill(-inf);
    // それぞれのmin/maxを得る
    for (int32_t areaNo = 0; areaNo < 4; ++areaNo)
    {
        const auto& targetTris = trisPerArea[areaNo];
        for (const auto& tri : targetTris)
        {
            for (const auto& v : tri.v)
            {
                minxsF[areaNo] = alMin(v.x, minxsF[areaNo]);
                minysF[areaNo] = alMin(v.y, minysF[areaNo]);
                minzsF[areaNo] = alMin(v.z, minzsF[areaNo]);
                maxxsF[areaNo] = alMax(v.x, maxxsF[areaNo]);
                maxysF[areaNo] = alMax(v.y, maxysF[areaNo]);
                maxzsF[areaNo] = alMax(v.z, maxzsF[areaNo]);
            }
        }
    }
    // QBVHNodeの作成
    assert(nodeIndex == nodes_.size());
    nodes_.push_back(QBVHNode());
    QBVHNode& node = nodes_[nodes_.size() - 1];
    node.aabbs[0][0] = _mm_loadu_ps(minxsF.data());
    node.aabbs[0][1] = _mm_loadu_ps(minysF.data());
    node.aabbs[0][2] = _mm_loadu_ps(minzsF.data());
    node.aabbs[1][0] = _mm_loadu_ps(maxxsF.data());
    node.aabbs[1][1] = _mm_loadu_ps(maxysF.data());
    node.aabbs[1][2] = _mm_loadu_ps(maxzsF.data());
    node.axisTop = axisTop;
    node.axisLeft = axisLR;
    node.axisRight = axisLR;
    // 子供の作成
    for (int32_t chNo = 0; chNo < 4; ++chNo)
    {
        // 葉の場合
        const bool isLeaf = trisPerArea[chNo].size() <= 4;
        if (isLeaf)
        {
            node.childIndex[chNo].isLeaf = true;
            // 葉の作成
            auto& targetTri = trisPerArea[chNo];
            // 浮動小数の例外が発生しない程度には大きすぎない値にしておく
            const float inf = 1.0e+15f;
            const Vec3 infVert(inf);
            const std::array<Vec3, 3> dummyVerts = { infVert, infVert, infVert };
            const auto& t0 = targetTri.size() >= 1 ? targetTri[0].v : dummyVerts;
            const auto& t1 = targetTri.size() >= 2 ? targetTri[1].v : dummyVerts;
            const auto& t2 = targetTri.size() >= 3 ? targetTri[2].v : dummyVerts;
            const auto& t3 = targetTri.size() >= 4 ? targetTri[3].v : dummyVerts;
            ALIGN32 float x0s[4] = { t0[0].x, t1[0].x, t2[0].x, t3[0].x };
            ALIGN32 float x1s[4] = { t0[1].x, t1[1].x, t2[1].x, t3[1].x };
            ALIGN32 float x2s[4] = { t0[2].x, t1[2].x, t2[2].x, t3[2].x };
            ALIGN32 float y0s[4] = { t0[0].y, t1[0].y, t2[0].y, t3[0].y };
            ALIGN32 float y1s[4] = { t0[1].y, t1[1].y, t2[1].y, t3[1].y };
            ALIGN32 float y2s[4] = { t0[2].y, t1[2].y, t2[2].y, t3[2].y };
            ALIGN32 float z0s[4] = { t0[0].z, t1[0].z, t2[0].z, t3[0].z };
            ALIGN32 float z1s[4] = { t0[1].z, t1[1].z, t2[1].z, t3[1].z };
            ALIGN32 float z2s[4] = { t0[2].z, t1[2].z, t2[2].z, t3[2].z };
            //
            leafs_.push_back(QBVHLeaf());
            QBVHLeaf& leaf = leafs_[leafs_.size() - 1];
            leaf.x[0] = _mm_load_ps(x0s);
            leaf.x[1] = _mm_load_ps(x1s);
            leaf.x[2] = _mm_load_ps(x2s);
            leaf.y[0] = _mm_load_ps(y0s);
            leaf.y[1] = _mm_load_ps(y1s);
            leaf.y[2] = _mm_load_ps(y2s);
            leaf.z[0] = _mm_load_ps(z0s);
            leaf.z[1] = _mm_load_ps(z1s);
            leaf.z[2] = _mm_load_ps(z2s);
            //
            for (int32_t ti = 0; ti < targetTri.size(); ++ti)
            {
                leaf.v[ti] = targetTri[ti].v;
                leaf.n[ti] = targetTri[ti].n;
                leaf.t[ti] = targetTri[ti].t;
                leaf.m[ti] = targetTri[ti].m;
            }
            // 葉のインデックス
            node.childIndex[chNo].index = (int32_t)leafs_.size() - 1;
        }
        // 節の場合
        else
        {
            // 節のインデックスの算出
            node.childIndex[chNo].isLeaf = false;
            const int32_t index = (int32_t)nodes_.size();
            node.childIndex[chNo].index = index;
            //constructNodeMedian(index, trisPerArea[chNo], depth + 1, progress);
            constructNodeSAH(index, trisPerArea[chNo], depth + 1, progress);
        }
    }
}

//-------------------------------------------------
// aabb
//-------------------------------------------------
AABB QBVH::aabb() const
{
    return aabb_;
}

//-------------------------------------------------
// numTris
//-------------------------------------------------
int32_t QBVH::numVerts() const
{
    return (int32_t)vs_.size();
}

//-------------------------------------------------
// numFaces
//-------------------------------------------------
int32_t QBVH::numFaces() const
{
    return (int32_t)fs_.size();
}

//-------------------------------------------------
// maxDepth
//-------------------------------------------------
int32_t QBVH::maxDepth() const
{
    return maxDepth_;
}
