#include "core/logging.hpp"
#include "light/light.hpp"
#include "shape/shape.hpp"
#include "core/floatstreamstats.hpp"
#include "sampler/sampler.hpp"

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
    sampleNum_ = objectProp.findChildBy("name", "samplenum").asInt(1);
}

/*
-------------------------------------------------
sampleNum
-------------------------------------------------
*/
int32_t Light::sampleNum() const { return sampleNum_; }

/*
-------------------------------------------------
-------------------------------------------------
*/
ConstantLight::ConstantLight() : Light(ObjectProp()) {}

/*
-------------------------------------------------
-------------------------------------------------
*/
ConstantLight::ConstantLight(const ObjectProp& objectProp) : Light(objectProp)
{
    const Spectrum spectrum = evalAsSpectrum(
        objectProp.findChildBy("name", "radiance").asString("0.5"), true);
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
bool ConstantLight::isDeltaFunc() const { return false; }

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum ConstantLight::emission(Vec3 pos, Vec3 dir) const
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
float ConstantLight::pdf(Vec3 targetPos, Vec3 dir) const
{
    // return 1.0f;
    return 1.0f / (4.0f * PI);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum ConstantLight::sampleLe(Sampler* sampler,
                                 Vec3 targetPos,
                                 Vec3* samplePos,
                                 float* pdf) const
{
    // 常に一定
    *pdf = this->pdf(Vec3(), Vec3());
    // 方向をランダムに選択
    float pdfSphere;
    const Vec3 sampleDir = sampler->getSphere(&pdfSphere);
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
    isect->bsdf = BSDF::vantaBlack.get(); // NOTE: 常に反射率0
    isect->emission = spectrum_;
    return true;
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
RectangleLight::RectangleLight() : Light(ObjectProp()) {}

/*
-------------------------------------------------
-------------------------------------------------
*/
RectangleLight::RectangleLight(const ObjectProp& objectProp) : Light(objectProp)
{
    const Spectrum spectrum = evalAsSpectrum(
        objectProp.findChildBy("name", "radiance").asString("0.5"), true);
    const Vec3 center =
        objectProp.findChildBy("name", "center").asXYZ(Vec3(0.0f));
    const Vec3 xaxis =
        objectProp.findChildBy("name", "axisx").asXYZ(Vec3(1.0f, 0.0f, 0.0f));
    const Vec3 yaxis =
        objectProp.findChildBy("name", "axisy").asXYZ(Vec3(0.0f, 0.0f, 1.0f));
    init(spectrum, center, xaxis, yaxis);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void RectangleLight::init(const Spectrum& spectrum,
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
bool RectangleLight::isDeltaFunc() const { return false; }

/*
-------------------------------------------------
Radianceを返す事に注意
-------------------------------------------------
*/
Spectrum RectangleLight::emission(Vec3 pos, Vec3 dir) const
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
        const Vec3 dir = toLight.normalized();
        // const float a = std::fabsf(Vec3::dot(dir, zaxisNorm_));
        return spectrum_ /** a*/ / distSqr;
    }
    else
    {
        return Spectrum::Black;
    }
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float RectangleLight::pdf(Vec3 targetPos, Vec3 dir) const
{
    AL_ASSERT_ALWAYS(false);
    return 0.0f;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum RectangleLight::sampleLe(Sampler* sampler,
                                  Vec3 targetPos,
                                  Vec3* samplePos,
                                  float* pdf) const
{
    // 位置を一様にサンプルする
    const Vec2 uv = sampler->get2d();
    *samplePos = xaxis2_ * uv.x() + yaxis2_ * uv.y() + origin_;
    // 測度を変換
    const Vec3 toLight = *samplePos - targetPos;
    const float distSqr = Vec3::dot(toLight, toLight);
    const Vec3 dir = toLight.normalized();
    const float a = std::fabsf(Vec3::dot(dir, zaxisNorm_));
    *pdf = distSqr * invArea_ / a;
    return spectrum_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool RectangleLight::intersect(const Ray& ray, Intersect* isect) const
{
    const auto interpolate = [](
        Intersect* isect, 
        Vec3 n, Vec3 v0, Vec3 v1, Vec3 v2,
        Vec2 uv0, Vec2 uv1, Vec2 uv2)
    {
        const Vec2 uv = isect->uvBicentric;
        const float u = uv.x();
        const float v = uv.y();
        isect->normal = n;
        isect->position =
            v0 * (1.0f - u - v) +
            v1 * u +
            v2 * v;
        isect->uv =
            uv0 * (1.0f - u - v) +
            uv1 * u +
            uv2 * v;
    };
    //
    bool isHit = false;
    if (intersectTriangle(ray,
        verts_[0],
        verts_[1],
        verts_[2],
        isect))
    {
        interpolate(
            isect, n_, verts_[0], verts_[1], verts_[2], uvs_[0], uvs_[1], uvs_[2]);
        //
        isHit = true;
    }
    if (intersectTriangle(ray,
        verts_[2],
        verts_[1],
        verts_[3],
        isect))
    {
        interpolate(
            isect, n_, verts_[2], verts_[1], verts_[3], uvs_[2], uvs_[1], uvs_[3]);
    }
    if (isHit)
    {
        isect->bsdf = BSDF::vantaBlack.get();
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
    return intersectTriangleCheck(ray, verts_[0], verts_[1], verts_[2]) ||
           intersectTriangleCheck(ray, verts_[2], verts_[1], verts_[3]);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
SphereLight::SphereLight() : Light(ObjectProp()) {}

/*
-------------------------------------------------
-------------------------------------------------
*/
SphereLight::SphereLight(const ObjectProp& objectProp) : Light(objectProp)
{
    const Spectrum spectrum = evalAsSpectrum(
        objectProp.findChildBy("name", "radiance").asString("0.5"), true);
    const Vec3 center =
        objectProp.findChildBy("name", "center").asXYZ(Vec3(0.0f));
    const float radius = objectProp.findChildBy("name", "radius").asFloat(0.0f);
    init(center, radius, spectrum);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SphereLight::init(Vec3 center, float radius, const Spectrum& emission)
{
    //
    AL_ASSERT_ALWAYS(!center.hasNan());
    AL_ASSERT_ALWAYS(0.0f < radius);
    //
    center_ = center;
    radius_ = ErrFloat(radius);
    radius2_ = radius_ * radius_;
    emission_ = emission;
    //
    area_ = 4.0f * PI * float(radius2_);
    invArea_ = 1.0f / area_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SphereLight::isDeltaFunc() const { return false; }

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum SphereLight::emission(Vec3 targetPos, Vec3 dir) const
{
    // 交差したらEmissionを返す
    Intersect isect;
    const bool hit =
        intersectSphere(Ray(targetPos, dir), center_, radius2_, &isect);
    if (hit)
    {
        return emission_;
    }
    else
    {
        return Spectrum::Black;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float SphereLight::pdf(Vec3 targetPos, Vec3 dir) const
{
    const Vec3 toLight = center_ - targetPos;
    const float dist2 = Vec3::dot(toLight, toLight);
    // 内側だった場合は、球を一様にサンプル
    if (dist2 < float(radius2_))
    {
        // TODO: これは本当に正しいのか？1/(4Pi)では？
        return invArea_;
    }
    // 外側だった場合はコーンの球冠部分からサンプルする
    else
    {
        // const float dist = std::sqrtf(dist2);
        // コーンの内側の角度を求める
        const float sinTheta2 = float(radius2_) / dist2;
        const float cosTheta = std::sqrtf(std::max(0.0f, 1.0f - sinTheta2));
        // 球冠の面積からpdfを出す
        const float sphericalCapArea = 2.0f * PI * (1.0f - cosTheta);
        const float pdf = 1.0f / std::max(sphericalCapArea,
                                          std::numeric_limits<float>::min());
        //
        return pdf;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum SphereLight::sampleLe(Sampler* sampler,
                               Vec3 targetPos,
                               Vec3* samplePos,
                               float* pdf) const
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
    const Vec3 toLight = center_ - targetPos;
    const float dist2 = Vec3::dot(toLight, toLight);
    // 内側だった場合は、球を一様にサンプル
    if (dist2 < float(radius2_))
    {
        float pdfSphere;
        const Vec3 samplePosNormal = sampler->getSphere(&pdfSphere);
        *samplePos = center_ + samplePosNormal * float(radius_);
        *pdf = invArea_;
        return emission_;
    }
    // 外側だった場合はコーンの球冠部分からサンプルする
    else
    {
        const float dist = std::sqrtf(dist2);
        // コーンの内側の角度を求める
        const float sinTheta2 = float(radius2_) / dist2;
        const float cosTheta = std::sqrtf(std::max(0.0f, 1.0f - sinTheta2));
        // サンプル
        const Vec3 posOnSphericalCapLocal = sampler->getCone(cosTheta) * dist;
        OrthonormalBasis<> localCoord;
        localCoord.set(toLight.normalized());
        const Vec3 posOnSphericalCapWorld =
            targetPos + localCoord.local2world(posOnSphericalCapLocal);
        // 球の上での位置を交差から算出
        const Vec3 rayDir = (posOnSphericalCapWorld - targetPos).normalized();
        const float lhs = Vec3::dot(rayDir, -toLight);
        const float D = lhs * lhs - dist2 + float(radius2_);
        const float DD = std::max(0.0f, D);
        //
        const float t = std::max((-lhs - sqrtf(DD)), 0.0f);
        //
        *samplePos = rayDir * t + targetPos;
        ;
        // 球冠の面積からpdfを出す
        const float sphericalCapArea = 2.0f * PI * (1.0f - cosTheta);
        *pdf = 1.0f /
               std::max(sphericalCapArea, std::numeric_limits<float>::min());
        //
        return emission_;
    }
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool SphereLight::intersect(const Ray& ray, Intersect* isect) const
{
    const bool hit = intersectSphere(ray, center_, radius2_, isect);
    if (hit)
    {
        isect->bsdf = BSDF::vantaBlack.get();
        isect->emission = emission_;
    }
    return hit;
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
    Vec3 dir;
    //
    Vec2 centroidInLatLongSpace;
    float theta;
    float phi;
};

/*
-------------------------------------------------
dir2tex()
-------------------------------------------------
*/
static void dir2tex(const Vec3& dir, Vec2* uv)
{
    const float invPI = 1.0f / PI;
    const float x = (atan2f(dir.x(), -dir.y()) * invPI + 1.0f) * 0.5f;
    const float y = acosf(dir.z()) * invPI;
    *uv = Vec2(x, y);
}

/*
-------------------------------------------------
tex2dir()
-------------------------------------------------
*/
static void tex2dir(const Vec2& uv, Vec3* dir)
{
    const float u = 2.0f * uv.x();
    const float v = uv.y();
    const float theta2 = PI * (u - 1.0f);
    const float phi2 = PI * v;
    dir->setX(sinf(phi2) * sinf(theta2));
    dir->setY(-sinf(phi2) * cosf(theta2));
    dir->setZ(cosf(phi2));
}

/*
-------------------------------------------------
EnviromentLight()
-------------------------------------------------
*/
EnviromentLight::EnviromentLight(const ObjectProp& objectProp)
    : Light(objectProp)
{
    logging("Start initialize EnviromentMap.");
    //
    const std::string fileName =
        objectProp.findChildBy("name", "filename").asString("");
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
void EnviromentLight::fill(int32_t w,
                           int32_t h,
                           std::function<Spectrum(float, float)> fillFunc)
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
bool EnviromentLight::isDeltaFunc() const { return false; }

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum EnviromentLight::emission(Vec3 pos, Vec3 dir) const
{
    Vec2 uv;
    dir2tex(dir, &uv);
    return image_.sample(uv.x(), uv.y());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum EnviromentLight::sampleLe(Sampler* sampler,
                                   Vec3 targetPos,
                                   Vec3* samplePos,
                                   float* pdf) const
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
    *pdf *= 2.0f / (PI * sinf(theta));
    //
    return image_.sample(uv.x(), uv.y());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const Image& EnviromentLight::image() { return image_; }

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
