#pragma once

#include "pch.hpp"
#include "core/math.hpp"
#include "core/bounding.hpp"
#include "core/transform.hpp"
#include "bsdf/bsdf.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
struct MeshFace AL_FINAL {
  // 位置
  std::array<uint32_t, 3> vi;
  // 法線
  std::array<uint32_t, 3> ni;
  // 接線
  std::array<uint32_t, 3> ti;
  // マテリアル
  int8_t mi;

  MeshFace() { mi = -1; }
};

/*
-------------------------------------------------
-------------------------------------------------
*/
typedef std::unordered_map<std::string, int32_t> MaterialIds;

/*
-------------------------------------------------
-------------------------------------------------
*/
class Mesh AL_FINAL {
 public:
  bool loadFromoObj(const std::string& fileName);
  void applyTransform(const Transform& transform);
  void recalcNormal();
  void recalcBound();
  const MaterialIds& materialIds() const;

 public:
  std::vector<Vec3> vs;
  std::vector<Vec3> ns;
  std::vector<Vec2> ts;
  std::vector<MeshFace> faces;
  MaterialIds materialIds_;
  AABB aabb;
};
