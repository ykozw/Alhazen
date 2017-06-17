#pragma once

#include "pch.hpp"
#include "image.hpp"
#include "math.hpp"

class ImageDraw
{
public:
    static void line(Image&image, int32_t sx, int32_t sy, int32_t ex, int32_t ey, const Spectrum& color);
    static void point(Image&image, int32_t x, int32_t y, int32_t r, const Spectrum& color);
};
