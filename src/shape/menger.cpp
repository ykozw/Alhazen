#include "pch.hpp"
#include "core/object.hpp"
#include "core/orthonormalbasis.hpp"
#include "core/transform.hpp"
#include "shape.hpp"
#include "bsdf/bsdf.hpp"


//-------------------------------------------------
// Menger
//-------------------------------------------------
class Menger
    :public Shape
{
public:
    Menger(const ObjectProp& objectProp);
    bool hasAABB() const override;
    AABB aabb() const override;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;
private:
    Transform transform_;
};
REGISTER_OBJECT(Shape,Menger);

//-------------------------------------------------
// PlaneShape
//-------------------------------------------------
Menger::Menger(const ObjectProp& objectProp)
    :Shape(objectProp)
{
    transform_ = Transform(objectProp.findChildByTag("transform"));
}

//-------------------------------------------------
// hasAABB()
//-------------------------------------------------
INLINE bool Menger::hasAABB() const
{
    AL_ASSERT_DEBUG(false);
    return true;
}

//-------------------------------------------------
// aabb()
//-------------------------------------------------
INLINE AABB Menger::aabb() const
{
    AL_ASSERT_DEBUG(false);
    return AABB();
}

//-------------------------------------------------
//
//-------------------------------------------------
float maxcomp(const Vec3& p)
{
    return std::max(p.x, std::max(p.y, p.z));
}

//-------------------------------------------------
//
//-------------------------------------------------
static Vec3 absVec3(const Vec3& v)
{
    return
        Vec3(
            std::abs(v.x),
            std::abs(v.y),
            std::abs(v.z));
}

//-------------------------------------------------
//
//-------------------------------------------------
static Vec3 maxVec3(const Vec3& v0, const Vec3& v1)
{
    return
        Vec3(
            std::max(v0.x, v1.x),
            std::max(v0.y, v1.y),
            std::max(v0.z, v1.z));
}

//-------------------------------------------------
//
//-------------------------------------------------
static Vec3 minVec3(const Vec3& v0, const Vec3& v1)
{
    return
        Vec3(
            std::min(v0.x, v1.x),
            std::min(v0.y, v1.y),
            std::min(v0.z, v1.z));
}

//-------------------------------------------------
//
//-------------------------------------------------
static Vec3 modVec3(const Vec3& v0, const Vec3& v1)
{
    return
        Vec3(
            fmodf(v0.x + v1.x*100.0f, v1.x),
            fmodf(v0.y + v1.y*100.0f, v1.y),
            fmodf(v0.z + v1.z*100.0f, v1.z));
}

//-------------------------------------------------
//
//-------------------------------------------------
float sdBox(const Vec3& p, const Vec3& b)
{
    Vec3  di = absVec3(p) - b;
    float mc = maxcomp(di);
    const float tmp = maxVec3(di, Vec3(0.0f)).length();
    return std::min(mc, tmp);
}

//-------------------------------------------------
//
//-------------------------------------------------
static Vec4 mapFunc(Vec3 p)
{
    /*const float scale = 2.0f;
    p *= scale;*/
    //
    float d = sdBox(p, Vec3(1.0));
    Vec4 res = Vec4(d, 1.0, 0.0, 0.0);

    float ani = 0.0f;
    //float off = 1.5*sinf(0.0f);

    float s = 1.0;
    for (int m = 0; m<4; m++)
    {
        Vec3 a = modVec3(p*s, Vec3(2.0f)) - 1.0;
        s *= 3.0;
        const Vec3 r = absVec3(1.0 - 3.0*absVec3(a));
        float da = std::max(r.x, r.y);
        float db = std::max(r.y, r.z);
        float dc = std::max(r.z, r.x);
        float c = (std::min(da, std::min(db, dc)) - 1.0f) / s;

        if (c>d)
        {
            d = c;
            res = Vec4(d, std::min(res.y, 0.2f*da*db*dc), (1.0f + float(m)) / 4.0f, 0.0f);
        }
    }

    return res;
}

//-------------------------------------------------
//
//-------------------------------------------------
Vec4 intersectMenger(Vec3 ro, Vec3 rd)
{
    float t = 0.0;
    Vec4 res = Vec4(-1.0f);
    Vec4 h = Vec4(1.0);
    for (int i = 0; i<64; i++)
    {
        /*if (h.x<0.002 || t>10.0) break;*/
        h = mapFunc(ro + rd*t);
        res = Vec4(t,h.y, h.z, h.w);
        t += h.x;
    }
    if (t>10.0) res = Vec4(-1.0);
    return res;
}

//-------------------------------------------------
//
//-------------------------------------------------
static Vec3 sel(const Vec3& v, int32_t i0, int32_t i1, int32_t i2)
{
    return Vec3(v[i0], v[i1], v[i2]);
}

//-------------------------------------------------
//
//-------------------------------------------------
Vec3 calcNormal(Vec3 pos)
{
    Vec3 eps = Vec3(0.001f, 0.0f, 0.0f);
    Vec3 nor;
    /*
    nor.x = mapFunc(pos + eps.xyy).x - map(pos - eps.xyy).x;
    nor.y = mapFunc(pos + eps.yxy).x - map(pos - eps.yxy).x;
    nor.z = mapFunc(pos + eps.yyx).x - map(pos - eps.yyx).x;
    */
    nor.x = mapFunc(pos + sel(eps,0,1,1)).x - mapFunc(pos - sel(eps, 0, 1, 1)).x;
    nor.y = mapFunc(pos + sel(eps,1,0,1)).x - mapFunc(pos - sel(eps, 1, 0, 1)).x;
    nor.z = mapFunc(pos + sel(eps,1,1,0)).x - mapFunc(pos - sel(eps, 1, 1, 0)).x;
    return nor.normalized();
}

//-------------------------------------------------
// intersect()
//-------------------------------------------------
INLINE bool Menger::intersect(const Ray& ray, _Inout_ Intersect* isect) const
{
    //
    Vec4 tmat = intersectMenger(ray.o, ray.d);
    if (tmat.x > 0.0)
    {
        const Vec3 pos = ray.o + tmat.x * ray.d;
        const Vec3 nor = calcNormal(pos);
        //
        isect->t = tmat.x;
        isect->position = pos;
        isect->normal = nor;
        return true;
    }
    return false;
}

//-------------------------------------------------
// intersectCheck()
//-------------------------------------------------
INLINE bool Menger::intersectCheck(const Ray& ray) const
{
    // TODO: より軽い方法があるならそれに切り替える
    Intersect isDummy;
    return intersect(ray, &isDummy);
}
