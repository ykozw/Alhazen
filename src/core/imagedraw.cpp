#include "pch.hpp"
#include "imagedraw.hpp"

//-------------------------------------------------
// line()
//-------------------------------------------------
void ImageDraw::line(Image& image, int32_t sx, int32_t sy, int32_t ex, int32_t ey, const Spectrum& acolor)
{
    Spectrum color = acolor;
    // 左から右になるようにする
    int lx, ly, rx, ry;
    if (sx > ex)
    {
        lx = ex;
        ly = ey;
        rx = sx;
        ry = sy;
    }
    else
    {
        lx = sx;
        ly = sy;
        rx = ex;
        ry = ey;
    }
    //
    int deltaX = rx - lx;
    int deltaY = ry - ly;
    // 垂直な線は特別処理
    if (deltaX == 0)
    {
        for (int y = ly; y < ry; ++y)
        {
            image.pixel(lx, y) = color;
        }
    }
    else
    {
        float error = 0.0f;
        float deltaErr = fabsf((float)deltaY / (float)deltaX);
        int y = ly;
        for (int x = lx; x < rx; ++x)
        {
            image.pixel(x, y) = color;
            error += deltaErr;
            while (error >= 0.5f)
            {
                image.pixel(x, y) = color;
                y += (ry > ly) ? 1 : -1;
                error = error - 1.0f;
            }
        }
    }
}

//-------------------------------------------------
// point()
//-------------------------------------------------
void ImageDraw::point(Image& image, int32_t px, int32_t py, int32_t r, const Spectrum& acolor)
{
    Spectrum color = acolor;
    const int32_t w = image.width();
    const int32_t h = image.height();
    const int32_t r2 = r * r;
    for (int32_t x = px - r; x < px + r; ++x)
    {
        for (int32_t y = py - r; y < py + r; ++y)
        {
            if (x < 0 || w - 1 < x)
            {
                continue;
            }
            if (y < 0 || h - 1 < y)
            {
                continue;
            }
            auto& p = image.pixel(x, y);
            const int32_t cr2 = (px - x)*(px - x) + (py - y)*(py - y);
            if (cr2 > r2)
            {
                continue;
            }
            p = color;
        }
    }
}
