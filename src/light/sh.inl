#pragma once

/*
-------------------------------------------------
-------------------------------------------------
*/
template<int32_t ORDER>
SphericalHarmonicsCoeff<ORDER> SphericalHarmonicsCoeff<ORDER>::evalDirection(const Vec3& dir)
{
    SphericalHarmonicsCoeff<ORDER> sh;
    extern void _evalDirection(float* values, int32_t order, const Vec3& dir);
    _evalDirection(sh.values.data(), ORDER, dir);
    return sh;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<int32_t ORDER>
SphericalHarmonicsCoeff<ORDER> SphericalHarmonicsCoeff<ORDER>::evalDirectionalLight(const Vec3& dir)
{
    SphericalHarmonicsCoeff<ORDER> sh;
    extern void _evalDirectionalLight(float* values, int32_t order, const Vec3& dir);
    _evalDirectionalLight(sh.values.data(), ORDER, dir);
    return sh;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<int32_t ORDER>
SphericalHarmonicsCoeff<ORDER> SphericalHarmonicsCoeff<ORDER>::scale(
    const SphericalHarmonicsCoeff<ORDER>& v,
    float s)
{
    SphericalHarmonicsCoeff<ORDER> ret;
    for(int32_t i=0; i<ret.value.size(); ++i)
    {
        ret.values[i] = v.values[i] * s;
    }
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<int32_t ORDER>
SphericalHarmonicsCoeff<ORDER> SphericalHarmonicsCoeff<ORDER>::add(
    const SphericalHarmonicsCoeff<ORDER>& lhs,
    const SphericalHarmonicsCoeff<ORDER>& rhs )
{
    SphericalHarmonicsCoeff<ORDER> ret;
    for (int32_t i = 0; i<ret.values.size(); ++i)
    {
        ret.values[i] = lhs.values[i] + rhs.values[i];
    }
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<int32_t ORDER>
float SphericalHarmonicsCoeff<ORDER>::dot(
    const SphericalHarmonicsCoeff<ORDER>& lhs,
    const SphericalHarmonicsCoeff<ORDER>& rhs)
{
    float ret = 0.0f;
    const int32_t numCoeff = ORDER * ORDER;
    for (int32_t i=0;i<numCoeff;++i)
    {
        ret += lhs.values[i] * rhs.values[i];
    }
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<int32_t l, int32_t m>
float sphericalHarmonics(float theta, float phi)
{
    // (l,m)の範囲チェック
    //static_assert((-l <= m) && (m <= l),"");
    //
    const auto K = [](int al, int am) -> float
    {
        int i;
        float lpm = 1;
        float lnm = 1;

        if (am < 0)
        {
            am = -am;
        }
        for (i = al - am; 0 < i; i--)
        {
            lnm *= i;
        }
        for (i = al + am; 0 < i; i--)
        {
            lpm *= i;
        }
        return sqrtf(((2 * al + 1)*lnm) / (4 * PI*lpm));
    };
    //
    const auto P = [](int32_t al, int32_t am, float ax) -> float
    {
        //
        float pmm = 1.0f;
        //
        if (0 < am)
        {
            const float somx2 = sqrtf((1.0f - ax)*(1.0f + ax));
            float fact = 1.0f;
            for (int i = 1; i <= am; i++)
            {
                pmm *= (-fact)*somx2;
                fact += 2.0f;
            }
        }
        if (al == am)
        {
            return pmm;
        }
        //
        float pmmp1 = ax*(2.0f*am + 1.0f) * pmm;
        if (al == am + 1)
        {
            return pmmp1;
        }

        float pll = 0.0;
        for (int ll = m + 2; ll <= al; ll++)
        {
            pll = ((2.0f*ll - 1.0f)*ax*pmmp1 - (ll + am - 1.0f)*pmm) / (ll - am);
            pmm = pmmp1;
            pmmp1 = pll;
        }
        return pll;
    };

    //
    float ret = K(l, m);

    if (0 < m)
    {
        ret *= sqrt(2.0f) * cos(m*phi) * P(l, m, cos(theta));
    }
    else if (m < 0)
    {
        ret *= sqrtf(2.0f) * sin(-m*phi) * P(l, -m, cos(theta));
    }
    else if (m == 0)
    {
        ret *= P(l, m, cosf(theta));
    }

    return ret;
}
