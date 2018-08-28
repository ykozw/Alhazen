//
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
//
#include "shape/shape.hpp"
#include "accelerator/bvh.hpp"
#include "core/bounding.hpp"
#include "core/transform.hpp"
#include "core/util.hpp"
#include "intersect/intersectengine.hpp"

//
/*
-------------------------------------------------
-------------------------------------------------
*/
class ObjShape AL_FINAL : public Shape
{
public:
    ObjShape(const ObjectProp& objectProp);
    void mapToIntersectEngine(IsectScene* isectScene) override;
    AABB aabb() const override;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;

private:
    //
    BVH bvh_;
    std::vector<BSDFPtr> bsdfs_;
    bool twosided_;

    // TODO: 一時的なデータをメンバとして持つのはとてもよくない。
    std::vector<Vec3> vs_;
    std::vector<Vec3> ns_;
    std::vector<Vec2> ts_;
    std::vector<MeshFace> fs_;
};
REGISTER_OBJECT(Shape, ObjShape);

/*
-------------------------------------------------
-------------------------------------------------
*/
ObjShape::ObjShape(const ObjectProp& objectProp) : Shape(objectProp)
{
    //
    const std::string fileName =
        objectProp.findChildByTag("filename").asString("");
    if (fileName == "")
    {
        loggingError("ObjFile name is empty");
        return;
    }
    const std::string baseDir = g_fileSystem.getSceneFileFolderPath();
    const std::string fullPath = baseDir + fileName;
    std::string objDir, fileNameDummy;
    g_fileSystem.getDirPath(fullPath, objDir, fileNameDummy);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    const char* mtlBaseDir = objDir.c_str();
    const bool triangulate = true;
    if (!tinyobj::LoadObj(&attrib,
                          &shapes,
                          &materials,
                          &err,
                          fullPath.c_str(),
                          mtlBaseDir,
                          triangulate))
    {
        loggingError(err.c_str());
        return;
    }
    
    // Position情報の作成
    for (size_t i = 0, e = attrib.vertices.size() / 3; i < e; ++i)
    {
        //
        const tinyobj::real_t vx = attrib.vertices[3 * i + 0];
        const tinyobj::real_t vy = attrib.vertices[3 * i + 1];
        const tinyobj::real_t vz = attrib.vertices[3 * i + 2];
        vs_.push_back(Vec3(float(vx), float(vy), float(vz)));
    }
    // Normal情報の作成
    for (size_t i = 0, e = attrib.normals.size() / 3; i < e; ++i)
    {
        const tinyobj::real_t nx = attrib.normals[3 * i + 0];
        const tinyobj::real_t ny = attrib.normals[3 * i + 1];
        const tinyobj::real_t nz = attrib.normals[3 * i + 2];
        ns_.push_back(Vec3(float(nx), float(ny), float(nz)));
    }
    // UV情報の作成
    for (size_t i = 0, e = attrib.texcoords.size() / 2; i < e; ++i)
    {
        const tinyobj::real_t tx = attrib.texcoords[2 * i + 0];
        const tinyobj::real_t ty = attrib.texcoords[2 * i + 1];
        ts_.push_back(Vec2(float(tx), float(ty)));
    }
    // face情報の作成
    for (auto& shape : shapes)
    {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            const int fv = shape.mesh.num_face_vertices[f];
            AL_ASSERT_ALWAYS(fv == 3);
            auto& indices = shape.mesh.indices;
            auto idx0 = indices[index_offset + 0];
            auto idx1 = indices[index_offset + 1];
            auto idx2 = indices[index_offset + 2];
            MeshFace mf;
            mf.vi = std::array<int32_t, 3>{
                {idx0.vertex_index, idx1.vertex_index, idx2.vertex_index}};
            mf.ni = std::array<int32_t, 3>{
                {idx0.normal_index, idx1.normal_index, idx2.normal_index}};
            mf.ti = std::array<int32_t, 3>{{idx0.texcoord_index,
                                            idx1.texcoord_index,
                                            idx2.texcoord_index}};
            mf.mi = shape.mesh.material_ids[f];
            fs_.push_back(mf);
            //
            index_offset += fv;
        }
    }
    //
    bvh_.construct(vs_, ns_, ts_, fs_);

    // マテリアルのロード
    for (auto& material : materials)
    {
        const auto f3toSpec = [](float v[3])
        {
            return  Spectrum::createFromRGB(std::array<float, 3>({ v[0], v[1], v[2] }), false);
        };
        bsdfs_.push_back(std::make_shared<MTLBSDF>(
            f3toSpec(material.diffuse),
            f3toSpec(material.specular),
            f3toSpec(material.transmittance),
            f3toSpec(material.emission),
            material.shininess,
            material.ior,
            material.dissolve,
            material.illum));
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void ObjShape::mapToIntersectEngine(IsectScene* isectScene)
{
    //
    isectScene->addMesh(
        int32_t(vs_.size()),
        int32_t(fs_.size()),
        [this](int32_t vi) { return vs_[vi]; },
        [this](int32_t fi) {
            auto& f = fs_[fi];
            return std::array<int32_t, 3>{{f.vi[0], f.vi[1], f.vi[2]}};
        },
        // 補間
        [this](int32_t primId, Vec2 biuv) {
            auto& face = fs_[primId];         
            const auto lerp3 = [](Vec3 p0, Vec3 p1, Vec3 p2, Vec2 biuv)
            {
                const float x = biuv.x();
                const float y = biuv.y();
                return (1.0f - x - y)*p0 + x * p1 + y * p2;
            };
            const auto lerp2 = [](Vec2 p0, Vec2 p1, Vec2 p2, Vec2 biuv)
            {
                const float x = biuv.x();
                const float y = biuv.y();
                return (1.0f - x - y)*p0 + x * p1 + y * p2;
            };
            IsectScene::Interpolated ret;
            ret.position = lerp3(vs_[face.vi[0]], vs_[face.vi[1]], vs_[face.vi[2]], biuv);
            ret.ns = lerp3(ns_[face.ni[0]], ns_[face.ni[1]], ns_[face.ni[2]], biuv);
            ret.ng = ret.ns;
            ret.uv = lerp2(ts_[face.ti[0]], ts_[face.ti[1]], ts_[face.ti[2]], biuv);
            return ret;
        });
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AABB ObjShape::aabb() const { return bvh_.aabb(); }

/*
-------------------------------------------------
-------------------------------------------------
*/
bool ObjShape::intersect(const Ray& ray, Intersect* isect) const
{
    if (!bvh_.intersect(ray, isect))
    {
        return false;
    }
    //
    if (isect->materialId == -1)
    {
        static Lambertian dummy(Spectrum::createFromRGB(
            std::array<float, 3>{0.1f, 0.1f, 0.1f}, false));
        isect->bsdf = &dummy;
    }
    else
    {
        isect->bsdf = bsdfs_[isect->materialId].get();
    }
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool ObjShape::intersectCheck(const Ray& ray) const
{
    return bvh_.intersectCheck(ray);
}
