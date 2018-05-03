#pragma once

#include "core/math.hpp"
#include "core/bounding.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
struct KdTreeNode
{
    // 0:x 1:y 2:z
    int32_t axis = -1;
    int32_t left = -1;
    int32_t right = -1;
    AABB bound;
    //
    KdTreePoint point;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
class KdTree
{
public:
    typedef KdTreeNode<KdTreePoint> NodeType;

public:
    KdTree() = default;
    ~KdTree() = default;
    void construct(KdTreePoint* points, int32_t numPoints);
    size_t numNode() const;
    KdTreePoint findNearest(Vec3 point, float* minDist = nullptr) const;
    KdTreePoint findNearestBF(Vec3 point) const;
    float findKNN(Vec3 point,
                  int32_t numSerch,
                  std::vector<const KdTreePoint*>& points) const;
    float findKNNBF(Vec3 poin,
                    int32_t numSerch,
                    std::vector<const KdTreePoint*>& points) const;
    void print() const;

private:
    int32_t constructSub(KdTreePoint* points,
                         int32_t numPoints,
                         std::vector<NodeType>& nodes,
                         int32_t& nodeIndex);

    void findNearestSub(Vec3 point,
                        int32_t nodeIndex,
                        float& bestDist,
                        const KdTreeNode<KdTreePoint>*& bestDistNode) const;

    void findKNNSub(Vec3 targetPoint,
                    int32_t numSerch,
                    int32_t nodeIndex,
                    float& farDist,
                    std::vector<const KdTreePoint*>& points) const;

    //
    float calcDist(const NodeType& node, Vec3 pos) const;
    float calcDist(const KdTreePoint* node, Vec3 pos) const;
    float calcDistSq(const NodeType& node, Vec3 pos) const;
    float calcDistSq(const KdTreePoint* node, Vec3 pos) const;

private:
    std::vector<NodeType> nodes_;
};

/*
-------------------------------------------------
kd-treeを構築する
入力された点列は内部で保持され、またソートされる
TODO: 完成したらinlに移動させる
-------------------------------------------------
*/
template<typename KdTreePoint>
inline void
KdTree<KdTreePoint>::construct(KdTreePoint* points, int32_t numPoints)
{
    nodes_.resize(numPoints);
    int32_t nodeIndex = 0;
    constructSub(points, numPoints, nodes_, nodeIndex);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
inline size_t
KdTree<KdTreePoint>::numNode() const
{
    return nodes_.size();
}

/*
-------------------------------------------------
TODO: 完成したらinlに移動させる
-------------------------------------------------
*/
template<typename KdTreePoint>
inline int32_t
KdTree<KdTreePoint>::constructSub(KdTreePoint* points,
                                  int32_t numPoints,
                                  std::vector<NodeType>& nodes,
                                  int32_t& nodeIndex)
{
    //
    if (numPoints <= 0) {
        return -1;
    }
    // 各軸に対して広がりがある方向を見つける
    AABB aabb;
    for (int32_t i = 0; i < numPoints; ++i) {
        aabb.addPoint(points[i].position());
    }
    const Vec3 s = aabb.size();
    int32_t axis = -1;
    if (s[0] < s[1]) {
        if (s[1] < s[2]) {
            axis = 2;
        } else {
            axis = 1;
        }
    } else {
        if (s[0] < s[2]) {
            axis = 2;
        } else {
            axis = 0;
        }
    }
    const int32_t midPoint = numPoints / 2;
    // その軸で二分する
    std::nth_element(
      points,
      points + midPoint,
      points + numPoints,
      [&axis](const KdTreePoint& lhs, const KdTreePoint& rhs) -> bool {
          const Vec3 l = lhs.position();
          const Vec3 r = rhs.position();
          return l[axis] < r[axis];
      });
    //
    const int32_t thisNodeIndex = nodeIndex;
    NodeType& node = nodes[thisNodeIndex];
    ++nodeIndex;
    node.point = points[midPoint];
    node.left = constructSub(points, midPoint, nodes, nodeIndex);
    node.right = constructSub(
      points + midPoint + 1, numPoints - midPoint - 1, nodes, nodeIndex);
    node.axis = axis;
    // AABBの作成
    AABB& bound = node.bound;
    bound.clear();
    bound.addPoint(node.point.position());
    if (node.left != -1) {
        bound.addAABB(nodes[node.left].bound);
    }
    if (node.right != -1) {
        bound.addAABB(nodes[node.right].bound);
    }
    //
    return thisNodeIndex;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
INLINE KdTreePoint
KdTree<KdTreePoint>::findNearest(Vec3 point, float* minDist) const
{
    // 空のツリーはそもそも探索しない
    AL_ASSERT_DEBUG(!nodes_.empty());
    //
    float tmp;
    float& bestDist = (minDist != nullptr) ? *minDist : tmp;
    bestDist = std::numeric_limits<float>::max();
    const NodeType* bestDistNode;
    findNearestSub(point, 0, bestDist, bestDistNode);
    return bestDistNode->point;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
INLINE void
KdTree<KdTreePoint>::findNearestSub(
  Vec3 point,
  int32_t nodeIndex,
  float& bestDist,
  const KdTreeNode<KdTreePoint>*& bestDistNode) const
{
    //
    if (nodeIndex == -1) {
        return;
    }
    const NodeType& node = nodes_[nodeIndex];
    // 最短距離よりもノードへの距離がすでに遠い場合は棄却
    const int32_t axis = node.axis;
    const auto& bound = node.bound;
    const auto& mn = bound.min();
    const auto& mx = bound.max();
    const float d0 = mn[axis] - point[axis];
    const float d1 = point[axis] - mx[axis];
    if (bestDist < d0 || bestDist < d1) {
        return;
    }
    // このノードのポイントがより近くなっていたら検索結果を更新
    const Vec3 nodePos = node.point.position();
    const float diff = Vec3::distance(point, nodePos);
    if (diff < bestDist) {
        bestDist = diff;
        bestDistNode = &node;
    }
    // 積んでいく順番はノード内で、ポイントが属している方向から。
    if (point[axis] < nodePos[axis]) {
        findNearestSub(point, node.left, bestDist, bestDistNode);
        findNearestSub(point, node.right, bestDist, bestDistNode);
    } else {
        findNearestSub(point, node.right, bestDist, bestDistNode);
        findNearestSub(point, node.left, bestDist, bestDistNode);
    }
}

/*
-------------------------------------------------
総当たりで最近接を探す
-------------------------------------------------
*/
template<typename KdTreePoint>
KdTreePoint
KdTree<KdTreePoint>::findNearestBF(Vec3 point) const
{
    // 空のツリーはそもそも探索しない
    AL_ASSERT_DEBUG(!nodes_.empty());
    //
    const NodeType* closestNode = nullptr;
    float distanceSq = std::numeric_limits<float>::max();
    for (const auto& node : nodes_) {
        const float dsq = calcDistSq(node, point);
        if (dsq < distanceSq) {
            distanceSq = dsq;
            closestNode = &node;
        }
    }
    return closestNode->point;
}

/*
-------------------------------------------------
KNN
-------------------------------------------------
*/
template<typename KdTreePoint>
float
KdTree<KdTreePoint>::findKNN(Vec3 targetPoint,
                             int32_t numSerch,
                             std::vector<const KdTreePoint*>& points) const
{
    // 探索する量よりもノードの数の方が少ない場合は失敗
    AL_ASSERT_DEBUG(numSerch < nodes_.size());
    //
    points.reserve(numSerch);
    points.clear();
    float farDist = std::numeric_limits<float>::max();
    findKNNSub(targetPoint, numSerch, 0, farDist, points);
    return farDist;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
INLINE void
KdTree<KdTreePoint>::findKNNSub(Vec3 targetPos,
                                int32_t numSerch,
                                int32_t nodeIndex,
                                float& farDist,
                                std::vector<const KdTreePoint*>& points) const
{
    //
    if (nodeIndex == -1) {
        return;
    }
    const NodeType& node = nodes_[nodeIndex];
    // 最短距離よりもノードへの距離がすでに遠い場合は棄却
    const int32_t axis = node.axis;
    AL_ASSERT_DEBUG(axis >= 0);
    const auto& bound = node.bound;
    const auto& mn = bound.min();
    const auto& mx = bound.max();
    const float d0 = mn[axis] - targetPos[axis];
    const float d1 = targetPos[axis] - mx[axis];
    if (farDist < d0 || farDist < d1) {
        return;
    }
    // 最も遠いノードを最後の要素にする
    const auto setFarNodeToLast = [&]() {
        float farDistSq = 0.0f;
        std::nth_element(points.begin(),
                         points.begin() + points.size() - 1,
                         points.end(),
                         [&targetPos, &farDistSq, this](
                           const KdTreePoint* lhs, const KdTreePoint* rhs) {
                             const float distSqL = calcDistSq(lhs, targetPos);
                             const float distSqR = calcDistSq(rhs, targetPos);
                             farDistSq = std::max(farDistSq, distSqL);
                             farDistSq = std::max(farDistSq, distSqR);
                             return distSqL < distSqR;
                         });
        const float farDist = std::sqrtf(farDistSq);
        return farDist;
    };
    // このノードのポイントがより近くなっていたら検索結果を更新
    const float diff = calcDist(node, targetPos);
    // まだ返す配列が一杯でなければ問答無用で格納
    if (points.size() < numSerch) {
        points.push_back(&node.point);
        // 一杯になったら末尾の要素を最も遠いノードにする
        if (points.size() == numSerch) {
            farDist = setFarNodeToLast();
        }
    }
    // 最も遠い要素よりも近ければ格納
    else if (diff < farDist) {
        points[points.size() - 1] = &node.point;
        farDist = setFarNodeToLast();
    }
    // 積んでいく順番はノード内で、ポイントが属している方向から。
    const Vec3 nodePos = node.point.position();
    if (targetPos[axis] < nodePos[axis]) {
        findKNNSub(targetPos, numSerch, node.left, farDist, points);
        findKNNSub(targetPos, numSerch, node.right, farDist, points);
    } else {
        findKNNSub(targetPos, numSerch, node.right, farDist, points);
        findKNNSub(targetPos, numSerch, node.left, farDist, points);
    }
}

/*
-------------------------------------------------
KNN(総当たり)
-------------------------------------------------
*/
template<typename KdTreePoint>
float
KdTree<KdTreePoint>::findKNNBF(Vec3 targetPos,
                               int32_t numSerch,
                               std::vector<const KdTreePoint*>& points) const
{
    // 探索する量よりもノードの数の方が少ない場合は失敗
    AL_ASSERT_DEBUG(numSerch < nodes_.size());
    //
    points.reserve(numSerch);
    points.clear();
    float distanceFarSq = 0.0f;
    //
    for (const NodeType& node : nodes_) {
        // 末尾の要素を最も遠いノードにする
        const auto setFarNodeToLast = [&targetPos, &points, this]() {
            std::nth_element(points.begin(),
                             points.begin() + (points.size() - 1),
                             points.end(),
                             [&targetPos, this](const KdTreePoint* lhs,
                                                const KdTreePoint* rhs) {
                                 const float dsqL = calcDistSq(lhs, targetPos);
                                 const float dsqR = calcDistSq(rhs, targetPos);
                                 return dsqL < dsqR;
                             });
        };
        //
        const float dsq = calcDistSq(node, targetPos);
        // まだ返す配列が一杯でなければ問答無用で格納
        if (points.size() < numSerch) {
            points.push_back(&node.point);
            distanceFarSq = std::max(dsq, distanceFarSq);
            // 一杯になったら末尾の要素を最も遠いノードにする
            if (points.size() == numSerch) {
                setFarNodeToLast();
            }
        }
        // 最も遠い要素よりも近ければ格納
        else if (dsq < distanceFarSq) {
            points[points.size() - 1] = &node.point;
            setFarNodeToLast();
            const KdTreePoint* lastElement = points.back();
            distanceFarSq = calcDistSq(lastElement, targetPos);
        }
    }
    return std::sqrtf(distanceFarSq);
}

#if 0
/*
-------------------------------------------------
指定のBounding内の点を全て列挙する
-------------------------------------------------
*/
template<typename KdTreePoint>
INLINE void KdTree<KdTreePoint>::requestInBounding(
    const AABB& aabb,
    std::vector<const KdTreePoint*>& points) const
{
#if 1 // ブルートフォースの方法
    for (const NodeType& node : nodes_)
    {
        aabb.
        node.point.position();
    }
#else
#endif
}
#endif

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
INLINE float
KdTree<KdTreePoint>::calcDist(const NodeType& node, Vec3 pos) const
{
    return Vec3::distance(pos, node.point.position());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
INLINE float
KdTree<KdTreePoint>::calcDist(const KdTreePoint* point, Vec3 pos) const
{
    return Vec3::distance(pos, point->position());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
INLINE float
KdTree<KdTreePoint>::calcDistSq(const NodeType& node, Vec3 pos) const
{
    return Vec3::distanceSq(pos, node.point.position());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
INLINE float
KdTree<KdTreePoint>::calcDistSq(const KdTreePoint* point, Vec3 pos) const
{
    return Vec3::distanceSq(pos, point->position());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<typename KdTreePoint>
INLINE void
KdTree<KdTreePoint>::print() const
{
    // 全てのノードの情報を書きだす
    for (int32_t i = 0; i < nodes_.size(); ++i) {
        const auto& node = nodes_[i];
        const auto& b = node.bound;
        const auto& mn = b.min();
        const auto& mx = b.max();
        const auto& p = node.point.position();
        printf("%d: \n", i);
        printf(" P:(%f,%f,%f) \n", p.x, p.y, p.z);
        printf(" L:%d \n", node.left);
        printf(" R:%d \n", node.right);
#if 0
        printf("Axis:%d \n", node.axis);
        printf("MN:(%f,%f,%f) \n", mn.x, mn.y, mn.z);
        printf("MX:(%f,%f,%f) \n", mx.x, mx.y, mx.z);
#endif
    }
}
