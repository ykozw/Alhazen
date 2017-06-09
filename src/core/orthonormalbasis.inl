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
    n_ = an;
    //
    switch (METHOD)
    {
    case ONB_METHOD::Naive:
        {
            if (fabsf(n_.x) < 0.99f)
            {
                s_ = Vec3::cross(n_, Vec3(1.0f, 0.0f, 0.0));
            }
            else
            {
                s_ = Vec3::cross(n_, Vec3(0.0f, 1.0f, 0.0f));
            }
            s_.normalize();
            t_ = Vec3::cross(s_, n_);
        }
        break;
    case ONB_METHOD::Moller99:
        {
            if (fabs(n_.x) > fabs(n_.z))
            {
                t_ = Vec3(-n_.y, n_.x, 0.0f);
            }
            else
            {
                t_ = Vec3(0.0f, -n_.z, n_.y);
            }
            t_ *= 1.0f / std::sqrtf(Vec3::dot(t_, t_));
            s_ = Vec3::cross(t_, n_);
        }
        break;
    case ONB_METHOD::Frisvad12:
        {
            if (n_.z < -0.9999999f)
            {
                s_ = Vec3(0.0f, -1.0f, 0.0f);
                t_ = Vec3(-1.0f, 0.0f, 0.0f);
            }
            else
            {
                const float a = 1.0f / (1.0f + n_.z);
                const float b = -n_.x * n_.y * a;
                s_ = Vec3(1.0f - n_.x * n_.x * a, b, -n_.x);
                t_ = Vec3(b, 1.0f - n_.y * n_.y * a, -n_.y);
            }
        }
        break;
    case ONB_METHOD::Duff17:
        {
            const float sign = copysignf(1.0f, n_.z);
            const float a = -1.0f / (sign + n_.z);
            const float b = n_.x * n_.y * a;
            t_ = Vec3(1.0f + sign * n_.x * n_.x * a, sign * b, -sign * n_.x);
            s_ = Vec3(b, sign + n_.y * n_.y * a, -n_.y);
        }
        break;
    case ONB_METHOD::Nelson17:
        {
            const double dthreshold = -0.9999999999776;
            float rthreshold = -0.7f;
            if (n_.z >= rthreshold)
            {
                const float a = 1.0f / (1.0f + n_.z);
                const float b = -n_.x * n_.y * a;
                t_ = Vec3(1.0f - n_.x * n_.x * a, b, -n_.x);
                s_ = Vec3(b, 1.0f - n_.y * n_.y * a, -n_.y);
            }
            else
            {
                double x = (double)n_.x;
                double y = (double)n_.y;
                double z = (double)n_.z;
                const double d = 1. / std::sqrt(x*x + y*y + z*z);
                x *= d;
                y *= d;
                z *= d;
                if (z >= dthreshold)
                {
                    const double a = 1.0 / (1.0 + z);
                    const double b = -x*y*a;
                    t_ = Vec3(1.0f - (float)(x*x*a), (float)b, (float)(-x));
                    s_ = Vec3((float)b, 1.0f - (float)(y*y*a), (float)(-y));
                }
                else
                {
                    t_ = Vec3(0.0f, -1.0f, 0.0f);
                    s_ = Vec3(-1.0f, 0.0f, 0.0f);
                }
            }
        }
        break;
    default:
        AL_ASSERT_ALWAYS(false);
        break;
    }
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
    n_ = an;
    t_ = at;
    s_ = Vec3::cross(n_, t_).normalize();
    t_ = Vec3::cross(s_, n_).normalize();
    //
    AL_ASSERT_DEBUG(s_.isNormalized());
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(n_, t_)) < 0.01f);
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(n_, s_)) < 0.01f);
    AL_ASSERT_DEBUG(fabsf(Vec3::dot(t_, s_)) < 0.01f);
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
    n_ = an;
    t_ = at;
    s_ = as;
    //
    updateNTS();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
void OrthonormalBasis<METHOD>::updateNTS()
{
    ntsX_ = Vec3(n_.x, t_.x, s_.x);
    ntsY_ = Vec3(n_.y, t_.y, s_.y);
    ntsZ_ = Vec3(n_.z, t_.z, s_.z);
}


/*
-------------------------------------------------
world2local()
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
Vec3 OrthonormalBasis<METHOD>::world2local(Vec3 world) const
{
    return
        Vec3(
            Vec3::dot(world, s_),
            Vec3::dot(world, t_),
            Vec3::dot(world, n_));
}

/*
-------------------------------------------------
local2world()
-------------------------------------------------
*/
template<ONB_METHOD METHOD>
Vec3 OrthonormalBasis<METHOD>::local2world(Vec3 local) const
{
    return
        Vec3(
            Vec3::dot(local, ntsX_),
            Vec3::dot(local, ntsY_),
            Vec3::dot(local, ntsZ_));
}
