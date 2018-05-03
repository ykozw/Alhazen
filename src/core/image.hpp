#pragma once

#include "math.hpp"
#include "core/object.hpp"
#include "core/spectrum.hpp"

/*
-------------------------------------------------
Kahanの補正加算を行う
-------------------------------------------------
*/
template<typename Type>
struct KahanAdd AL_FINAL
{
public:
    Type sum = Type(0.0f);
    Type err = Type(0.0f);

public:
    KahanAdd(const Type& other)
    {
        sum = other;
        err = Type(0.0f);
    }
    INLINE void reset()
    {
        sum = Type(0.0f);
        err = Type(0.0f);
    }
    INLINE void add(const Type& v)
    {
        // 増加分候補
        const Type d = v - err;
        // 実際に加算
        const Type t = sum + d;
        // 実際の増加分と増加候補の差を記録
        err = (t - sum) - d;
        //
        sum = t;
    }
    INLINE KahanAdd<Type>& operator+=(const Type& other)
    {
        add(other);
        return *this;
    }
    //
    INLINE KahanAdd<Type> operator+(const Type& other)
    {
        KahanAdd<Type> nt;
        nt.add(*this);
        nt.add(other);
        return nt;
    }
    //
    INLINE KahanAdd<Type>& operator*=(float scale)
    {
        sum *= scale;
        err *= scale;
        return *this;
    }
    //
    INLINE KahanAdd<Type>& operator=(const Type& other)
    {
        sum = other;
        err = Type(0.0f);
        return *this;
    }
    INLINE operator Type() const { return sum - err; }
};

/*
-------------------------------------------------
Image
-------------------------------------------------
*/
class Image AL_FINAL
{
public:
#if 0
    // 補正加算を使う場合
    typedef KahanAdd<Spectrum> SpectrumType;
#else
    // 補正加算を使わない場合
    typedef Spectrum SpectrumType;
#endif
public:
    Image();
    Image(int32_t width, int32_t height);
    ~Image();
    void resize(int32_t width, int32_t height);
    void clear(const Spectrum& color);
    int32_t index(int32_t x, int32_t y) const;
    void addTo(Image& dstImage, int32_t x, int32_t y) const;
    const SpectrumType& pixel(int32_t x, int32_t y) const;
    SpectrumType& pixel(int32_t x, int32_t y);
    const SpectrumType& pixel(int32_t index) const;
    SpectrumType& pixel(int32_t index);
    void addPixel(int32_t x, int32_t y, const Spectrum& spectrum);
    const std::vector<SpectrumType>& pixels() const;
    std::vector<SpectrumType>& pixels();
    SpectrumType sample(float u, float v) const;
    int32_t width() const;
    int32_t height() const;
    void scale(float scale);
    // read/write
    void readBmp(const std::string& fileName, bool sRGB);
    void readHdr(const std::string& fileName);
    void writeHdr(const std::string& fileName) const;

private:
    int32_t width_ = 0;
    int32_t height_ = 0;
    std::vector<SpectrumType> spectrums_;
};

/*
-------------------------------------------------
PixelLDR
-------------------------------------------------
*/
struct PixelLDR AL_FINAL
{
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

/*
-------------------------------------------------
ImageLDR
-------------------------------------------------
*/
class ImageLDR AL_FINAL
{
public:
    ImageLDR();
    ImageLDR(int32_t width, int32_t height);
    ~ImageLDR();
    void resize(int32_t width, int32_t height);
    void clear(const PixelLDR& pixel);
    int32_t width() const;
    int32_t height() const;
    const PixelLDR& pixel(uint32_t index) const;
    PixelLDR& pixel(uint32_t index);
    const std::vector<PixelLDR>& pixels() const;
    std::vector<PixelLDR>& pixels();
    // read/write
    // void readBmp(const std::string& fileName, bool sRGB);
    void writePNG(const std::string& fileName) const;

private:
    int32_t width_;
    int32_t height_;
    std::vector<PixelLDR> pixels_;
};

// TODO: これをTonemappingのDegammaが直接利用するようにする。
bool
HDR2LDR(const Image& src, ImageLDR& dst, float scale);

#include "image.inl"
