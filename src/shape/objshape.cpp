//
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
//
#include "core/bounding.hpp"
#include "core/transform.hpp"
#include "shape/shape.hpp"
#include "accelerator/bvh.hpp"
#include "core/util.hpp"

//
/*
-------------------------------------------------
-------------------------------------------------
*/
class ObjShape AL_FINAL : public Shape
{
public:
    ObjShape(const ObjectProp& objectProp);
    AABB aabb() const override;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;

private:
    BVH bvh_;
    std::vector<BSDFPtr> bsdfs_;
    bool twosided_;
};
REGISTER_OBJECT(Shape, ObjShape);

/*
-------------------------------------------------
-------------------------------------------------
*/
ObjShape::ObjShape(const ObjectProp& objectProp) : Shape(objectProp)
{
    // 
    const std::string fileName = objectProp.findChildByTag("filename").asString("");
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
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, fullPath.c_str(), mtlBaseDir, triangulate))
    {
        loggingError(err.c_str());
        return;
    }
    // 
    std::vector<Vec3> vs;
    std::vector<Vec3> ns;
    std::vector<Vec2> ts;
    std::vector<MeshFace> fs;
    // Position情報の作成
    for (size_t i = 0, e= attrib.vertices.size() / 3; i <e ; ++i)
    {
        //
        const tinyobj::real_t vx = attrib.vertices[3 * i + 0];
        const tinyobj::real_t vy = attrib.vertices[3 * i + 1];
        const tinyobj::real_t vz = attrib.vertices[3 * i + 2];
        vs.push_back(Vec3(float(vx), float(vy), float(vz)));
    }
    // Normal情報の作成
    for (size_t i = 0, e = attrib.normals.size() / 3; i <e; ++i)
    {
        const tinyobj::real_t nx = attrib.normals[3 * i + 0];
        const tinyobj::real_t ny = attrib.normals[3 * i + 1];
        const tinyobj::real_t nz = attrib.normals[3 * i + 2];
        ns.push_back(Vec3(float(nx), float(ny), float(nz)));
    }
    // UV情報の作成
    for (size_t i = 0, e = attrib.texcoords.size() / 2; i <e; ++i)
    {
        const tinyobj::real_t tx = attrib.texcoords[2 * i + 0];
        const tinyobj::real_t ty = attrib.texcoords[2 * i + 1];
        ts.push_back(Vec2(float(tx), float(ty)));
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
            mf.vi = std::array<int32_t, 3>{ { idx0.vertex_index, idx1.vertex_index, idx2.vertex_index }};
            mf.ni = std::array<int32_t, 3>{ { idx0.normal_index, idx1.normal_index, idx2.normal_index }};
            mf.ti = std::array<int32_t, 3>{ { idx0.texcoord_index, idx1.texcoord_index, idx2.texcoord_index }};
            mf.mi = shape.mesh.material_ids[f];
            fs.push_back(mf);
            //
            index_offset += fv;
        }
    }
    //
    bvh_.construct(vs, ns, ts, fs);

    // マテリアルのロード
    for (auto& material : materials)
    {
        // HACK: とりあえずDiffuseだけ対応しておく
        const Spectrum spectrum = 
            Spectrum::createFromRGB(
                std::array<float, 3>({material.diffuse[0], material.diffuse[1], material.diffuse[2]}),
                false);
        bsdfs_.push_back(std::make_shared<Lambertian>(spectrum));
    }
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
        static Lambertian dummy(Spectrum::createFromRGB(std::array<float, 3>{0.1f, 0.1f, 0.1f}, false));
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
