#pragma once

#include "pch.hpp"
#include "core/math.hpp"

class MathmaticaUtil AL_FINAL {
 public:
  static void writePlotPoint2D(const std::string& fileName,
                               const std::vector<Vec2>& points);
  static void writePlotPoint3D(const std::string& fileName,
                               const std::vector<Vec3>& points);
};
