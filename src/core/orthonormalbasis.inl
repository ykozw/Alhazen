#include "pch.hpp"
#include "core/orthonormalbasis.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
OrthonormalBasis<METHOD>::OrthonormalBasis()
{

}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
OrthonormalBasis<METHOD>::OrthonormalBasis(const Vec3& an)
{
    set(an);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
OrthonormalBasis<METHOD>::OrthonormalBasis(const Vec3& an, const Vec3& at)
{
    set(an, at);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
void OrthonormalBasis<METHOD>::set(const Vec3& an)
{
    AL_ASSERT_DEBUG(an.isNormalized());
    Vec3 n, s, t;
    n = an;
    //
    switch (METHOD)
    {
    case ONB_METHOD::Naive:
        {
            if (fabsf(n.x()) < 0.99f)
            {
                s = Vec3::cross(n, Vec3(1.0f, 0.0f, 0.0));
            }
            else
            {
                s = Vec3::cross(n, Vec3(0.0f, 1.0f, 0.0f));
            }
            s.normalize();
            t = Vec3::cross(s, n);
        }
        break;
    case ONB_METHOD::Moller99:
        {
            if (fabs(n.x()) > fabs(n.z()))
            {
                t = Vec3(-n.y(), n.x(), 0.0f);
            }
            else
            {
                t = Vec3(0.0f, -n.z(), n.y());
            }
            t *= 1.0f / std::sqrtf(Vec3::dot(t, t));
            s = Vec3::cross(t, n);
        }
        break;
    case ONB_METHOD::Frisvad12:
        {
            if (n.z() < -0.9999999f)
            {
                s = Vec3(0.0f, -1.0f, 0.0f);
                t = Vec3(-1.0f, 0.0f, 0.0f);
            }
            else
            {
                const float a = 1.0f / (1.0f + n.z());
                const float b = -n.x() * n.y() * a;
                s = Vec3(1.0f - n.x() * n.x() * a, b, -n.x());
                t = Vec3(b, 1.0f - n.y() * n.y() * a, -n.y());
            }
        }
        break;
    case ONB_METHOD::Duff17:
        {
            const float sign = copysignf(1.0f, n.z());
            const float a = -1.0f / (sign + n.z());
            const float b = n.x() * n.y() * a;
            t = Vec3(1.0f + sign * n.x() * n.x() * a, sign * b, -sign * n.x());
            s = Vec3(b, sign + n.y() * n.y() * a, -n.y());
        }
        break;
    case ONB_METHOD::Nelson17:
        {
            const double dthreshold = -0.9999999999776;
            float rthreshold = -0.7f;
            if (n.z() >= rthreshold)
            {
                const float a = 1.0f / (1.0f + n.z());
                const float b = -n.x() * n.y() * a;
                t = Vec3(1.0f - n.x() * n.x() * a, b, -n.x());
                s = Vec3(b, 1.0f - n.y() * n.y() * a, -n.y());
            }
            else
            {
                double x = (double)n.x();
                double y = (double)n.y();
                double z = (double)n.z();
                const double d = 1.0 / std::sqrt(x*x + y*y + z*z);
                x *= d;
                y *= d;
                z *= d;
                if (z >= dthreshold)
                {
                    const double a = 1.0 / (1.0 + z);
                    const double b = -x*y*a;
                    t = Vec3(1.0f - (float)(x*x*a), (float)b, (float)(-x));
                    s = Vec3((float)b, 1.0f - (float)(y*y*a), (float)(-y));
                }
                else
                {
                    t = Vec3(0.0f, -1.0f, 0.0f);
                    s = Vec3(-1.0f, 0.0f, 0.0f);
                }
            }
        }
        break;
    default:
        AL_ASSERT_ALWAYS(false);
        break;
    }
    //
    nts.set(s, t, n);
    //
    updateNTS();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
void OrthonormalBasis<METHOD>::set(const Vec3& an, const Vec3& at)
{
    AL_ASSERT_DEBUG(an.isNormalized());
    AL_ASSERT_DEBUG(at.isNormalized());
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(an, at)) < 0.0001f);
    // 法線と接ベクトルからローカル座標を作る
    // 与えられた法線と接ベクトルは正規化されていることを想定している
    // 与えられた法線と接ベクトルは直交していない場合もあることを想定している
    Vec3 n, t, s;
    n = an;
    t = at;
    s = Vec3::cross(n, t).normalize();
    t = Vec3::cross(s, n).normalize();
    //
    AL_ASSERT_DEBUG(s.isNormalized());
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(n, t)) < 0.01f);
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(n, s)) < 0.01f);
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(t, s)) < 0.01f);
    //
    nts.setRow<0>(s);
    nts.setRow<1>(t);
    nts.setRow<2>(n);
    //
    updateNTS();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
void OrthonormalBasis<METHOD>::set(const Vec3& an, const Vec3& at, const Vec3& as)
{
    AL_ASSERT_DEBUG(an.isNormalized());
    AL_ASSERT_DEBUG(at.isNormalized());
    AL_ASSERT_DEBUG(as.isNormalized());
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(an, at)) < 0.0001f);
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(an, as)) < 0.0001f);
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(at, as)) < 0.0001f);
    Vec3 n, t, s;
    n = an;
    t = at;
    s = as;
    //
    nts.setRow<0>(s);
    nts.setRow<1>(t);
    nts.setRow<2>(n);
    updateNTS();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
void OrthonormalBasis<METHOD>::updateNTS()
{
    // 正規直交基底なので inverse(T) = transpose(T)
    ntsInv = nts.transposed();
}


/*
-------------------------------------------------
world2local()
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
Vec3 OrthonormalBasis<METHOD>::world2local(Vec3 world) const
{
    return nts.transform(world);
}

/*
-------------------------------------------------
local2world()
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
Vec3 OrthonormalBasis<METHOD>::local2world(Vec3 local) const
{
    return ntsInv.transform(local);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
Vec3 OrthonormalBasis<METHOD>::getN() const
{
    return nts.row<2>();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
Vec3 OrthonormalBasis<METHOD>::getT() const
{
    return nts.row<1>();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
Vec3 OrthonormalBasis<METHOD>::getS() const
{
    return nts.row<0>();
}
