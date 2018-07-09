//
#include <embree3/rtcore.h>
//
#include "intersect/intersectengine.hpp"
#include "core/math.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectSceneEmbree::Impl : public IsectSceneEmbree
{
public:
    Impl() = default;
    //Impl(RTCDevice device);
    ~Impl();
    void init(RTCDevice device);
    void addMesh(
        int32_t numVtx,
        int32_t numFace,
        const std::function<Vec3(int32_t idx)>& getVtx,
        const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace, 
        const InterpolateFun& interpolate);
    void buildScene();
    bool intersect(const Ray& ray, Intersect* isect) const;
    bool intersectCheck(const Ray& ray) const;
private:
    RTCDevice device_ = nullptr;
    RTCScene scene_ = nullptr;
    // geomIdに対応する補間関数
    std::vector<InterpolateFun> interpolateFuns_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
IsectSceneEmbree::IsectSceneEmbree()
    :impl_(std::make_unique<Impl>())
{
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void IsectSceneEmbree::Impl::init(RTCDevice device)
{
    device_ = device;
    scene_ = rtcNewScene(device_);
    // TODO: rtcSetSceneProgressMonitorFunction()を設定してcommit()?の時間を計る
    rtcSetSceneFlags(scene_, RTC_SCENE_FLAG_COMPACT | RTC_SCENE_FLAG_ROBUST);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
IsectSceneEmbree::~IsectSceneEmbree()
{}
IsectSceneEmbree::Impl::~Impl()
{
    if (scene_ != nullptr)
    {
        rtcReleaseScene(scene_);
        scene_ = nullptr;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void IsectSceneEmbree::addMesh(
    int32_t numVtx,
    int32_t numFace,
    const std::function<Vec3(int32_t idx)>& getVtx,
    const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace,
    const InterpolateFun& interpolate)
{
    return impl_->addMesh(numVtx, numFace, getVtx, getFace, interpolate);
}
void IsectSceneEmbree::Impl::addMesh(
    int32_t numVtx,
    int32_t numFace,
    const std::function<Vec3(int32_t idx)>& getVtx,
    const std::function<std::array<int32_t, 3>(int32_t faceNo)>& getFace, 
    const InterpolateFun& interpolate)
{
    RTCGeometry mesh = rtcNewGeometry(device_, RTC_GEOMETRY_TYPE_TRIANGLE);

    struct Vertex
    {
    public:
        float x, y, z;
    };
    // 頂点のマップ
    Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), numVtx);
    for (int32_t vi = 0; vi < numVtx; ++vi)
    {
        const Vec3 v = getVtx(vi);
        vertices[vi].x = v.x();
        vertices[vi].y = v.y();
        vertices[vi].z = v.z();
    }
    //
    struct Face
    {
        int32_t v0;
        int32_t v1;
        int32_t v2;
    };
    // 面の設定
    Face* faces = (Face*)rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Face), 2);
    for (int32_t fi = 0; fi < numFace; ++fi)
    {
        auto& face = faces[fi];
        const auto idxs = getFace(fi);
        face.v0 = idxs[0];
        face.v1 = idxs[1];
        face.v2 = idxs[2];
    }
    //
    rtcCommitGeometry(mesh);
    const uint32_t geomID = rtcAttachGeometry(scene_, mesh);
    rtcReleaseGeometry(mesh);
    // 交差関数を登録
    if (interpolateFuns_.size() <= geomID)
    {
        interpolateFuns_.resize(geomID + 1);
    }
    interpolateFuns_[geomID] = interpolate;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void IsectSceneEmbree::buildScene()
{
    impl_->buildScene();
}
void IsectSceneEmbree::Impl::buildScene()
{
    if (scene_ != nullptr)
    {
        // 高クオリティデータとして作成
        rtcSetSceneBuildQuality(scene_, RTC_BUILD_QUALITY_HIGH);
        rtcCommitScene(scene_);
        // TODO: rtcGetSceneBounds()で現在のBoundを表示する
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool IsectSceneEmbree::intersect(const Ray& ray, Intersect* isect) const
{
    return impl_->intersect(ray, isect);
}
bool IsectSceneEmbree::Impl::intersect(const Ray& ray, Intersect* isect) const
{
    //
    RTCRay rayEmbree = {};
    rayEmbree.org_x = ray.o.x();
    rayEmbree.org_y = ray.o.y();
    rayEmbree.org_z = ray.o.z();
    rayEmbree.dir_x = ray.d.x();
    rayEmbree.dir_y = ray.d.y();
    rayEmbree.dir_z = ray.d.z();
    rayEmbree.tnear = ray.mint;
    rayEmbree.tfar = ray.maxt;
    //
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    RTCRayHit rayHit;
    rayHit.ray = rayEmbree;
    rayHit.hit = {};
    rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    rtcIntersect1(scene_, &context, &rayHit);
    //
    const bool isHit = (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID);
    if (isHit)
    {
        const auto& interpolate = interpolateFuns_[rayHit.hit.geomID];
        const Interpolated interpolated = interpolate(rayHit.hit.primID, Vec2(rayHit.hit.u, rayHit.hit.v));
        isect->position = interpolated.position;
        isect->normal = interpolated.ng;
        // TODO: nsも入れられるようにする
        //isect->normal = Interpolated.ns;
    }

    //
    return isHit;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool IsectSceneEmbree::intersectCheck(const Ray& ray) const
{
    return impl_->intersectCheck(ray);
}
bool IsectSceneEmbree::Impl::intersectCheck(const Ray& ray) const
{
    //
    RTCRay rayEmbree = {};
    rayEmbree.org_x = ray.o.x();
    rayEmbree.org_y = ray.o.y();
    rayEmbree.org_z = ray.o.z();
    rayEmbree.dir_x = ray.d.x();
    rayEmbree.dir_y = ray.d.y();
    rayEmbree.dir_z = ray.d.z();
    rayEmbree.tnear = ray.mint;
    rayEmbree.tfar = ray.maxt;
    //
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    rtcOccluded1(scene_, &context, &rayEmbree);
    //
    return (rayEmbree.tfar >= 0.0f);
}

/*
-------------------------------------------------
指定した場所間が可視か
可視の場合はtrueが返る(isect系と意味が逆になっている)事に注意
-------------------------------------------------
*/
bool IsectSceneEmbree::isVisible(const Vec3& p0,
    const Vec3& p1) const
{
    //
    const Vec3 d = p1 - p0;
    Ray ray(p0, d);
    ray.maxt = d.length();
    return !intersectCheck(ray);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AABB IsectSceneEmbree::aabb() const
{
    // TODO: 実装
    AL_ASSERT_ALWAYS(false);
    AABB aabb;
    aabb.clear();
    return aabb;
}


/*
-------------------------------------------------
-------------------------------------------------
*/
class IsectEngineEmbreeV3::Impl
{
public:
    Impl();
    ~Impl();
    std::unique_ptr<IsectScene> createScene();
public:
    RTCDevice device_ = nullptr;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
IsectEngineEmbreeV3::IsectEngineEmbreeV3()
    :impl_(std::make_unique<Impl>())
{}
IsectEngineEmbreeV3::Impl::Impl()
{
    device_ = rtcNewDevice(nullptr);
    //
    // TODO: 各種プロパティを得る
    rtcGetDeviceProperty(device_, RTC_DEVICE_PROPERTY_VERSION);
    // TODO: rtcSetDeviceErrorFunction()でエラーのコールバックを設定する
    // TODO: rtcSetDeviceMemoryMonitorFunction()でメモリ関連のコールバックを設定する
}

/*
-------------------------------------------------
-------------------------------------------------
*/
IsectEngineEmbreeV3::~IsectEngineEmbreeV3() = default;

/*
-------------------------------------------------
-------------------------------------------------
*/
IsectEngineEmbreeV3::Impl::~Impl()
{
    if (device_ != nullptr)
    {
        rtcReleaseDevice(device_);
        device_ = nullptr;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
std::unique_ptr<IsectScene> IsectEngineEmbreeV3::createScene()
{
    return impl_->createScene();
}
std::unique_ptr<IsectScene> IsectEngineEmbreeV3::Impl::createScene()
{
    IsectSceneEmbree::Impl* impl = new IsectSceneEmbree::Impl;
    impl->init(device_);
    return std::unique_ptr<IsectSceneEmbree>(impl);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void test()
{
    // https://github.com/embree/embree/blob/master/tutorials/triangle_geometry/triangle_geometry_device.cpp
    IsectEngineEmbreeV3 embree;
    auto scene = embree.createScene();
    scene->addMesh(4, 2,
        [](int32_t vi)
    {
        switch (vi)
        {
        case 0:  return Vec3(-10.0f, -2.0f, -10.0f);
        case 1:  return Vec3(-10.0f, -2.0f, +10.0f);
        case 2:  return Vec3(+10.0f, -2.0f, -10.0f);
        case 3:  return Vec3(+10.0f, -2.0f, +10.0f);
        default: return Vec3(0.0f, 0.0f, 0.0);
        }
    }, [](int32_t fi)->std::array<int32_t, 3>
    {
        switch (fi)
        {
        case 0: return { 0,1,2 };
        case 1: return { 1,3,2 };
        default: return {};
        }
    }, [](int32_t primId, Vec2 biuv)
    {
        IsectScene::Interpolated ret;
        //Interpolated(int32_t primId, Vec2 biuv) > ;
        return ret;
    });
    //
    scene->buildScene();
    //
    {
        // Rayの作成
        Ray ray;
        ray.o = Vec3(0.0f, 0.0f, 0.0f);
        ray.d = Vec3(0.0f, -1.0f, 0.0f);
        ray.mint = 0.0f;
        ray.maxt = std::numeric_limits<float>::infinity();
        Intersect isect;
        const bool hit = scene->intersect(ray, &isect);
        printf("%d\n", hit);
    }
    {
        // Rayの作成
        Ray ray;
        ray.o = Vec3(0.0f, 0.0f, 0.0f);
        ray.d = Vec3(0.0f, -1.0f, 0.0f);
        ray.mint = 0.0f;
        ray.maxt = std::numeric_limits<float>::infinity();
        const bool hitAny = scene->intersectCheck(ray);
        printf("%d\n", hitAny);
    }
}

