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
        fopen_s(&file_, filename, "rt");
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
// Amp
// .ampファイルからトーンマッピングを行う
//-------------------------------------------------
class Amp
    : public Tonemapper
{
public:
    Amp(const ObjectProp& objectProp);
    bool process(const Image& src, ImageLDR& dst) const override;
private:
    float invGamma_;
    std::array<uint8_t, 256> irgb_;
    std::array<uint8_t, 256> ir_;
    std::array<uint8_t, 256> ig_;
    std::array<uint8_t, 256> ib_;
};

REGISTER_OBJECT(Tonemapper,Amp);


//-------------------------------------------------
// Amp
//-------------------------------------------------
Amp::Amp(const ObjectProp& objectProp)
    :Tonemapper(objectProp)
{
    const float gamma = objectProp.findChildBy("name","gamma").asFloat( 2.2f );
    invGamma_ = 1.0f / gamma;

    // AMPファイルのロード
    const std::string fileName = objectProp.findChildBy("name","file").asString("none");
    File file;
    file.openRead(fileName.c_str());
    FILE* filePtr = file.file();
    fseek(filePtr, 0, SEEK_END );
    const int32_t fileSize = ftell(filePtr);
    fseek(filePtr, 0, SEEK_SET);
    if (fileSize == 256 * 3)
    {
        for (int32_t i = 0; i < 256; ++i)
        {
            irgb_[i] = i;
        }
        fread(&ir_, 256, 1, filePtr);
        fread(&ig_, 256, 1, filePtr);
        fread(&ib_, 256, 1, filePtr);
    }
    else if (
        fileSize == 256 * 4 ||
        fileSize == 256 * 5)
    {
        fread(&irgb_, 256, 1, filePtr);
        fread(&ir_, 256, 1, filePtr);
        fread(&ig_, 256, 1, filePtr);
        fread(&ib_, 256, 1, filePtr);
    }
    else
    {
        AL_ASSERT_DEBUG(false);
    }
}

//-------------------------------------------------
// process
//-------------------------------------------------
bool Amp::process(const Image& src, ImageLDR& dst) const
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
#if 0
            float weight = src.weight(index);
            weight = alMax(weight, 1.0f);
            Spectrum nsp = sp / weight;
#else
            Spectrum nsp = sp;
#endif
            SpectrumRGB rgb;
            nsp.toRGB(rgb);
#if 0
            // tonemapping
            const float ifr = rgb.r * 255.0f + 0.5f;
            const float ifg = rgb.g * 255.0f + 0.5f;
            const float ifb = rgb.b * 255.0f + 0.5f;
            const int32_t idxr0 = clamp((int32_t)(ifr), 0, 255);
            const int32_t idxg0 = clamp((int32_t)(ifg), 0, 255);
            const int32_t idxb0 = clamp((int32_t)(ifb), 0, 255);
            const int32_t idxr1 = clamp(idxr0 + 1, 0, 255);
            const int32_t idxg1 = clamp(idxg0 + 1, 0, 255);
            const int32_t idxb1 = clamp(idxb0 + 1, 0, 255);
            const float fr = fmodf(ifr, 1.0f);
            const float fg = fmodf(ifg, 1.0f);
            const float fb = fmodf(ifb, 1.0f);
            const float tr1 = lerp(r_[idxr0], r_[idxr1], fr);
            const float tg1 = lerp(g_[idxg0], g_[idxg1], fg);
            const float tb1 = lerp(b_[idxb0], b_[idxb1], fb);
            // degamma
            const float tr2 = powf(tr1, invGamma_);
            const float tg2 = powf(tg1, invGamma_);
            const float tb2 = powf(tb1, invGamma_);
            //
            PixelLDR& dp = dst.pixel(index);
            dp.r = clamp((int32_t)(tr2 * 255.0f + 0.5f), 0, 255);
            dp.g = clamp((int32_t)(tg2 * 255.0f + 0.5f), 0, 255);
            dp.b = clamp((int32_t)(tb2 * 255.0f + 0.5f), 0, 255);
#else
            // degamma
            const float tr2 = powf(rgb.r, invGamma_);
            const float tg2 = powf(rgb.g, invGamma_);
            const float tb2 = powf(rgb.b, invGamma_);
            const float ifr = tr2 * 255.0f + 0.5f;
            const float ifg = tg2 * 255.0f + 0.5f;
            const float ifb = tb2 * 255.0f + 0.5f;
            const uint8_t idxr0 = (uint8_t)alClamp((int32_t)(ifr), (int32_t)0, (int32_t)255);
            const uint8_t idxg0 = (uint8_t)alClamp((int32_t)(ifg), (int32_t)0, (int32_t)255);
            const uint8_t idxb0 = (uint8_t)alClamp((int32_t)(ifb), (int32_t)0, (int32_t)255);
            // tonemapping
            PixelLDR& dp = dst.pixel(index);
            dp.r = irgb_[ir_[idxr0]];
            dp.g = irgb_[ig_[idxg0]];
            dp.b = irgb_[ib_[idxb0]];
#endif
        }
    }
    //
    return true;
}

//-------------------------------------------------
// testAI()
//-------------------------------------------------
AL_TEST(AMP, basic)
{
#if 0
    ObjectProp prop;
    prop.createFromString(
        "<tonemapper type = \"Amp\">"\
        "<string name = \"filee\" value = \"C:\\dev\\chv5\\asset\\scene\\tone.amp\" />"\
        "</tonemapper>");
    Amp amp(prop);
#endif
}
