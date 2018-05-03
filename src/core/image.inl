#pragma once

#include "image.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Image::Image()
{
    resize(1, 1);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Image::Image(int32_t width, int32_t height)
{
    resize(width, height);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Image::~Image()
{

}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Image::resize(int32_t width, int32_t height)
{
    width_ = width;
    height_ = height;
    spectrums_.resize(width_ * height_, Spectrum(0.0f));
    clear(Spectrum(0.0f));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Image::clear(const Spectrum& color)
{
    for (auto& p : spectrums_)
    {
        p = color;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t Image::index(int32_t x, int32_t y) const
{
    return x + width_ * y;
}

#if 0
/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Image::addTo(Image& dstImage, int32_t sx, int32_t sy) const
{
    // サイズチェック
    AL_ASSERT_DEBUG(sx + width_ <= dstImage.width_);
    AL_ASSERT_DEBUG(sy + height_ <= dstImage.height_);
    // 加算
    for (int32_t y = 0; y < height_; ++y)
    {
        for (int32_t x = 0; x < width_; ++x)
        {
            const Spectrum& src = pixel(x, y);
            const int32_t dstIndex = dstImage.index(sx + x, sy + y);
            Spectrum& dst = dstImage.pixel(dstIndex);
            dst += src;
            //
            dstImage.totalWeight_[dstIndex] += 1.0f;
        }
    }
}
#endif

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE const Image::SpectrumType& Image::pixel(int32_t x, int32_t y) const
{
    return spectrums_[x + y*width_];
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Image::SpectrumType& Image::pixel(int32_t x, int32_t y)
{
    return spectrums_[x + y*width_];
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE const Image::SpectrumType& Image::pixel(int32_t index) const
{
    return spectrums_[index];
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Image::SpectrumType& Image::pixel(int32_t index)
{
    return spectrums_[index];
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Image::addPixel(int32_t x, int32_t y, const Spectrum& spectrum)
{
    const int32_t idx = x + y * width_;
    spectrums_[idx] += spectrum;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE const std::vector<Image::SpectrumType>& Image::pixels() const
{
    return spectrums_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE std::vector<Image::SpectrumType>& Image::pixels()
{
    return spectrums_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE Image::SpectrumType Image::sample(float u, float v) const
{
    AL_ASSERT_DEBUG(0.0f <= u && u <= 1.0f);
    AL_ASSERT_DEBUG(0.0f <= v && v <= 1.0f);
    //
    const float x = alMax(u * (float)width_ - 0.5f, 0.0f);
    const float y = alMax(v * (float)height_ - 0.5f, 0.0f);
    // 最近傍4点を見つける
    const int32_t sx = (int32_t)(x);
    const int32_t sy = (int32_t)(y);
    const int32_t lx = alMin((sx + 1), width_ - 1);
    const int32_t ly = alMin((sy + 1), height_ - 1);
    // ウェイトを算出
    const float hw = (x - (float)sx);
    const float vw = (y - (float)sy);
    // ブレンド
    const Image::SpectrumType& lu = pixel(sx, sy);
    const Image::SpectrumType& ru = pixel(lx, sy);
    const Image::SpectrumType& ld = pixel(sx, ly);
    const Image::SpectrumType& rd = pixel(lx, ly);
    Image::SpectrumType uu = lerp(lu, ru, hw);
    Image::SpectrumType dd = lerp(ld, rd, hw);
    Image::SpectrumType r = lerp(uu, dd, vw);
    return r;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t Image::width() const
{
    return width_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t Image::height() const
{
    return height_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void Image::scale(float scale)
{
    for (auto& s : spectrums_)
    {
        s *= scale;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ImageLDR::ImageLDR()
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ImageLDR::ImageLDR(int32_t width, int32_t height)
{
    resize(width, height);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE ImageLDR::~ImageLDR()
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void ImageLDR::resize(int32_t width, int32_t height)
{
    width_ = width;
    height_ = height;
    pixels_.resize(width_*height_);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void ImageLDR::clear(const PixelLDR& aPixel)
{
    for (auto& pixel : pixels_)
    {
        pixel = aPixel;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t ImageLDR::width() const
{
    return width_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE int32_t ImageLDR::height() const
{
    return height_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE const PixelLDR& ImageLDR::pixel(uint32_t index) const
{
    return pixels_[index];
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE PixelLDR& ImageLDR::pixel(uint32_t index)
{
    return pixels_[index];
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE std::vector<PixelLDR>& ImageLDR::pixels()
{
    return pixels_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE const std::vector<PixelLDR>& ImageLDR::pixels() const
{
    return pixels_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE bool HDR2LDR(const Image& src, ImageLDR& dst, float scale)
{
#if 0
    loggingErrorIf(
        (src.width() != dst.width()) ||
        (src.height() != dst.height()),
        "Invalid size tonemapping target");
#endif

#if 1
    const float invGamma = 1.0f / 2.2f;
#else
    // HACK: テスト用
    const float invGamma = 1.0f / 1.0f;
#endif
    //
    for (int32_t y = 0, h = src.height(); y < h; ++y)
    {
        for (int32_t x = 0, w = src.width(); x < w; ++x)
        {
            const int32_t index = src.index(x, y);
            const Spectrum& sp = src.pixel(index);
            Spectrum nsp = sp * scale;
            PixelLDR& dsp = dst.pixel(index);
            SpectrumRGB rgbf;
            nsp.toRGB(rgbf);
            //
            const auto f2u = [&invGamma](float v)
            {
                float fv = alClamp(powf(v, invGamma)*255.0f + 0.5f, 0.0f, 255.0f);
                return static_cast<uint8_t>(fv);
            };
            dsp.r = f2u(rgbf.r);
            dsp.g = f2u(rgbf.g);
            dsp.b = f2u(rgbf.b);
        }
    }
    return true;
}
