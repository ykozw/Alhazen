#include "pch.hpp"
#include "tonemapper/tonemapper.hpp"
#include "core/logging.hpp"
#include "core/distribution1d.hpp"
#include "core/unittest.hpp"

//-------------------------------------------------
// File
// TODO: できたらどこか別の場所に移動させる
//-------------------------------------------------
class File
{
public:
    File()
        :file_(NULL)
    {
    }
    ~File()
    {
        fclose(file_);
    }
    void openRead(const char* filename)
    {
        file_ = fopen(filename, "rt");
    }
    //
    FILE* file()
    {
        return file_;
    }
private:
    FILE* file_;
};


//-------------------------------------------------
//
//-------------------------------------------------
struct Cube
{
public:
    void resize(int32_t size)
    {
        size_ = size;
        size2_ = size_ * size_;
        vs.resize(size*size*size);
    }
    Vec3& pixel(int32_t x, int32_t y, int32_t z)
    {
        return vs[x + y*size_ + z*size2_];
    }
    const Vec3& pixel(int32_t x, int32_t y, int32_t z) const
    {
        return vs[x + y*size_ + z*size2_];
    }
    //
    Vec3 sample(float r, float g, float b) const
    {
        //
        r = alClamp(r, 0.0f, 1.0f);
        g = alClamp(g, 0.0f, 1.0f);
        b = alClamp(b, 0.0f, 1.0f);
        // トリリニアサンプリング
        float fr, fg, fb;
        float ifr, ifg, ifb;
        const float sizef = float(size_) - 0.000001f; // HACK
        fr = std::modf(r*sizef, &ifr);
        fg = std::modf(g*sizef, &ifg);
        fb = std::modf(b*sizef, &ifb);
        const int32_t ir = int32_t(ifr);
        const int32_t ig = int32_t(ifg);
        const int32_t ib = int32_t(ifb);
        //
        const int32_t ur = std::min(int32_t(ifr + 1.0f), size_ - 1);
        const int32_t ug = std::min(int32_t(ifg + 1.0f), size_ - 1);
        const int32_t ub = std::min(int32_t(ifb + 1.0f), size_ - 1);
        // X方向
        const Vec3 v00 = pixel(ir, ig, ib) * (1.0f - fr) + pixel(ur, ig, ib) * (fr);
        const Vec3 v01 = pixel(ir, ug, ib) * (1.0f - fr) + pixel(ur, ug, ib) * (fr);
        const Vec3 v02 = pixel(ir, ig, ub) * (1.0f - fr) + pixel(ur, ig, ub) * (fr);
        const Vec3 v03 = pixel(ir, ug, ub) * (1.0f - fr) + pixel(ur, ug, ub) * (fr);
        // Y方向
        const Vec3 v10 = v00 * (1.0f - fg) + v01 * (fg);
        const Vec3 v11 = v02 * (1.0f - fg) + v03 * (fg);
        // Z方向
        const Vec3 v20 = v10 * (1.0f - fb) + v11 * (fb);
        return v20;
    }
private:
    std::vector<Vec3> vs;
    int32_t size_;
    int32_t size2_;
};

//-------------------------------------------------
// Dotcube
// .Dotcubeファイルからトーンマッピングを行う
//-------------------------------------------------
class Dotcube
    : public Tonemapper
{
public:
    Dotcube(const ObjectProp& objectProp);
    bool process(const Image& src, ImageLDR& dst) const override;
private:
    Cube cube_;
    float invGamma_;
};

REGISTER_OBJECT(Tonemapper,Dotcube);


//-------------------------------------------------
// Dotcube
//-------------------------------------------------
Dotcube::Dotcube(const ObjectProp& objectProp)
    :Tonemapper(objectProp)
{
    //
    const float gamma = objectProp.findChildBy("name","gamma").asFloat( 2.2f );
    invGamma_ = 1.0f / gamma;

    // Dotcubeファイルのロード
    const std::string fileName = objectProp.findChildBy("name","file").asString("none");
    File file;
    file.openRead(fileName.c_str());

    // "LUT_3D_SIZE 32"まで進める
    char buffer[0xff];
    fgets(buffer, 0xff, file.file()); // TODO: まともにする
    fgets(buffer, 0xff, file.file());
    fgets(buffer, 0xff, file.file());
    fgets(buffer, 0xff, file.file());
    fgets(buffer, 0xff, file.file());
    int32_t dimSize = 0;
    while (fscanf(file.file(), "LUT_3D_SIZE %d", &dimSize) == 0){}
    cube_.resize(dimSize);
    //
    for (int32_t z = 0; z < dimSize; ++z)
    {
        for (int32_t y = 0; y < dimSize; ++y)
        {
            for (int32_t x = 0; x < dimSize; ++x)
            {
                Vec3& v = cube_.pixel(x, y, z);
                float fx,fy,fz;
                fscanf(file.file(), "%f %f %f", &fx, &fy, &fz);
                v = Vec3(fx,fy,fz);
            }
        }
    }
}

//-------------------------------------------------
// process
//-------------------------------------------------
bool Dotcube::process(const Image& src, ImageLDR& dst) const
{
    //
    loggingErrorIf(
        (src.width() != dst.width()) ||
        (src.height() != dst.height()),
        "Invalid size tonemapping target");
    //
    for (int32_t y = 0, h = src.height(); y < h; ++y)
    {
        for (int32_t x = 0, w = src.width(); x < w; ++x)
        {
            const int32_t index = src.index(x, y);
            const Spectrum& sp = src.pixel(index);
            Spectrum nsp = sp;
            SpectrumRGB rgb;
            nsp.toRGB(rgb);
            // degamma
            const float tr2 = powf(std::max(rgb.r, 0.0f), invGamma_);
            const float tg2 = powf(std::max(rgb.g, 0.0f), invGamma_);
            const float tb2 = powf(std::max(rgb.b, 0.0f), invGamma_);
            // tonemapping
            const Vec3 newPix = cube_.sample(tr2, tg2, tb2);
            // toInt
            const float ifr = newPix.x() * 255.0f + 0.5f;
            const float ifg = newPix.y() * 255.0f + 0.5f;
            const float ifb = newPix.z() * 255.0f + 0.5f;
            const uint8_t ir = (uint8_t)alClamp((int32_t)(ifr), (int32_t)0, (int32_t)255);
            const uint8_t ib = (uint8_t)alClamp((int32_t)(ifg), (int32_t)0, (int32_t)255);
            const uint8_t ig = (uint8_t)alClamp((int32_t)(ifb), (int32_t)0, (int32_t)255);
            PixelLDR& dp = dst.pixel(index);
            dp.r = ir;
            dp.g = ib;
            dp.b = ig;
        }
    }
    //
    return true;
}

