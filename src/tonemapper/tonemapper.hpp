#pragma once

#include "pch.hpp"
#include "core/object.hpp"
#include "core/image.hpp"
#include "core/math.hpp"
#include "core/rng.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class Tonemapper : public Object {
 public:
  Tonemapper(const ObjectProp& objectProp) { static_cast<void>(objectProp); }
  virtual ~Tonemapper() {}
  virtual bool process(const Image& src, ImageLDR& dst) const = 0;
};
typedef std::shared_ptr<Tonemapper> TonemapperPtr;
