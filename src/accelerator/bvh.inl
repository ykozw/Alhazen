#include "pch.hpp"
#include "bvh.hpp"
#include "core/intersect.hpp"
#include "shape/shape.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool BruteForceBVH::intersect(const Ray& ray, Intersect* isect) const
{
    bool isHit = false;
    // 全ての三角形を見ていく
    for (auto& f : fs_)
    {
        const Vec3 v0 = vs_[f.vi[0]];
        const Vec3 v1 = vs_[f.vi[1]];
        const Vec3 v2 = vs_[f.vi[2]];
        const Vec3 n0 = ns_[f.vi[0]];
        const Vec3 n1 = ns_[f.vi[1]];
        const Vec3 n2 = ns_[f.vi[2]];
        const Vec2 t0 = ts_[f.ti[0]];
        const Vec2 t1 = ts_[f.ti[1]];
        const Vec2 t2 = ts_[f.ti[2]];
        //
        if (intersectTriangle(ray, v0, v1, v2, n0, n1, n2, t0, t1, t2, isect))
        {
            isHit = true;
        }
    }
    return isHit;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool BruteForceBVH::intersectCheck(const Ray& ray) const
{
    bool isHit = false;
    // 全ての三角形を見ていく
    for (auto& f : fs_)
    {
        const Vec3 v0 = vs_[f.vi[0]];
        const Vec3 v1 = vs_[f.vi[1]];
        const Vec3 v2 = vs_[f.vi[2]];
        //
        if (intersectTriangleCheck(ray, v0, v1, v2))
        {
            return true;
        }
    }
    return isHit;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE AABB SimpleBVH::aabb() const
{
    AL_ASSERT_DEBUG(nodes_.size() != 0);
    return nodes_[0].aabb;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t SimpleBVH::maxDepth() const
{
    return maxDepth_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool SimpleBVH::intersect(const Ray& ray, Intersect* isect) const
{
    return intersectSub(0, ray, isect);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool SimpleBVH::intersectCheck(const Ray& ray) const
{
    // TODO: もう少しちゃんと計算を省いたものを入れる
    Intersect isect;
    return intersectSub(0, ray, &isect);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool SimpleBVH::intersectSub(int32_t nodeIndex, const Ray& ray, Intersect* isect) const
{
    /*
    スタックレス版の方が2割ほど低速であったため再帰版を採用する
    */
#if 1
    // スタック利用版
    const auto& node = nodes_[nodeIndex];
    // このAABBに交差しなければ終了
    if (!node.aabb.intersectCheck(ray, isect->t))
    {
        return false;
    }
    // 葉の場合は、ノードの三角形と交差判定
    else if (node.childlen[0] == -1)
    {
        auto& v = node.v;
        auto& n = node.n;
        auto& t = node.t;
        if (intersectTriangle(
            ray,
            v[0], v[1], v[2],
            n[0], n[1], n[2],
            t[0], t[1], t[2],
            isect ))
        {
            //isect.materialId = node.m;
            return true;
        }
        return false;
    }
    // 枝の場合は、子を見に行く
    else
    {
        const bool h0 = intersectSub(node.childlen[0], ray, isect);
        const bool h1 = intersectSub(node.childlen[1], ray, isect);
        return h0 || h1;
    }
#else
    // スタックレスバージョン
    (void)nodeIndex;
    //int32_t nodeIndexStack[16];
    std::array<int32_t, 16> nodeIndexStack;
    int32_t nodeDepth = 1;
    nodeIndexStack[0] = 0;
    //
    while (nodeDepth != 0)
    {
        const int32_t nextNodeIndex = nodeIndexStack[nodeDepth - 1];
        --nodeDepth;
        const auto& node = nodes_[nextNodeIndex];
        // このAABBに交差しなければ終了
        if (!node.aabb.intersect(ray, contact.mint))
        {
            continue;
        }
        // 葉だったら、ノードの三角形と交差判定
        else if (node.childlen[0] == -1)
        {
            auto& v = node.v;
            Triangle::intersectTriangle(ray, v[0], v[1], v[2], contact);
            continue;
        }
        // 枝であれば子を見に行く
        else
        {
            nodeIndexStack[nodeDepth] = node.childlen[0];
            ++nodeDepth;
            nodeIndexStack[nodeDepth] = node.childlen[1];
            ++nodeDepth;
        }
    }
#endif
}

/*
-------------------------------------------------
intersectAABBSIMD()
下位4bitに交差したか否かが格納されている
-------------------------------------------------
*/
INLINE int32_t testAABBSIMD(
    const QBVHNode& node,
    RaySIMD& ray,
    IntersectSIMD& isect
    )
{
    //
    const auto& aabbs = node.aabbs;
    const auto& sign = ray.sign;
    const auto& o = ray.o;
    const auto& dinv = ray.dinv;
    __m128 mint = ray.mint;
    __m128 maxt = isect.t;
    // x
    mint = _mm_max_ps(mint, _mm_mul_ps(_mm_sub_ps(aabbs[sign[0]][0], o[0]), dinv[0]));
    maxt = _mm_min_ps(maxt, _mm_mul_ps(_mm_sub_ps(aabbs[1 - sign[0]][0], o[0]), dinv[0]));
    // y
    mint = _mm_max_ps(mint, _mm_mul_ps(_mm_sub_ps(aabbs[sign[1]][1], o[1]), dinv[1]));
    maxt = _mm_min_ps(maxt, _mm_mul_ps(_mm_sub_ps(aabbs[1 - sign[1]][1], o[1]), dinv[1]));
    // z
    mint = _mm_max_ps(mint, _mm_mul_ps(_mm_sub_ps(aabbs[sign[2]][2], o[2]), dinv[2]));
    maxt = _mm_min_ps(maxt, _mm_mul_ps(_mm_sub_ps(aabbs[1 - sign[2]][2], o[2]), dinv[2]));
    //
    const int32_t hitMask = _mm_movemask_ps(_mm_cmpge_ps(maxt, mint));
    return hitMask;
}

/*
-------------------------------------------------
intersectAABBSIMD()
下位4bitに交差したか否かが格納されている
-------------------------------------------------
*/
INLINE int32_t testAABB(
    const QBVHNode& node,
    RaySIMD& raySIMD
    )
{
    //
    const auto& aabbs = node.aabbs;
    ALIGN32 float mnx[4];
    ALIGN32 float mny[4];
    ALIGN32 float mnz[4];
    ALIGN32 float mxx[4];
    ALIGN32 float mxy[4];
    ALIGN32 float mxz[4];
    _mm_store_ps(mnx, aabbs[0][0]);
    _mm_store_ps(mny, aabbs[0][1]);
    _mm_store_ps(mnz, aabbs[0][2]);
    _mm_store_ps(mxx, aabbs[1][0]);
    _mm_store_ps(mxy, aabbs[1][1]);
    _mm_store_ps(mxz, aabbs[1][2]);
    //
    const auto& ray = raySIMD.origRay;
    //
    AABB aabb[4];
    for (int32_t i = 0; i < 4; ++i)
    {
        aabb[i].addPoint(Vec3(mnx[i], mny[i], mnz[i]));
        aabb[i].addPoint(Vec3(mxx[i], mxy[i], mxz[i]));
    }
    const int32_t hit0 = aabb[0].intersectCheck(ray, ray.maxt );
    const int32_t hit1 = aabb[1].intersectCheck(ray, ray.maxt );
    const int32_t hit2 = aabb[2].intersectCheck(ray, ray.maxt );
    const int32_t hit3 = aabb[3].intersectCheck(ray, ray.maxt );
    //
    const int32_t hitMask = (hit3 << 3) | (hit2 << 2) | (hit1 << 1) | (hit0 << 0);
    return hitMask;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_FORCEINLINE __m128 select(__m128 v0, __m128 v1, __m128 mask)
{
    return _mm_or_ps(
        _mm_andnot_ps(mask, v0),
        _mm_and_ps(v1, mask));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool QBVH::intersect(
    const Ray& ray,
    Intersect* isect,
    int8_t* materialId ) const
{
    // SIMD用レイ
    RaySIMD raySIMD(ray);
    IntersectSIMD intersectRecordSIMD;
    intersectRecordSIMD.upadte(ray.maxt);
    const auto& sign = ray.sign;
    // 定数
    const __m128 zero = _mm_set_ps1(0.0f);
    const __m128 keps = _mm_set_ps1(1e-5f);
    const __m128 one = _mm_set_ps1(1.0f);
    const __m128 minusone = _mm_set_ps1(-1.0f);
    //
    bool isHit = false;
    //
    std::array<int32_t, 64> nodeStack;
    int32_t nodeStackSize = 0;
    nodeStack[nodeStackSize] = 0;
    ++nodeStackSize;
    //
    while (nodeStackSize != 0)
    {
        // 一番上のノードを取得する
        const int32_t nodeIndex = nodeStack[nodeStackSize - 1];
        --nodeStackSize;
        auto& node = nodes_[nodeIndex];
        // 同時に4つのAABBに対して交差判定
        const int32_t hitMask = testAABBSIMD(node, raySIMD, intersectRecordSIMD);
#if 0 // for check
        assert(hitMask == testAABB(node, raySIMD));
#endif
        // 何も衝突していなければ次のノードへ
        if (hitMask == 0)
        {
            continue;
        }
        /*
        衝突したノードをスタックに積んでいく
        スタックに積む順番はレイの射出方向の逆方向順
        */
        static const int32_t ORDER_TABLE[16][8] =
        {
            { 0x44444, 0x44444, 0x44444, 0x44444, 0x44444, 0x44444, 0x44444, 0x44444 },
            { 0x44440, 0x44440, 0x44440, 0x44440, 0x44440, 0x44440, 0x44440, 0x44440 },
            { 0x44441, 0x44441, 0x44441, 0x44441, 0x44441, 0x44441, 0x44441, 0x44441 },
            { 0x44401, 0x44401, 0x44410, 0x44410, 0x44401, 0x44401, 0x44410, 0x44410 },
            { 0x44442, 0x44442, 0x44442, 0x44442, 0x44442, 0x44442, 0x44442, 0x44442 },
            { 0x44402, 0x44402, 0x44402, 0x44402, 0x44420, 0x44420, 0x44420, 0x44420 },
            { 0x44412, 0x44412, 0x44412, 0x44412, 0x44421, 0x44421, 0x44421, 0x44421 },
            { 0x44012, 0x44012, 0x44102, 0x44102, 0x44201, 0x44201, 0x44210, 0x44210 },
            { 0x44443, 0x44443, 0x44443, 0x44443, 0x44443, 0x44443, 0x44443, 0x44443 },
            { 0x44403, 0x44403, 0x44403, 0x44403, 0x44430, 0x44430, 0x44430, 0x44430 },
            { 0x44413, 0x44413, 0x44413, 0x44413, 0x44431, 0x44431, 0x44431, 0x44431 },
            { 0x44013, 0x44013, 0x44103, 0x44103, 0x44301, 0x44301, 0x44310, 0x44310 },
            { 0x44423, 0x44432, 0x44423, 0x44432, 0x44423, 0x44432, 0x44423, 0x44432 },
            { 0x44023, 0x44032, 0x44023, 0x44032, 0x44230, 0x44320, 0x44230, 0x44320 },
            { 0x44123, 0x44132, 0x44123, 0x44132, 0x44231, 0x44321, 0x44231, 0x44321 },
            { 0x40123, 0x40132, 0x41023, 0x41032, 0x42301, 0x43201, 0x42310, 0x43210 }
        };
        const int32_t rayDirIndex = (sign[node.axisTop] << 2) | (sign[node.axisLeft] << 1) | (sign[node.axisRight]);
        int32_t order = ORDER_TABLE[hitMask][rayDirIndex];
        for (int32_t i = 0;
            (i < 4) && !(order & 0x04);
            ++i)
        {
            const int32_t childIndex = order & 0x03;
            const auto& cidx = node.childIndex[childIndex];
            order >>= 4;
            // 葉であった場合はこの場で交差判定
            if (cidx.isLeaf)
            {
                const int32_t leafIndex = cidx.index;
                //
                const QBVHLeaf& leaf = leafs_[leafIndex];
#if 0 // 四枚のLeafをすべて見ていく
                for (int32_t leafNo = 0; leafNo < 4; ++leafNo)
                {
                    const auto& vs = leaf.v[leafNo];
                    const auto& ns = leaf.n[leafNo];
                    const auto& ts = leaf.t[leafNo];
                    if (intersectTriangle(
                        ray,
                        vs[0], vs[1], vs[2],
                        ns[0], ns[1], ns[2],
                        ts[0], ts[1], ts[2],
                        isect) )
                    {
                        //logging("%d", leafNo);
                    }
                }

#else
                const float eps = 1e-4f; // TODO: この値はどうやって決まるのか？
                __m128 t0 = _mm_set_ps1(0.0f - eps);
                __m128 t1 = _mm_set_ps1(1.0f + eps);
                __m128 e1_x = _mm_sub_ps(leaf.x[1], leaf.x[0]);
                __m128 e1_y = _mm_sub_ps(leaf.y[1], leaf.y[0]);
                __m128 e1_z = _mm_sub_ps(leaf.z[1], leaf.z[0]);
                __m128 e2_x = _mm_sub_ps(leaf.x[2], leaf.x[0]);
                __m128 e2_y = _mm_sub_ps(leaf.y[2], leaf.y[0]);
                __m128 e2_z = _mm_sub_ps(leaf.z[2], leaf.z[0]);
                __m128 s1_x = _mm_sub_ps(_mm_mul_ps(raySIMD.d[1], e2_z), _mm_mul_ps(raySIMD.d[2], e2_y));
                __m128 s1_y = _mm_sub_ps(_mm_mul_ps(raySIMD.d[2], e2_x), _mm_mul_ps(raySIMD.d[0], e2_z));
                __m128 s1_z = _mm_sub_ps(_mm_mul_ps(raySIMD.d[0], e2_y), _mm_mul_ps(raySIMD.d[1], e2_x));
                __m128 divisor = _mm_add_ps(_mm_add_ps(_mm_mul_ps(s1_x, e1_x), _mm_mul_ps(s1_y, e1_y)), _mm_mul_ps(s1_z, e1_z));
                __m128 no_hit = _mm_cmpeq_ps(divisor, zero);
                __m128 invDivisor = _mm_rcp_ps(divisor);
                __m128 d_x = _mm_sub_ps(raySIMD.o[0], leaf.x[0]);
                __m128 d_y = _mm_sub_ps(raySIMD.o[1], leaf.y[0]);
                __m128 d_z = _mm_sub_ps(raySIMD.o[2], leaf.z[0]);
                __m128 b1 = _mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(d_x, s1_x), _mm_mul_ps(d_y, s1_y)), _mm_mul_ps(d_z, s1_z)), invDivisor);
                no_hit = _mm_or_ps(no_hit, _mm_or_ps(_mm_cmplt_ps(b1, t0), _mm_cmpgt_ps(b1, t1)));
                __m128 s2_x = _mm_sub_ps(_mm_mul_ps(d_y, e1_z), _mm_mul_ps(d_z, e1_y));
                __m128 s2_y = _mm_sub_ps(_mm_mul_ps(d_z, e1_x), _mm_mul_ps(d_x, e1_z));
                __m128 s2_z = _mm_sub_ps(_mm_mul_ps(d_x, e1_y), _mm_mul_ps(d_y, e1_x));
                __m128 b2 = _mm_mul_ps(
                    _mm_add_ps(_mm_add_ps(_mm_mul_ps(raySIMD.d[0], s2_x), _mm_mul_ps(raySIMD.d[1], s2_y)),
                    _mm_mul_ps(raySIMD.d[2], s2_z)), invDivisor);
                no_hit = _mm_or_ps(no_hit, _mm_or_ps(_mm_cmplt_ps(b2, t0), _mm_cmpgt_ps(_mm_add_ps(b1, b2), t1)));
                __m128 t = _mm_mul_ps(
                    _mm_add_ps(_mm_add_ps(_mm_mul_ps(e2_x, s2_x), _mm_mul_ps(e2_y, s2_y)),
                    _mm_mul_ps(e2_z, s2_z)), invDivisor);
                no_hit = _mm_or_ps(no_hit, _mm_cmplt_ps(t, keps));
                const int32_t nohitmask = _mm_movemask_ps(no_hit);
                ALIGN32 float t_f[4];
                _mm_store_ps(t_f, t);
                //
                __m128 isFlip = select(minusone, one, _mm_cmpge_ps(divisor, zero));
                ALIGN32 float isFlipF[4];
                _mm_store_ps(isFlipF, isFlip);
                //
                int32_t hitTriangleIndex = -1;
                Vec2 hitUVOnTri;
                for (int i = 0; i < 4; ++i)
                {
                    if ((nohitmask & (1 << i)) == 0 && isect->t > t_f[i])
                    {
                        //
                        hitTriangleIndex = i;
                        isect->t = t_f[i];
                        intersectRecordSIMD.upadte(isect->t);
                        //
                        ALIGN32 float b1f[4];
                        ALIGN32 float b2f[4];
                        _mm_store_ps(b1f, b1);
                        _mm_store_ps(b2f, b2);
                        hitUVOnTri.setX(b1f[i]);
                        hitUVOnTri.setY(b2f[i]);
                    }
                }
                // 衝突していたらそれにを衝突情報に記録
                if (hitTriangleIndex != -1)
                {
                    const auto& vs = leaf.v[hitTriangleIndex];
                    const auto& ns = leaf.n[hitTriangleIndex];
                    const auto& ts = leaf.t[hitTriangleIndex];
                    *materialId = leaf.m[hitTriangleIndex];
                    const float u = hitUVOnTri.x();
                    const float v = hitUVOnTri.y();
                    isect->normal =
                        ns[0] * (1.0f - u - v) +
                        ns[1] * u +
                        ns[2] * v;
                    //
                    isect->normal *= isFlipF[hitTriangleIndex];
                    //
                    isect->normal.normalize();
                    isect->position =
                        vs[0] * (1.0f - u - v) +
                        vs[1] * u +
                        vs[2] * v;
                    isect->uv =
                        ts[0] * (1.0f - u - v) +
                        ts[1] * u +
                        ts[2] * v;
                    //
                    
                    isect->uv.setX( alClamp(isect->uv.x(), 0.0f, 1.0f) );
                    isect->uv.setY( alClamp(isect->uv.y(), 0.0f, 1.0f) );
                }
#endif
            }
            // 節であった場合はスタックに積む
            else
            {
                ++nodeStackSize;
                nodeStack[nodeStackSize - 1] = cidx.index;
            }
        }
    }
    return isHit;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool QBVH::intersectCheck(const Ray& ray) const
{
    // TODO: 交差判定だけであれば、UVからの補間計算は要らないので、その部分を削除できるようにする
    Intersect ir;
    int8_t materialId;
    return intersect(ray, &ir, &materialId);
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool ShapeBVH::intersect(const Ray& ray, Intersect* isect) const
{
    return intersectSub(0, ray, isect);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool ShapeBVH::intersectCheck(const Ray& ray) const
{
    return intersectCheckSub(0, ray);
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool ShapeBVH::intersectSub(int32_t nodeIndex, const Ray& ray, Intersect* isect) const
{
    // スタック利用版
    const auto& node = nodes_[nodeIndex];
    // このAABBに交差しなければ終了
    if (!node.aabb.intersectCheck(ray, isect->t))
    {
        return false;
    }
    // 葉の場合は、ノードの三角形と交差判定
    else if (node.childlen[0] == -1)
    {
        const bool isHit = node.shape->intersect(ray, isect);
        if(isHit)
        {
            isect->sceneObject = node.shape.get();
        }
        return isHit;
    }
    // 枝の場合は、子を見に行く
    else
    {
        const bool h0 = intersectSub(node.childlen[0], ray, isect);
        const bool h1 = intersectSub(node.childlen[1], ray, isect);
        return h0 || h1;
    }
}


/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool ShapeBVH::intersectCheckSub(int32_t nodeIndex, const Ray& ray) const
{
    //
    const auto& node = nodes_[nodeIndex];
    // このAABBに交差しなければ終了
    if (!node.aabb.intersectCheck(ray, std::numeric_limits<float>::max()))
    {
        return false;
    }
    // 葉の場合は、ノードの三角形と交差判定
    else if (node.childlen[0] == -1)
    {
        return node.shape->intersectCheck(ray);
    }
    // 枝の場合は、子を見に行く
    else
    {
        const bool h0 = intersectCheckSub(node.childlen[0], ray);
        const bool h1 = intersectCheckSub(node.childlen[1], ray);
        return h0 || h1;
    }
}
