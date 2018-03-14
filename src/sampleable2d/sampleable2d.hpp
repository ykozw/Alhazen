#pragma once

#include "pch.hpp"
#include "core/object.hpp"
#include "core/spectrum.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class Sampleable2d : public Object {
 public:
  Sampleable2d() {}
  virtual ~Sampleable2d() {}
  virtual Spectrum sample(float u, float v) = 0;
};
typedef std::shared_ptr<Sampleable2d> Sampleable2dPtr;
