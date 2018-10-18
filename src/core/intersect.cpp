#include "core/intersect.hpp"
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
