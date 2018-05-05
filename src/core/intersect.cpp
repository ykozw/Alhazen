#include "core/intersect.hpp"
#include "core/unittest.hpp"
#include "core/ray.hpp"
#include "core/vdbmt.hpp"


/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool solveQuadratic_0(float a, float b, float c, float* t0, float* t1)
{
    const float D = b * b - 4.0f * a * c;
    // 解なし
    if (D < 0.0f)
    {
        return false;
    }
    const float Dsqrt = std::sqrtf(D);
    float q;
    if (float(b) < 0.0f)
    {
        q = -0.5f * (b - Dsqrt);
    }
    else
    {
        q = -0.5f * (b + Dsqrt);
    }
    *t0 = q / a;
    *t1 = c / q;
    if (float(*t1) < float(*t0))
    {
        std::swap(*t0, *t1);
    }
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool solveQuadratic_1(float a, float b, float c, float* t0, float* t1)
{
    const float D = b * b - 4.0f * a * c;
    // 解なし
    if (D < 0.0f)
    {
        return false;
    }
    const float Dsqrt = std::sqrtf(D);

    if (float(b) >= 0.0f)
    {
        const float tmp = (-b - Dsqrt);
        *t0 = tmp /(2.0f*a);
        *t1 = (2.0f*c)/ tmp;
    }
    else
    {
        const float tmp = (-b + Dsqrt);
        *t0 = (2.0f*c) / tmp;
        *t1 = tmp / (2.0f*a);
    }
    return true;
}

AL_TEST(Isect, 0)
{
    //
    vdbmt_frame();
    //
    const Vec3 so(2000.0f, 0.0f, 0.0f);
    const ErrFloat r(1.0f);
    const ErrFloat r2 = r * r;
    //
    const int32_t numSample = 128;
    for (int32_t i= numSample-80;i<numSample;++i)
    {
        const float y = float(i) / float(numSample);
        const Ray ray(Vec3(0.0f, y, 0.0f), Vec3(1.0f, 0.0f, 0.0f));
        Intersect isect, isect1;
        isect.clear();
        isect1.clear();
        const bool hit = intersectSphere(ray, so, r2, &isect);
        if (hit)
        {
            // 二回交差する
            /*const Vec3 pp = isect.t * ray.d + ray.o;
            isect.clear();
            const Ray ray2(pp, Vec3(1.0f, 0.0f, 0.0f));
            intersectSphere2(ray2, so, r2, &isect);*/
            //
            vdbmt_color(Vec3(0.0f,1.0f,0.0f));
            // めり込んでいる場合
            auto t = long double(isect.t);
            auto dx = long double(ray.d.x());
            auto dy = long double(ray.d.y());
            auto dz = long double(ray.d.z());
            auto ox = long double(ray.o.x());
            auto oy = long double(ray.o.y());
            auto oz = long double(ray.o.z());
            auto sox = long double(so.x());
            auto soy = long double(so.y());
            auto soz = long double(so.z());
            auto x = dx * t + ox;
            auto y = dy * t + oy;
            auto z = dz * t + oz;
            auto ddx = (sox - x);
            auto ddy = (soy - y);
            auto ddz = (soz - z);
            if ((ddx*ddx + ddy*ddy + ddz*ddz) < 1.0)
            {
                vdbmt_color(Vec3(1.0f, 0.0f, 0.0f));
            }
            //
            const Vec3 p0(so.x() - 2.0f, y, 0.0f);
            const Vec3 p1 = isect.t * ray.d + ray.o;
            vdbmt_line(p0, p1);
        }
        else
        {
            vdbmt_color(Vec3(0.0f, 0.0f, 1.0f));
            //
            const Vec3 p0(so.x() - 2.0f, y, 0.0f);
            const Vec3 p1(so.x() + 0.0f, y, 0.0f);
            vdbmt_line(p0, p1);
        }
        // 
        
    }
    exit(0);
}
