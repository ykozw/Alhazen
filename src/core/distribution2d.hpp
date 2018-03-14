#pragma once

#include "pch.hpp"
#include "core/math.hpp"
#include "core/distribution1d.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class Distribution2D AL_FINAL {
 public:
  Distribution2D();
  Distribution2D(const std::vector<std::vector<float>>& values);
  Distribution2D(
      const std::initializer_list<std::initializer_list<float>>& values);
  Distribution2D(const float* values, int32_t raw, int32_t column);
  Distribution2D(std::function<float(float, float)> valueGen, int32_t raw,
                 int32_t column);
  void construct(const std::vector<std::vector<float>>& valules);
  void sample(float u, float v, _Out_ float* pdf, _Out_ Vec2* samplePoint,
              int32_t* offsetU = NULL, int32_t* offsetV = NULL) const;
  int32_t row() const;
  int32_t column() const;
  float invRow() const;
  float invColumn() const;
  float pdf(int32_t x, int32_t y) const;

 private:
  std::vector<Distribution1D> raws_;
  Distribution1D columns_;
  float row_;
  float col_;
  float invRow_;
  float invCol_;
};
