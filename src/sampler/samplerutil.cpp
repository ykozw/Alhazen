#include "core/rng.hpp"
#include "core/math.hpp"
#include "accelerator/kdtree.hpp"

/*
-------------------------------------------------
star discrepancyを推定する
-------------------------------------------------
*/
float estimateStarDiscrepancy(const std::vector<Vec2>& srcSamples)
{
#if 0
    /*
    kd-treeで次のことができるようになったら置き換える
    - 任意次元
    - 指定したbounding内のリスト
    */
    // TODO: kd-treeの構築
    struct KDP
    {
        Vec2 pos;
        Vec3 position() const
        {
            return Vec3(pos.x, pos.y, 0.0f);
        }
    };
    static_assert(sizeof(KDP) == sizeof(Vec2), "");
    KdTree<KDP> kdtree;
    kdtree.construct((KDP*)&samples, sizeof(Vec2)*samples.size());
#else
    //
    std::vector<Vec2> samples(srcSamples);
    std::sort(
        samples.begin(), samples.end(), [](const Vec2& lhs, const Vec2& rhs) {
            //
            if (lhs.x() < rhs.x())
            {
                return true;
            }
            else if (lhs.x() > rhs.x())
            {
                return false;
            }
            //
            else if (lhs.y() < rhs.y())
            {
                return true;
            }
            else if (lhs.y() > rhs.y())
            {
                return false;
            }
            //
            return false;
        });
#endif
    //
    XorShift128 rng; // HACK: seed固定
    float maxDiscrepancy = 0.0f;
    const int32_t numSample = 128; // HACK: 固定
    for (int32_t i = 0; i < numSample; ++i)
    {
        // 2サンプルを抽出する
        const int32_t si0 = rng.nextSize((int32_t)samples.size());
        const int32_t si1 = rng.nextSize((int32_t)samples.size());
        const Vec2 s0 = samples[si0];
        const Vec2 s1 = samples[si1];
        const Vec2 mn =
            Vec2(std::min(s0.x(), s1.x()), std::min(s0.y(), s1.y()));
        const Vec2 mx =
            Vec2(std::max(s0.x(), s1.x()), std::max(s0.y(), s1.y()));
        //
        // TODO: そのサンプルの範囲内に含まれるサンプル数を算出
        const auto beginX =
            std::lower_bound(samples.begin(),
                             samples.end(),
                             mn,
                             [](const Vec2& lhs, const Vec2& rhs) {
                                 if (lhs.x() < rhs.x())
                                 {
                                     return true;
                                 }
                                 else if (lhs.x() > rhs.x())
                                 {
                                     return false;
                                 }
                                 return false;
                             });
        const auto lastX =
            std::upper_bound(samples.begin(),
                             samples.end(),
                             mn,
                             [](const Vec2& lhs, const Vec2& rhs) {
                                 if (lhs.x() < rhs.x())
                                 {
                                     return true;
                                 }
                                 else if (lhs.x() > rhs.x())
                                 {
                                     return false;
                                 }
                                 return false;
                             });
        int32_t count = 0;
        for (auto ite = beginX; ite != lastX; ++ite)
        {
            if ((mn.y() <= ite->y()) && (ite->y() <= mx.y()))
            {
                ++count;
            }
        }
        // 最初と最後は必ず含まれてしまうから2を取り除く
        count -= 2;
        //
        const float area = (mx.x() - mn.x()) * (mx.y() - mn.y());
        maxDiscrepancy = std::max((float(count) / float(samples.size()) - area),
                                  maxDiscrepancy);
    }
    return maxDiscrepancy;
}

/*
-------------------------------------------------
最小の距離を得る
-------------------------------------------------
*/
float minimumDisptance(const std::vector<Vec2>& samples)
{
    // kd-treeの構築
    struct KDP
    {
        Vec2 pos;
        Vec3 position() const { return Vec3(pos.x(), pos.y(), 0.0f); }
    };
    static_assert(sizeof(KDP) == sizeof(Vec2), "");
    KdTree<KDP> kdtree;
    kdtree.construct((KDP*)&samples, int32_t(sizeof(Vec2) * samples.size()));
    // 全てのサンプル点に対して最小距離を出しその中での最小距離を出す
    float minMinDist = std::numeric_limits<float>::max();
    for (const Vec2& s : samples)
    {
        float minDist = 0.0;
        AL_ASSERT_DEBUG(
            false); // TODO:
                    // 現在の方法だとサンプル点自身が返されてしまう。どうにかする。
        kdtree.findNearest(Vec3(s.x(), s.y(), 0.0f), &minDist);
        minMinDist = std::min(minMinDist, minDist);
    }
    return minMinDist;
}
