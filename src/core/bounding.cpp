#include "core/bounding.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
Convexhull2d::Convexhull2d(const std::vector<Vec2>& aPoints)
{
#if 0
    //
    std::vector<Vec2> points = aPoints;
    if (points.size() < 3)
    {
        return;
    }

    const auto calcMinMax = [&](std::function<float(int32_t pi)> evalFunc) -> std::tuple<int32_t, int32_t>
    {
        float mn = std::numeric_limits<float>::max();
        float mx = std::numeric_limits<float>::lowest();
        int32_t mni = -1;
        int32_t mxi = -1;
        for (int32_t pn = 0; pn < points.size(); ++pn)
        {
            const float v = evalFunc(pn);
            if (v < mn)
            {
                mni = pn;
                mn = v;
            }
            if (mx < v)
            {
                mxi = pn;
                mx = v;
            }
        }
        return {mni,mxi};
    };

    //
    // TODO: Quick hul
    const auto xmnmx = calcMinMax([&](int32_t pi) { return points[pi].x(); });

    // 最もその線から遠い二点を出す
    const Vec2 line = (points[std::get<1>(xmnmx)] - points[std::get<0>(xmnmx)]).normalized();
    const Vec2 dir = Vec2(line.y(), -line.x());
    const auto xmnmx2 = calcMinMax([&](int32_t pi) {
        const Vec2 newline = points[pi] - points[std::get<0>(xmnmx)];
        return Vec2::dot(dir, newline);
    });

    // これまでに決まった4点の中にある点は全て取り除く
#endif
}
