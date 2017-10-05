#include "pch.hpp"
#include "core/logging.hpp"
#include "core/unittest.hpp"
#include "core/floatstreamstats.hpp"
#include "light/light.hpp"
#include "sampler/sampler.hpp"
#include "shape/shape.hpp"

REGISTER_OBJECT(Light, ConstantLight);
REGISTER_OBJECT(Light, RectangleLight);
REGISTER_OBJECT(Light, SphereLight);
REGISTER_OBJECT(Light, EnviromentLight);

/*
-------------------------------------------------
Light
-------------------------------------------------
*/
Light::Light(const ObjectProp& objectProp)
{
    sampleNum_ = objectProp.findChildBy("name","samplenum").asInt(1);
}

/*
-------------------------------------------------
sampleNum
-------------------------------------------------
*/
int32_t Light::sampleNum() const
{
    return sampleNum_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
ConstantLight::ConstantLight()
    :Light(ObjectProp())
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
ConstantLight::ConstantLight(const ObjectProp& objectProp)
    :Light(objectProp)
{
    const Spectrum spectrum = evalAsSpectrum(objectProp.findChildBy("name", "radiance").asString("0.5"), true);
    init(spectrum);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void ConstantLight::init(const Spectrum& spectrum)
{
    spectrum_ = spectrum;
    // HACK: シーンサイズから見て十分な遠方は決め打ち
    faraway_ = 100000.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool ConstantLight::isDeltaFunc() const
{
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum ConstantLight::emittion(Vec3 pos, Vec3 dir) const
{
    static_cast<void>(pos);
    static_cast<void>(dir);
    // 固定
    return spectrum_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float ConstantLight::pdf(
    Vec3 targetPos,
    Vec3 dir) const
{
    //return 1.0f;
    return 1.0f / (4.0f * PI);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum ConstantLight::sampleLe(
    SamplerPtr sampler,
    Vec3 targetPos,
    _Out_ Vec3* samplePos,
    _Out_ float* pdf) const
{
    // 常に一定
    *pdf = this->pdf(Vec3(), Vec3());
    // 方向をランダムに選択
    const Vec3 sampleDir = sampler->getSphere();
    // sample位置を作成
    *samplePos = targetPos + sampleDir * faraway_;
    // spectrumは一定
    return spectrum_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool ConstantLight::intersect(const Ray& ray, Intersect* isect) const
{
    const float lightFar = std::numeric_limits<float>::infinity();
    if (isect->t < lightFar)
    {
        return false;
    }
    // ConstantLightは常に衝突対象
    isect->t = lightFar;
    isect->normal = -ray.d;
    isect->bsdf = BSDF::vantaBlack; // NOTE: 常に反射率0
    isect->emission = spectrum_;
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(ConstantLight, 0)
{
    /*
    ライトに照らされている完全各三面の平面を考える
    */
    ConstantLight light;
    light.init(Spectrum::createAsWhite());
    // TODO: ランダムにサンプル
    SamplerPtr sampler = std::make_shared<SamplerIndepent>();
    sampler->setHash(0x01);
    FloatStreamStats stats;
    for (int32_t sn = 0; sn<1024 * 16; ++sn)
    {
        sampler->startSample(sn);
        const Vec3 dir = sampler->getHemisphere();
        Intersect isect;
        isect.clear();
        const bool hit = light.intersect(Ray(Vec3(0.0f, 0.0f, 0.0f), dir), &isect);
        if (hit)
        {
            const float reflection = 1.0f / PI;
            stats.add(dir.z() * 2.0f * PI * reflection);
        }
        else
        {
            stats.add(0.0f);
        }
    }
    //
    FloatStreamStats stats2;
    // 明示的にサンプル
    for (int32_t sn = 0; sn<1024 * 128; ++sn)
    {
        sampler->startSample(sn);
        Vec3 samplePos;
        float pdf;
        const Spectrum e = light.sampleLe(sampler, Vec3(0.0f, 0.0f, 0.0f), &samplePos, &pdf);
        if (samplePos.z() < 0.0f)
        {
            stats2.add(0.0f);
        }
        else
        {
            const Vec3 wi = samplePos.normalized();
            const float reflection = 1.0f / PI;
            stats2.add(e.r * wi.z() * reflection / pdf);
        }
    }
    // 真値が同じ範囲に入っているかチェック
    AL_ASSERT_ALWAYS(FloatStreamStats::maybeSameMean(stats, stats2));
}

#if 0
/*
-------------------------------------------------
-------------------------------------------------
*/
static void TestConstant()
{
    XorShift128 rng;
    ObjectProp prop;
    prop.createFromString(
        "<emitter type = \"constant\">"\
        "<spectrum name = \"intensity\" value = \"1\" / >"\
        "</emitter>");
    LightPtr light = createObjectOld<Light>("constant", prop);
    const int32_t NUM_SAMPLE = 1024;
    int32_t sampleNum = 0;
    float total = 0.0f;
    for (int32_t i = 0; i < NUM_SAMPLE; ++i)
    {
        Vec3 dir;
        float pdf;
        Spectrum spectrum;
        light->sampleDir(rng, &spectrum, &dir, &pdf);
        const float v = Vec3::dot(dir, Vec3(0.0f, 0.0f, 1.0f));
        if (v >= 0.0f)
        {
            total += v;
            ++sampleNum;
        }
    }
    // 全半球上の影響の算出
    total /= (float)sampleNum;
    total *= 2.0f * PI;
    //
    AL_ASSERT_DEBUG(fabsf(total / PI - 1.0f) < 0.01f);
}
TEST(TestConstant);
#endif

/*
-------------------------------------------------
-------------------------------------------------
*/
RectangleLight::RectangleLight()
    :Light(ObjectProp())
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
RectangleLight::RectangleLight(const ObjectProp& objectProp)
    :Light(objectProp)
{
    const Spectrum spectrum = evalAsSpectrum(objectProp.findChildBy("name", "radiance").asString("0.5"), true);
    const Vec3 center = objectProp.findChildBy("name", "center").asXYZ(Vec3(0.0f));
    const Vec3 xaxis = objectProp.findChildBy("name", "axisx").asXYZ(Vec3(1.0f, 0.0f, 0.0f));
    const Vec3 yaxis = objectProp.findChildBy("name", "axisy").asXYZ(Vec3(0.0f, 0.0f, 1.0f));
    init(spectrum, center, xaxis, yaxis);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void RectangleLight::init(
    const Spectrum& spectrum,
    Vec3 center,
    Vec3 xaxis,
    Vec3 yaxis)
{
    AL_ASSERT_DEBUG(xaxis != yaxis);
    origin_ = center - (xaxis + yaxis);
    xaxis2_ = xaxis * 2.0f;
    yaxis2_ = yaxis * 2.0f;
    xaxisNorm_ = xaxis.normalized();
    yaxisNorm_ = yaxis.normalized();
    zaxisNorm_ = Vec3::cross(xaxisNorm_, yaxisNorm_).normalized();
    // 各頂点を出す
    verts_[0] = origin_;
    verts_[1] = origin_ + xaxis2_;
    verts_[2] = origin_ + yaxis2_;
    verts_[3] = origin_ + xaxis2_ + yaxis2_;
    // 法線の算出
    n_ = Vec3::cross(xaxis2_, yaxis2_).normalized();
    // 面積の算出
    area_ = Vec3::cross(xaxis2_, yaxis2_).length();
    invArea_ = 1.0f / area_;
    //
    uvs_[0] = Vec2(0.0f, 0.0f);
    uvs_[1] = Vec2(1.0f, 0.0f);
    uvs_[2] = Vec2(0.0f, 1.0f);
    uvs_[3] = Vec2(1.0f, 1.0f);
    //
    spectrum_ = spectrum;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool RectangleLight::isDeltaFunc() const
{
    return false;
}

/*
-------------------------------------------------
Radianceを返す事に注意
-------------------------------------------------
*/
Spectrum RectangleLight::emittion(Vec3 pos, Vec3 dir) const
{
#if 1
    // HACK: 交差した前提になっている
    return spectrum_;
#else
    Intersect isect;
    if (intersect(Ray(pos, dir), &isect))
    {
        // 測度の変換
        const Vec3 toLight = isect.position - pos;
        const float distSqr = Vec3::dot(toLight, toLight);
        const Vec3  dir = toLight.normalized();
        //const float a = std::fabsf(Vec3::dot(dir, zaxisNorm_));
        return spectrum_ /** a*/ / distSqr;
    }
    else
    {
        return Spectrum::createAsBlack();
    }
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float RectangleLight::pdf(
    Vec3 targetPos,
    Vec3 dir ) const
{
    AL_ASSERT_ALWAYS(false);
    return 0.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum RectangleLight::sampleLe(
    SamplerPtr sampler,
    Vec3 targetPos,
    _Out_ Vec3* samplePos,
    _Out_ float* pdf) const
{
    // 位置を一様にサンプルする
    const Vec2 uv = sampler->get2d();
    *samplePos = xaxis2_ * uv.x() + yaxis2_ * uv.y() + origin_;
    // 測度を変換
    const Vec3 toLight = *samplePos - targetPos;
    const float distSqr = Vec3::dot(toLight, toLight);
    const Vec3  dir = toLight.normalized();
    const float a = std::fabsf(Vec3::dot(dir, zaxisNorm_));
    *pdf = distSqr * invArea_ / a;
    return spectrum_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool RectangleLight::intersect(const Ray& ray, _Inout_ Intersect* isect) const
{
    bool isHit = false;
    isHit |= intersectTriangle(ray, verts_[0], verts_[1], verts_[2], n_, n_, n_, uvs_[0], uvs_[1], uvs_[2], isect);
    isHit |= intersectTriangle(ray, verts_[2], verts_[1], verts_[3], n_, n_, n_, uvs_[1], uvs_[2], uvs_[3], isect);
    if (isHit)
    {
        isect->bsdf = BSDF::vantaBlack;
        isect->emission = spectrum_;
    }
    return isHit;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool RectangleLight::intersectCheck(const Ray& ray) const
{
    return
        intersectTriangleCheck(ray, verts_[0], verts_[1], verts_[2]) ||
        intersectTriangleCheck(ray, verts_[2], verts_[1], verts_[3]);
}


/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(RectangleLight,0)
{
    // HACK: 止まらないが非常に遅い
    return;
    /*
    1x1のRectangleLightが地平面に対してある場合
    */
    RectangleLight light;
    light.init(
        Spectrum::createAsWhite(),
        Vec3(0.0f, 0.0f, 1.0f),
        Vec3(1.0f, 0.0f, 0.0f),
        Vec3(0.0f, 1.0f, 0.0f));
    // TODO: ランダムにサンプル
    SamplerPtr sampler = std::make_shared<SamplerIndepent>();
    sampler->setHash(0x01);
    FloatStreamStats stats;
    for (int32_t sn=0;sn<1024*1024;++sn)
    {
        sampler->startSample(sn);
        const Vec3 dir = sampler->getHemisphere();
        Intersect isect;
        isect.clear();
        const bool hit = light.intersect(Ray(Vec3(0.0f, 0.0f, 0.0f), dir), &isect);
        if (hit)
        {
            //stats.add(dir.z);
            stats.add(1.0f * 2.0f * PI);
        }
        else
        {
            stats.add(0.0f);
        }
    }
    //
    FloatStreamStats stats2;
    // 明示的にサンプル
    for (int32_t sn = 0; sn<1024; ++sn)
    {
        sampler->startSample(sn);
        Vec3 samplePos;
        float pdf;
        const Spectrum e = light.sampleLe(sampler, Vec3(0.0f, 0.0f, 0.0f), &samplePos, &pdf);
        stats2.add(e.r / pdf);
    }
    // 真値が同じ範囲に入っているかチェック
    AL_ASSERT_ALWAYS(FloatStreamStats::maybeSameMean(stats, stats2));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(RectangleLight, 1)
{
    return;
    /*
    1x1のRectangleLightが地平面に対してある場合
    */
    RectangleLight light;
    light.init(
        Spectrum::createAsWhite(),
        Vec3(0.0f, 0.0f, 0.2f),
        Vec3(1000.0f, 0.0f, 0.0f),
        Vec3(0.0f, 1000.0f, 0.0f));
    // TODO: ランダムにサンプル
    SamplerPtr sampler = std::make_shared<SamplerIndepent>();
    sampler->setHash(0x01);
    FloatStreamStats stats;
    for (int32_t sn = 0; sn<1024 * 16; ++sn)
    {
        sampler->startSample(sn);
        const Vec3 dir = sampler->getHemisphere();
        Intersect isect;
        isect.clear();
        const bool hit = light.intersect(Ray(Vec3(0.0f, 0.0f, 0.0f), dir), &isect);
        if (hit)
        {
            const float reflection = 1.0f / PI;
            stats.add(dir.z() * 2.0f * PI * reflection);
        }
        else
        {
            stats.add(0.0f);
        }
    }
    //
    FloatStreamStats stats2;
    // 明示的にサンプル
    for (int32_t sn = 0; sn<1024 * 128; ++sn)
    {
        sampler->startSample(sn);
        Vec3 samplePos;
        float pdf;
        const Spectrum e = light.sampleLe(sampler, Vec3(0.0f, 0.0f, 0.0f), &samplePos, &pdf);
        const Vec3 wi = samplePos.normalized();
        stats2.add(e.r * wi.z() / pdf);
    }
    // 真値が同じ範囲に入っているかチェック
    AL_ASSERT_ALWAYS(FloatStreamStats::maybeSameMean(stats, stats2));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
SphereLight::SphereLight()
    :Light(ObjectProp())
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
SphereLight::SphereLight(const ObjectProp& objectProp)
    :Light(objectProp)
{
    const Spectrum spectrum = evalAsSpectrum(objectProp.findChildBy("name", "radiance").asString("0.5"), true);
    const Vec3 center = objectProp.findChildBy("name", "center").asXYZ(Vec3(0.0f));
    const float radius = objectProp.findChildBy("name", "radius").asFloat(0.0f);
    init(center,radius, spectrum);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SphereLight::init(
    Vec3 center,
    float radius,
    const Spectrum& emission)
{
    //
    AL_ASSERT_ALWAYS(!center.hasNan());
    AL_ASSERT_ALWAYS(0.0f < radius);
    //
    center_ = center;
    radius_ = radius;
    emission_ = emission;
    //
    area_ = 4.0f * PI * radius_ * radius_;
    invArea_ = 1.0f / area_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SphereLight::isDeltaFunc() const
{
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum SphereLight::emittion(
    Vec3 targetPos,
    Vec3 dir) const
{
    // 交差したらEmissionを返す
    Intersect isect;
    const bool hit = intersectSphere(Ray(targetPos, dir), center_, radius_, &isect);
    if (hit)
    {
        return emission_;
    }
    else
    {
        return Spectrum::createAsBlack();
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float SphereLight::pdf(
    Vec3 targetPos,
    Vec3 dir ) const
{
    const float radius2 = radius_ * radius_;
    const Vec3 toLight = center_ - targetPos;
    const float dist2 = Vec3::dot(toLight, toLight);
    // 内側だった場合は、球を一様にサンプル
    if (dist2 < radius2)
    {
        // TODO: これは本当に正しいのか？1/(4Pi)では？
        return invArea_;
    }
    // 外側だった場合はコーンの球冠部分からサンプルする
    else
    {
        const float dist = std::sqrtf(dist2);
        // コーンの内側の角度を求める
        const float sinTheta2 = radius2 / dist2;
        const float cosTheta = std::sqrtf(std::max(0.0f, 1.0f - sinTheta2));
        // 球冠の面積からpdfを出す
        const float sphericalCapArea = 2.0f * PI * (1.0f - cosTheta);
        const float pdf = 1.0f / std::max(sphericalCapArea, std::numeric_limits<float>::min());
        //
        return pdf;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum SphereLight::sampleLe(
    SamplerPtr sampler,
    Vec3 targetPos,
    _Out_ Vec3* samplePos,
    _Out_ float* pdf) const
{
#if 0 // 単純な方法
    // 球の表面を選択
    const Vec3 samplePosNormal = sampler->getSphere();
    *samplePos = center_ + samplePosNormal * radius_;
    *pdf = invArea_;

    // 測度を変換
    const Vec3 toLight = *samplePos - targetPos;
    const float distSqr = Vec3::dot(toLight, toLight);
    const Vec3  dir = toLight.normalized();
    const float dn = Vec3::dot(dir, samplePosNormal);
    const float a = std::fabsf(dn);
    *pdf = distSqr * invArea_ / a;
    return emission_;
#else // コーンを使った方法
    const float radius2 = radius_ * radius_;
    const Vec3 toLight = center_ - targetPos;
    const float dist2 = Vec3::dot(toLight, toLight);
    // 内側だった場合は、球を一様にサンプル
    if (dist2 < radius2)
    {
        const Vec3 samplePosNormal = sampler->getSphere();
        *samplePos = center_ + samplePosNormal * radius_;
        *pdf = invArea_;
        return emission_;
    }
    // 外側だった場合はコーンの球冠部分からサンプルする
    else
    {
        const float dist = std::sqrtf(dist2);
        // コーンの内側の角度を求める
        const float sinTheta2 = radius2 / dist2;
        const float cosTheta = std::sqrtf(std::max(0.0f, 1.0f - sinTheta2));
        // サンプル
        const Vec3 posOnSphericalCapLocal = sampler->getCone(cosTheta) * dist;
        OrthonormalBasis<> localCoord;
        localCoord.set(toLight.normalized());
        const Vec3 posOnSphericalCapWorld = targetPos + localCoord.local2world(posOnSphericalCapLocal);
        // 球の上での位置を交差から算出
        const Vec3 rayDir = (posOnSphericalCapWorld - targetPos).normalized();
        const float lhs = Vec3::dot(rayDir, -toLight);
        const float D = lhs * lhs - dist2 + radius2;
        const float DD = std::max(0.0f,D);
        //
        const float t = std::max((-lhs - sqrtf(DD)),0.0f);
        //
        *samplePos = rayDir * t + targetPos;;
        // 球冠の面積からpdfを出す
        const float sphericalCapArea = 2.0f * PI * (1.0f - cosTheta);
        *pdf = 1.0f / std::max(sphericalCapArea,std::numeric_limits<float>::min());
        //
        return emission_;
    }
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SphereLight::intersect(const Ray& ray, _Inout_ Intersect* isect) const
{
    const bool hit = intersectSphere(ray, center_, radius_, isect);
    if (hit)
    {
        isect->bsdf = BSDF::vantaBlack;
        isect->emission = emission_;
    }
    return hit;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(SphereLight, 1)
{
    /*
    1x1のRectangleLightが地平面に対してある場合
    */
    SphereLight light;
    light.init(Vec3(0.0f, 0.0f, 0.2f), 0.1f, Spectrum::createAsWhite());
    // TODO: ランダムにサンプル
    SamplerPtr sampler = std::make_shared<SamplerIndepent>();
    sampler->setHash(0x01);
    FloatStreamStats stats;
    for (int32_t sn = 0; sn<1024 * 16; ++sn)
    {
        sampler->startSample(sn);
        const Vec3 dir = sampler->getHemisphere();
        Intersect isect;
        isect.clear();
        const bool hit = light.intersect(Ray(Vec3(0.0f, 0.0f, 0.0f), dir), &isect);
        if (hit)
        {
            const float reflection = 1.0f / PI;
            stats.add(dir.z() * 2.0f * PI * reflection);
        }
        else
        {
            stats.add(0.0f);
        }
    }
    //
    FloatStreamStats stats2;
    // 明示的にサンプル
    for (int32_t sn = 0; sn<1024 * 128; ++sn)
    {
        sampler->startSample(sn);
        Vec3 samplePos;
        float pdf;
        const Spectrum e = light.sampleLe(sampler, Vec3(0.0f, 0.0f, 0.0f), &samplePos, &pdf);
        const Vec3 wi = samplePos.normalized();
        stats2.add(e.r * wi.z() / pdf);
    }
    // 真値が同じ範囲に入っているかチェック
    AL_ASSERT_ALWAYS(FloatStreamStats::maybeSameMean(stats, stats2));
}

#if 0
/*
-------------------------------------------------
TestConstant
-------------------------------------------------
*/
static bool TestRectangleLight()
{
    XorShift128 rng;
    ObjectProp prop;
    // 原点の上に配置しておく
    prop.createFromString(
        "<emitter type = \"rectangle\">"\
        "   <vec3 name=\"origin\" x=\"0.0\" y=\"5.0\" z =\"0.0\"/>"\
        "   <vec3 name = \"axisx\" x = \"5.0\" y = \"0.0\" z = \"0.0\" />"\
        "   <vec3 name = \"axisy\" x = \"0.0\" y = \"0.0\" z = \"5.0\" />"\
        "   <rgb name = \"radiance\" value = \"0.5 0.3 0.5\" />"\
        "</emitter>");
    LightPtr light = createObjectOld<Light>("rectangle", prop);

    const int32_t NUM_SAMPLE = 1024;
    // TODO: 原点からランダムにレイを飛ばし、照度を求める
    for (int32_t i = 0; i < NUM_SAMPLE; ++i)
    {
    }

    //
    int32_t sampleNum = 0;
    float total = 0.0f;
    for (int32_t i = 0; i < NUM_SAMPLE; ++i)
    {
        Vec3 dir;
        float pdf;
        Spectrum spectrum;
        light->sampleDir(rng, &spectrum, &dir, &pdf);
        const float v = Vec3::dot(dir, Vec3(0.0f, 0.0f, 1.0f));
        if (v >= 0.0f)
        {
            total += v;
            ++sampleNum;
        }
    }
    // 全半球上の影響の算出
    total /= (float)sampleNum;
    total *= 2.0f * PI;
    //
    AL_ASSERT_DEBUG(fabsf(total / PI - 1.0f) < 0.01f);
    return true;
}
TEST(Emitter, TestRectangleLight, TestRectangleLight);
#endif

/*
-------------------------------------------------
MedianCutBoxInfo
-------------------------------------------------
*/
struct MedianCutBoxInfo
{
    //
    int32_t minx;
    int32_t miny;
    int32_t maxx;
    int32_t maxy;
    //
    Spectrum intensity;
    Vec3  dir;
    //
    Vec2  centroidInLatLongSpace;
    float theta;
    float phi;
};

/*
-------------------------------------------------
dir2tex()
-------------------------------------------------
*/
static void dir2tex(const Vec3& dir, _Out_ Vec2* uv)
{
    const float invPI = 1.0f / PI;
    const float x = (atan2f(dir.x(), -dir.y()) * invPI + 1.0f) * 0.5f;
    const float y = acosf(dir.z()) * invPI;
    *uv = Vec2(x,y);
}

/*
-------------------------------------------------
tex2dir()
-------------------------------------------------
*/
static void tex2dir(const Vec2& uv, _Out_ Vec3* dir)
{
    const float u = 2.0f * uv.x();
    const float v = uv.y();
    const float theta2 = PI * (u - 1.0f);
    const float phi2 = PI * v;
    dir->setX( sinf(phi2) * sinf(theta2) );
    dir->setY( -sinf(phi2) * cosf(theta2) );
    dir->setZ( cosf(phi2) );
}

/*
-------------------------------------------------
EnviromentLight()
-------------------------------------------------
*/
EnviromentLight::EnviromentLight(const ObjectProp& objectProp)
    :Light(objectProp)
{
    logging("Start initialize EnviromentMap.");
    //
    const std::string fileName = objectProp.findChildBy("name", "filename").asString("");
    const std::string ext = strrchr(fileName.c_str(), '.');
    if (ext == ".hdr")
    {
#if defined(WINDOWS)
        image_.readHdr(fileName);
#endif
    }
    else if (ext == ".bmp")
    {
        image_.readBmp(fileName, true);
    }
    else
    {
        AL_ASSERT_DEBUG(false);
    }
    // スケール値の取得
    const float scale = objectProp.findChildBy("name", "scale").asFloat(1.0f);
    image_.scale(scale);
    //
    prepareImportSampling();
    //
    logging("Finish initialize EnviromentMap.");
}

/*
-------------------------------------------------
fill()
-------------------------------------------------
*/
void EnviromentLight::fill(int32_t w, int32_t h, std::function<Spectrum(float, float)> fillFunc)
{
    image_.resize(w, h);
    for (int32_t y = 0; y < h; ++y)
    {
        for (int32_t x = 0; x < w; ++x)
        {
            const float theta = (((float)y + 0.5f) / (float)h) * PI;
            const float phi = (x / (float)w) * 2.0f * PI;
            image_.pixel(x, y) = fillFunc(theta, phi);
        }
    }
    //
    prepareImportSampling();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool EnviromentLight::isDeltaFunc() const
{
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum EnviromentLight::emittion(Vec3 pos, Vec3 dir) const
{
    Vec2 uv;
    dir2tex(dir, &uv);
    return image_.sample(uv.x(), uv.y());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum EnviromentLight::sampleLe(
        SamplerPtr sampler,
        Vec3 targetPos,
        _Out_ Vec3* samplePos,
        _Out_ float* pdf) const
{
    //
    const float u1 = sampler->get1d();
    const float u2 = sampler->get1d();
    Vec2 uv;
    Vec3 worldDir;
    distribution_.sample(u1, u2, pdf, &uv);
    tex2dir(uv, &worldDir);
    // HACK: この係数がどのように算出されたかを説明を入れる
    const float theta = uv.y() * PI;
    *pdf *= 2.0f / (PI*sinf(theta));
    //
    return image_.sample(uv.x(), uv.y());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const Image& EnviromentLight::image()
{
    return image_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void EnviromentLight::prepareImportSampling()
{
    logging("prepare improtance sampling");
    //
    std::vector<std::vector<float>> values;
    const int32_t w = image_.width();
    const int32_t h = image_.height();
    for (int32_t y = 0; y < h; ++y)
    {
        const float theta = (((float)y + 0.5f) / (float)h) * PI;
        const float scale = sinf(theta);
        std::vector<float> rowValue;
        for (int32_t x = 0; x < w; ++x)
        {
            const Spectrum& spectrum = image_.pixel(x, y);
            const float lum = spectrum.luminance();
            rowValue.push_back(lum * scale);
        }
        values.push_back(rowValue);
    }
    distribution_ = Distribution2D(values);
}

#if 0
/*
-------------------------------------------------
-------------------------------------------------
*/
std::vector<MedianCutBoxInfo> EnviromentLight::medianCut(int32_t cutNo) const
{
    // SATの計算
    const auto computeSAT = [](const Image& src, Image& satImage) ->void
    {
        const int32_t w = src.width();
        const int32_t h = src.height();
        satImage.resize(w, h);
        for (int32_t y = 0; y < h; ++y)
        {
            for (int32_t x = 0; x < w; ++x)
            {
                auto p = src.pixel(x, y);
                if (x != 0)
                {
                    p += satImage.pixel(x - 1, y);
                }
                if (y != 0)
                {
                    p += satImage.pixel(x, y - 1);
                }
                if (x != 0 && y != 0)
                {
                    p -= satImage.pixel(x - 1, y - 1);
                }
                auto& s = satImage.pixel(x, y);
                s = p;
            }
        }
    };
    // AreaSumを返す。開始点を含んだ範囲内の和を計算するために-1を入れる場合があるため、
    // 負数もありえるため、その対処もしている。
    const auto areaSum = [](const Image& image, int x0, int y0, int x1, int y1)-> Spectrum
    {
        Spectrum r(0.0f);
        r += image.pixel(x1, y1);
        r -= (x0 < 0) ? Spectrum(0.0f) : image.pixel(x0, y1);
        r -= (y0 < 0) ? Spectrum(0.0f) : image.pixel(x1, y0);
        r += (x0 < 0 || y0 < 0) ? Spectrum(0.0f) : image.pixel(x0, y0);
        return r;
    };

    const int32_t width = image_.width();
    const int32_t height = image_.height();
    const float widthf = (float)width;
    const float heightf = (float)height;

    // TODO: cutNoが2の累乗かをチェック

    // Cos項がかかかった輝度値と輝度値のSAT
    Image monoBuffer;
    Image monoSAT;
    monoBuffer.resize(width, height);
    monoSAT.resize(width, height);
    // Cos項がかからないカラー値のSAT
    Image colorSAT;

    // 輝度値を入れる
    for (int32_t y = 0; y < height; ++y)
    {
        const float phi = ((float)y / (float)image_.height() - 0.5f) * PI;
        // const float cosTerm = fabsf(cosf(phi));
        const float cosTerm = 1.0f;
        for (int32_t x = 0; x < width; ++x)
        {
            const auto& p = image_.pixel(x, y);
            auto& d = monoBuffer.pixel(x, y);
            const float l = p.luminance() * cosTerm;
            d = Spectrum(l);
        }
    }

    // 各SATの計算
    computeSAT(monoBuffer, monoSAT);
    computeSAT(image_, colorSAT);

    //
    std::deque<AABB2D> boxs;
    // 全体を一つのボックスに入れる
    AABB2D rootBox;
    rootBox.addPoint(Vec2(0.0f, 0.0f));
    rootBox.addPoint(Vec2(widthf - 1.0f, heightf - 1.0f));
    boxs.push_back(rootBox);
    //
    while (!boxs.empty() && boxs.size() < cutNo)
    {
        // 先頭のボックスを取り出す
        auto rootBox = boxs.front();
        boxs.pop_front();
        // ボックスの中央
        const Vec2 center = rootBox.center();
        const Vec2 min = rootBox.min();
        const Vec2 max = rootBox.max();
        // ボックスの長軸を出す。Y方向はcosTerm分補正される(極付近であれば短くする)。
        const float phi = (center.y / (float)height - 0.5f) * PI;
        const float cosTerm = fabsf(cosf(phi));
        // ボックスの長軸を判定する。0:x 1:y
        const bool isAxisIsX = cosTerm * rootBox.width() > rootBox.height();
        // 分割点を探す
        int32_t splitIndex = 0;
        float minimuDif = std::numeric_limits<float>::max();
        if (isAxisIsX)
        {
            for (int32_t i = (int32_t)min.x; i < (int32_t)max.x; ++i)
            {
                const int32_t sx = (int32_t)min.x;
                const int32_t x = i;
                const int32_t ex = (int32_t)max.x;
                const int32_t yu = (int32_t)min.y;;
                const int32_t yd = (int32_t)max.y;;
                auto p0 = areaSum(monoSAT, sx - 1, yu - 1, x, yd);
                auto p1 = areaSum(monoSAT, x - 1, yu - 1, ex, yd);
                // エネルギーの差の計算
                const float energyDif = Spectrum::energyDif(p0, p1);
                if (energyDif < minimuDif)
                {
                    minimuDif = energyDif;
                    splitIndex = i;
                }
            }
            // 新しいボックスを作成する
            AABB2D leftBox;
            AABB2D rightBox;
            leftBox.addPoint(min);
            leftBox.addPoint(Vec2((float)(splitIndex - 1), max.y));
            rightBox.addPoint(Vec2((float)splitIndex, min.y));
            rightBox.addPoint(max);
            //
            boxs.push_back(leftBox);
            boxs.push_back(rightBox);
        }
        else
        {
            for (int32_t i = (int32_t)min.y; i < (int32_t)max.y; ++i)
            {
                const int32_t lx = (int32_t)min.x;
                const int32_t rx = (int32_t)max.x;
                const int32_t sy = (int32_t)min.y;
                const int32_t y = i;
                const int32_t ey = (int32_t)max.y;
                auto p0 = areaSum(monoSAT, lx - 1, sy - 1, rx, y);
                auto p1 = areaSum(monoSAT, lx - 1, y - 1, rx, ey);
                // エネルギーの差の計算
                const float energyDif = Spectrum::energyDif(p0, p1);
                if (energyDif < minimuDif)
                {
                    minimuDif = energyDif;
                    splitIndex = i;
                }
            }
            // 新しいボックスを作成する
            AABB2D upBox;
            AABB2D downBox;
            upBox.addPoint(min);
            upBox.addPoint(Vec2(max.x, (float)(splitIndex - 1)));
            downBox.addPoint(Vec2(min.x, (float)splitIndex));
            downBox.addPoint(max);
            //
            boxs.push_back(upBox);
            boxs.push_back(downBox);
        }
    }

    //
    std::vector<MedianCutBoxInfo> result;
    for (const auto& box : boxs)
    {
        // 各ボックスの輝度重心を計算する
        const auto& min = box.min();
        const auto& max = box.max();

        // HACK: ちゃんと対応するが、とりあえず潰れてるboxはスキップする
        if (min.x < 0 || min.y < 0 || min.x == max.x || min.y == max.y)
        {
            //   continue;
        }

        // 小さな値を大量に加算したSATでは無視できないレベルの誤差が発生するため、
        // regionSumはSATからではなくこの場で計算する。
        float regionSum = 0.0f;
        Vec2 centroid(0.0f);
        for (int32_t x = (int32_t)min.x; x <= (int32_t)max.x; ++x)
        {
            for (int32_t y = (int32_t)min.y; y <= (int32_t)max.y; ++y)
            {
                // HACK: 何かがおかしい
                y = alMax(0, y);
                x = alMax(0, x);
                //
                auto p = monoBuffer.pixel(x, y).luminance();
                regionSum += p;
                centroid += p * Vec2((float)x - min.x, (float)y - min.y);
            }
        }
        loggingWarningIf(regionSum <= 0.0f, "regionSum <= 0.0");
        centroid = centroid / regionSum;
        centroid += min;
        AL_ASSERT_DEBUG(min.x <= centroid.x && centroid.x <= max.x);
        AL_ASSERT_DEBUG(min.y <= centroid.y && centroid.y <= max.y);

        // 重心輝度を求める
        const Spectrum intensity = areaSum(colorSAT, (int32_t)min.x - 1, (int32_t)min.y - 1, (int32_t)max.x, (int32_t)max.y) / (widthf*heightf);
        // theta/phiを求める
        const float theta = centroid.x / widthf * 2.0f * PI;
        const float phi = (centroid.y / heightf - 0.5f) * PI;
        const Vec3 dir(cosf(phi)*cosf(theta), cosf(phi)*sinf(theta), sinf(phi));
        // 出力情報としてまとめる
        MedianCutBoxInfo boxInfo;
        boxInfo.intensity = intensity;
        boxInfo.dir = dir;
        boxInfo.centroidInLatLongSpace = centroid;
        boxInfo.theta = theta;
        boxInfo.phi = phi;
        boxInfo.minx = (int32_t)box.min().x;
        boxInfo.miny = (int32_t)box.min().y;
        boxInfo.maxx = (int32_t)box.max().x;
        boxInfo.maxy = (int32_t)box.max().y;
        result.push_back(boxInfo);
    }
    //
    return result;
}
#endif

/*
-------------------------------------------------
-------------------------------------------------
*/
static bool TestEnviroment()
{
    //worldDir = { x = -1.35296619 y = -0.940121114 z = -1.05976379 }
    /*Vec2 uv;
    const Vec3 worldDir(-1.35296619f, -0.940121114f, -1.05976379f);
    dir2tex(worldDir, &uv);
    AL_ASSERT_DEBUG(!uv.hasNan());*/
#if 0
    // ブルートフォースで輝度平均を得る
    {
        XorShift128 rng;
        ObjectProp prop;
        prop.createFromString(
            "<emitter type = \"envmap\">"\
            "<string name = \"filename\" value = \"../asset/scene/ldr.bmp\" />"\
            "<float samplingWeight = \"1.0\" />"\
            "</emitter>");
        LightPtr light2 = createObjectOld<Light>("envmap", prop);
        EnviromentLight* light = dynamic_cast<EnviromentLight*>(light2.get());
        const Spectrum meanRadiance = light->meanRadiance();
        const int32_t NUM_SAMPLE_BF = 1024 * 1024;
        Spectrum bruteTotal;
        bruteTotal.clear();
        float weight = 0.0f;
        for (int32_t i = 0; i < NUM_SAMPLE_BF; ++i)
        {
            Vec3 dir;
            uniformInSphere(rng.nextFloat(), rng.nextFloat(), &dir);
            Ray ray(Vec3(0.0f), dir);
            const float c = Vec3::dot(dir, Vec3(0.0f, 0.0f, 1.0f));
            const Spectrum spectrum = light->emittion(ray);
            bruteTotal += spectrum;
        }
        bruteTotal /= (float)NUM_SAMPLE_BF;
        const float rate = bruteTotal.r / meanRadiance.r;
        AL_ASSERT_DEBUG(fabs(rate - 1.0f) < 0.01f);
    }
#endif

#if 0
    // ブルートフォースで照度(Z-up)を得る
    {
        XorShift128 rng;
        ObjectProp prop;
        prop.createFromString(
            "<emitter type = \"envmap\">"\
            "<string name = \"filename\" value = \"../asset/scene/ldr_white.bmp\" />"\
            "<float samplingWeight = \"1.0\" />"\
            "</emitter>");
        LightPtr light2 = createObjectOld<Light>("envmap", prop);
        EnviromentLight* light = dynamic_cast<EnviromentLight*>(light2.get());
        const Spectrum irradianceZup = light->irradianceZUp();
        const int32_t NUM_SAMPLE_BF = 1024 * 1024;
        Spectrum bruteTotal;
        bruteTotal.clear();
        float weight = 0.0f;
        for (int32_t i = 0; i < NUM_SAMPLE_BF; ++i)
        {
            const Vec3 dir = remapSquareToHemisphere(Vec2(rng.nextFloat(), rng.nextFloat()));
            Ray ray(Vec3(0.0f), dir);
            const float c = Vec3::dot(dir, Vec3(0.0f, 0.0f, 1.0f));
            const Spectrum spectrum = light->emittion(ray) * c;
            bruteTotal += spectrum;
        }
        bruteTotal /= (float)NUM_SAMPLE_BF;
        bruteTotal *= 2.0f * PI;
        const float rate = bruteTotal.r / irradianceZup.r;
        AL_ASSERT_DEBUG(rate < 0.01f);
    }
#endif

#if 0
    // ImportanceSamplingで輝度平均を得る
    {
        XorShift128 rng;
        ObjectProp prop;
        prop.createFromString(
            "<emitter type = \"envmap\">"\
            "<string name = \"filename\" value = \"../asset/scene/ldr.bmp\" />"\
            "<float samplingWeight = \"1.0\" />"\
            "</emitter>");
        LightPtr light2 = createObjectOld<Light>("envmap", prop);
        EnviromentLight* light = dynamic_cast<EnviromentLight*>(light2.get());
        const auto fillFunc = [](float theta, float phi)
        {
            //return Spectrum(1.0f);
            //return Spectrum(alMax(sinf(theta*2.0f), 0.0f));
            //return Spectrum(alMax(cosf(theta*2.0f), 0.0f));
            return Spectrum(alMax(theta*theta, 0.0f));
        };
        light->fill(128, 128, fillFunc);
        const Spectrum meanRadiance = light->meanRadiance();
        float importanceSampleTotal = 0.0f;
        const int32_t NUM_SAMPLE = 1024;
        for (int32_t i = 0; i < NUM_SAMPLE; ++i)
        {
            Vec3 dir;
            Spectrum spectrum;
            float pdf;
            light->sample(Vec3(0.0f), rng, &spectrum, &dir, &pdf);
            importanceSampleTotal += spectrum.r / pdf;
        }
        importanceSampleTotal /= (float)NUM_SAMPLE;
        const float rate = fabsf(1.0f - meanRadiance.r / importanceSampleTotal);
        AL_ASSERT_DEBUG(rate < 0.01f);
    }
#endif

#if 0
    // ImportanceSamplingで照度(Z-up)を得る
    {
        XorShift128 rng;
        ObjectProp prop;
        prop.createFromString(
            "<emitter type = \"envmap\">"\
            "<string name = \"filename\" value = \"../asset/test_envmap/Mans_Outside_2k.hdr\" />"\
            "<float samplingWeight = \"1.0\" />"\
            "</emitter>");
        LightPtr light2 = createObjectOld<Light>("envmap", prop);
        EnviromentLight* light = dynamic_cast<EnviromentLight*>(light2.get());
        const auto fillFunc = [](float theta, float phi)
        {
            // TOOD: ここで Spectrum(1.0f)以外、正しい結果にならない。原因を考える
            //return Spectrum(1.0f);
            return Spectrum(alMax(sinf(theta*2.0f), 0.0f));
            //return Spectrum(alMax(cosf(theta*2.0f), 0.0f));
            //return Spectrum(alMax(theta*theta, 0.0f));
        };
        //light->fill(128, 128, fillFunc);
        const Image& image = light->image();

        float importanceSampleTotal = 0.0f;
        const int32_t NUM_SAMPLE = 1024 * 1024 * 16;
        const float invNumSample = 1.0f / (float)NUM_SAMPLE;
        Spectrum randomAve, importanceAve;
        randomAve.clear();
        importanceAve.clear();
        for (int32_t i = 0; i < NUM_SAMPLE; ++i)
        {
            Vec3 dir;
            Spectrum spectrum;
            float pdf;
            light->sampleDir(Vec3(0.0f), rng, &spectrum, &dir, &pdf);
            importanceAve += spectrum / pdf * invNumSample;
            //
            Vec3 randomDir;
            uniformInSphere(rng.nextFloat(), rng.nextFloat(), &randomDir);
            randomAve += light->emittion(Ray(Vec3(0.0f), randomDir)) * invNumSample;
        }
        const float rateR = randomAve.r / importanceAve.r;
        const float rateG = randomAve.g / importanceAve.g;
        const float rateB = randomAve.b / importanceAve.b;
        AL_ASSERT_DEBUG(rateR < 0.01f);
    }
#endif
    return true;
}
//TEST(Enviroment, Basic, TestEnviroment);
