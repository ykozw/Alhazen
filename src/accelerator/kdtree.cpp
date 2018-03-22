#include "pch.hpp"
#include "accelerator/kdtree.hpp"
#include "sampler/sampler.hpp"
#include "core/unittest.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(Accel, kdtree0)
{
    return;

    struct TestPoint
    {
    public:
        Vec3 pos;
        float dummy = 0.0f;

    public:
        TestPoint() {}
        TestPoint(const TestPoint &other)
        {
            pos = other.pos;
            dummy = other.dummy;
        }
        TestPoint(TestPoint &&other) { *this = std::move(other); }
        TestPoint &operator=(TestPoint &&other)
        {
            pos = std::move(other.pos);
            dummy = std::move(other.dummy);
            return *this;
        }
        Vec3 position() const { return pos; }
        TestPoint &operator=(const TestPoint &other) = default;
        bool operator==(const TestPoint &other) const
        {
            return (pos == other.pos);
        }
        bool operator!=(const TestPoint &other) const
        {
            return !(*this == other);
        }
    };
    const auto rp = []() {
        const auto rv = []() {
            return float(rand() % 100) / float(100.0f) - 0.5f;
        };
        return Vec3(rv(), rv(), rv());
    };
#if 0
    const auto rp1 = []()
    {
        const auto rv = []()
        {
            return float(rand() % 100) / float(100.0f) - 0.5f;
        };
        return Vec3(rv(), 0.0f, 0.0f);
    };
#endif
#if 0
    const auto rp2 = []()
    {
        const auto rv = []()
        {
            return float(rand() % 100) / float(100.0f) - 0.5f;
        };
        return Vec3(rv(), rv(), 0.0f);
    };
#endif
    std::vector<TestPoint> points;
    const int32_t np = 1024;
    for (int32_t i = 0; i < np; ++i)
    {
        TestPoint tp;
        tp.pos = rp();
        // tp.pos = rp1();
        // tp.pos = rp2();
        // tp.pos = Vec3(float(i)/float(np) - 0.5f, 0.0f,0.0f);
        points.push_back(tp);
    }
    KdTree<TestPoint> kdtree;
    kdtree.construct(points.data(), (int32_t)points.size());
    // kdtree.print();
    const int32_t numSample = 1024;
    for (int32_t i = 0; i < numSample; ++i)
    {
        const Vec3 v = rp();
        const TestPoint kp0 = kdtree.findNearest(v);
        const TestPoint kp1 = kdtree.findNearestBF(v);
        const float dist0 = Vec3::distance(v, kp0.pos);
        const float dist1 = Vec3::distance(v, kp1.pos);
        if ((kp0 != kp1) && (dist0 != dist1))
        {
            printf("OMG\n");
        }
        AL_ASSERT_DEBUG(dist0 == dist1);
    }

    for (int32_t i = 0; i < 1024; ++i)
    {
        const Vec3 v = rp();
        std::vector<const TestPoint *> points0;
        std::vector<const TestPoint *> points1;
        const float farDist0 = kdtree.findKNN(v, 3, points0);
        const float farDist1 = kdtree.findKNNBF(v, 3, points1);
#if 0
        //
        const auto pred = [](const TestPoint* lhs, const TestPoint* rhs)
        {
            const Vec3 lp = lhs->pos;
            const Vec3 rp = rhs->pos;
            if (lp.x != rp.x)
            {
                return lp.x < rp.x;
            }
            else if (lp.y != rp.y)
            {
                return lp.y < rp.y;
            }
            else
            {
                return lp.z < rp.z;
            }
        };
        std::sort(points0.begin(), points0.end(), pred);
        std::sort(points1.begin(), points1.end(), pred);
        const bool same = std::equal(points0.begin(), points0.end(), points1.begin(),
            [&v](const TestPoint* lhs, const TestPoint* rhs)
        {
            const float dist0 = Vec3::distanceSq(v, lhs->pos);
            const float dist1 = Vec3::distanceSq(v, rhs->pos);
            return dist0 == dist1;
        });
#else
        const bool same = (farDist0 == farDist1);
#endif
        if (!same)
        {
            printf("OMG\n");
        }
        AL_ASSERT_DEBUG(same);
    }
    // printf("End\n");
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(Accel, kdtree1)
{
    return;
    //
    struct Point : public Vec3
    {
    public:
        Point() = default;
        Point(float x, float y, float z) : Vec3(x, y, z) {}
        Vec3 position() const { return *this; }
    };
    std::vector<Point> points;

    XorShift128 rng;
    for (int32_t i = 0; i < 1024; ++i)
    {
        Point v(rng.nextFloat(), rng.nextFloat(), rng.nextFloat());
        points.push_back(v);
    }
    KdTree<Point> kdtree;
    kdtree.construct(points.data(), int32_t(points.size()));
    //
    bool fail = false;
    for (int32_t i = 0; i < 128; ++i)
    {
        const Vec3 serch(rng.nextFloat(), rng.nextFloat(), rng.nextFloat());
        // この場で全探索(ソートして上から取る)
        std::sort(points.begin(),
                  points.end(),
                  [serch](const Point &lhs, const Point &rhs) {
                      return Vec3::distance(serch, lhs) <
                             Vec3::distance(serch, rhs);
                  });
        //
        std::vector<const Point *> result0;
        kdtree.findKNNBF(serch, 16, result0);
        for (int32_t i = 0; i < result0.size(); ++i)
        {
            const bool same = (*result0[i] == points[i]);
            AL_ASSERT_DEBUG(same);
            fail |= !same;
        }
        //
        std::vector<const Point *> result1;
        kdtree.findKNN(serch, 16, result1);
        for (int32_t i = 0; i < result1.size(); ++i)
        {
            const bool same = (*result1[i] == points[i]);
            AL_ASSERT_DEBUG(same);
            fail |= !same;
        }
    }
    AL_ASSERT_ALWAYS(!fail);
}
