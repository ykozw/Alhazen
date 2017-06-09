#include "pch.hpp"
#include "core/aabb.hpp"
#include "core/transform.hpp"
#include "shape/shape.hpp"
#include "accelerator/bvh.hpp"

//-------------------------------------------------
// ObjShape
//-------------------------------------------------
class ObjShape
    :public Shape
{
public:
    ObjShape( const ObjectProp& objectProp );
    bool hasAABB() const override;
    int32_t numVerts() const;
    int32_t numFaces() const;
    AABB aabb() const override;
    int32_t maxDepth() const;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;
    void setBSDFs(AllBSDFList bsdfs) override;
private:
    BVH bvh_;
    MaterialIds materialIds_;
    std::array<BSDFPtr, 256> bsdfs_;
    bool twosided_;
};
REGISTER_OBJECT(Shape,ObjShape);

//-------------------------------------------------
//
//-------------------------------------------------
ObjShape::ObjShape(const ObjectProp& objectProp)
    :Shape(objectProp)
{
    const std::string fileName = objectProp.findChildBy("name","filename").asString("test.obj");
    const Transform transform(objectProp.findChildByTag("transform"));
    twosided_ = objectProp.findChildBy("name","twosided").asBool(false);
    // meshのロード
    Mesh mesh;
    mesh.loadFromoObj(fileName);
    mesh.applyTransform(transform);
    //mesh.recalcNormal();
    //mesh.recalcBound();
    materialIds_ = mesh.materialIds();
    // bvhの構築
    bvh_.construct(mesh.vs, mesh.ns, mesh.ts, mesh.faces);
}

//-------------------------------------------------
//
//-------------------------------------------------
bool ObjShape::hasAABB() const
{
    return true;
}

//-------------------------------------------------
//
//-------------------------------------------------
int32_t ObjShape::numVerts() const
{
    return bvh_.numVerts();
}

//-------------------------------------------------
//
//-------------------------------------------------
int32_t ObjShape::numFaces() const
{
    return bvh_.numFaces();
}

//-------------------------------------------------
//
//-------------------------------------------------
AABB ObjShape::aabb() const
{
    return bvh_.aabb();
}

//-------------------------------------------------
//
//-------------------------------------------------
int32_t ObjShape::maxDepth() const
{
    return bvh_.maxDepth();
}

//-------------------------------------------------
//
//-------------------------------------------------
bool ObjShape::intersect(const Ray& ray, Intersect* isect) const
{
    int8_t materialId = 0;
    if (bvh_.intersect(ray, isect, &materialId))
    {
        isect->bsdf = bsdfs_[materialId];
        return true;
    }
    return false;
}

//-------------------------------------------------
//
//-------------------------------------------------
bool ObjShape::intersectCheck(const Ray& ray) const
{
    return bvh_.intersectCheck(ray);
}

//-------------------------------------------------
// setBsdf()
//-------------------------------------------------
void ObjShape::setBSDFs(AllBSDFList bsdfs)
{
    for (const auto& materialId : materialIds_)
    {
        const std::string& name = materialId.first;
        const int32_t id = materialId.second;
        const BSDFPtr bsdf = bsdfs.find(name);
        bsdfs_[id] = bsdf;
    }
}
