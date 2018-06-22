
#include "shape/shape.hpp"
#include "core/transform.hpp"

/*
 -------------------------------------------------
 SubDivのテスト用のShape
 -------------------------------------------------
 */
class SubDShape AL_FINAL : public Shape
{
public:
    SubDShape(const ObjectProp& objectProp);
    AABB aabb() const override;
    bool intersect(const Ray& ray, Intersect* isect) const override;
    bool intersectCheck(const Ray& ray) const override;

private:
#if 0
    // ローカル座標系でのBox。ワールド座標系ではないことに注意。
    AABB aabbLocal_;
    // AABBを変換する
    Transform transform_;
#else
    struct Face
    {
        std::array<Vec3, 4> vs;
        Vec3 n;
    };
    std::array<Face, 6> faces_;
    //
    AABB aabb_;
#endif
};
REGISTER_OBJECT(Shape, SubDShape);

/*
 -------------------------------------------------
 -------------------------------------------------
 */
SubDShape::SubDShape(const ObjectProp& objectProp) : Shape(objectProp)
{
#if 0
    transform_ = Transform(objectProp.findChildByTag("transform"));
    /*
     AABB自体のサイズは指定しない。
     スケールも含めてTransformが吸収する
     */
    const float d = 0.5f;
    aabbLocal_.addPoint(Vec3(-d, -d, -d));
    aabbLocal_.addPoint(Vec3(-d, -d, +d));
    aabbLocal_.addPoint(Vec3(-d, +d, -d));
    aabbLocal_.addPoint(Vec3(-d, +d, +d));
    aabbLocal_.addPoint(Vec3(+d, -d, -d));
    aabbLocal_.addPoint(Vec3(+d, -d, +d));
    aabbLocal_.addPoint(Vec3(+d, +d, -d));
    aabbLocal_.addPoint(Vec3(+d, +d, +d));
#else
    Transform transform(objectProp.findChildByTag("transform"));
    //
    std::array<Vec3, 8> vs;
    vs[0] = transform.toWorld(Vec3(-1.0f, -1.0f, -1.0f));
    vs[1] = transform.toWorld(Vec3(+1.0f, -1.0f, -1.0f));
    vs[2] = transform.toWorld(Vec3(-1.0f, +1.0f, -1.0f));
    vs[3] = transform.toWorld(Vec3(+1.0f, +1.0f, -1.0f));
    vs[4] = transform.toWorld(Vec3(-1.0f, -1.0f, +1.0f));
    vs[5] = transform.toWorld(Vec3(+1.0f, -1.0f, +1.0f));
    vs[6] = transform.toWorld(Vec3(-1.0f, +1.0f, +1.0f));
    vs[7] = transform.toWorld(Vec3(+1.0f, +1.0f, +1.0f));
    //
    const auto createFace = [](Vec3 v0, Vec3 v1, Vec3 v2, Vec3 v3) -> Face {
        Face f;
        f.vs[0] = v0;
        f.vs[1] = v1;
        f.vs[2] = v2;
        f.vs[3] = v3;
        f.n = Vec3::cross(v1 - v0, v2 - v0).normalized();
        return f;
    };
    // -Z
    faces_[0] = createFace(vs[0], vs[1], vs[2], vs[3]);
    // +Z
    faces_[1] = createFace(vs[4], vs[5], vs[6], vs[7]);
    // -Y
    faces_[2] = createFace(vs[0], vs[1], vs[4], vs[5]);
    // +Y
    faces_[3] = createFace(vs[2], vs[3], vs[6], vs[7]);
    // -X
    faces_[4] = createFace(vs[0], vs[2], vs[4], vs[6]);
    // +X
    faces_[5] = createFace(vs[1], vs[3], vs[5], vs[7]);
#endif
    //
    for (auto& v : vs)
    {
        aabb_.addPoint(v);
    }
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE AABB SubDShape::aabb() const { return aabb_; }

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool SubDShape::intersect(const Ray& ray, Intersect* isect) const
{
#if 0
    // レイの位置と方向をLocal座標系に変換
    const Vec3 no = transform_.toLocal(ray.o);
    const Vec3 nd = transform_.toLocalDir(ray.d);
    const float ndl = nd.length();
    const float rd = ray.d.length();
    const float s = rd / ndl;
    // ローカル座標系でAABBと交差判定
    Intersect isectLocal;
    isectLocal.t = isect->t * s;
    if (aabbLocal_.intersect(Ray(no, nd), &isectLocal))
    {
        // ワールド座標系に戻す
        isect->position = transform_.toWorld(isectLocal.position);
        isect->normal = transform_.toWorldDir(isectLocal.normal);
        isect->t = isectLocal.t * s;
        isect->isHit = isectLocal.isHit;
        return true;
    }
#else
    //
    const auto intersectFace =
        [](const Ray& ray, const Face& face, Intersect* isect) {
            const auto& vs = face.vs;
            // HACK: UVは適当
            return intersectTriangle(
                       ray, vs[0], vs[1], vs[2], isect) ||
                   intersectTriangle(
                       ray, vs[2], vs[1], vs[3], isect);
        };
    bool isHit = false;
    isHit |= intersectFace(ray, faces_[0], isect);
    isHit |= intersectFace(ray, faces_[1], isect);
    isHit |= intersectFace(ray, faces_[2], isect);
    isHit |= intersectFace(ray, faces_[3], isect);
    isHit |= intersectFace(ray, faces_[4], isect);
    isHit |= intersectFace(ray, faces_[5], isect);
    return isHit;
#endif
}

/*
 -------------------------------------------------
 -------------------------------------------------
 */
INLINE bool SubDShape::intersectCheck(const Ray& ray) const
{
    // TODO: より軽い方法があるならそれに切り替える
    Intersect isDummy;
    return intersect(ray, &isDummy);
}
