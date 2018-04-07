#include "pch.hpp"
#include "tonemapper/tonemapper.hpp"
#include "core/logging.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class Degamma AL_FINAL : public Tonemapper
{
public:
    Degamma(const ObjectProp& objectProp);
    bool process(const Image& src, ImageLDR& dst) const override;

private:
    float invGamma_;
};

REGISTER_OBJECT(Tonemapper, Degamma);

/*
-------------------------------------------------
-------------------------------------------------
*/
Degamma::Degamma(const ObjectProp& objectProp) : Tonemapper(objectProp)
{
    const float gamma = objectProp.findChildByTag("gamma").asFloat(2.2f);
    invGamma_ = 1.0f / gamma;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Degamma::process(const Image& src, ImageLDR& dst) const
{
    loggingErrorIf((src.width() != dst.width()) ||
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

            PixelLDR& dsp = dst.pixel(index);
            SpectrumRGB rgbf;
            nsp.toRGB(rgbf);
            //
            const float& invGamma = invGamma_;
            const auto f2u = [&invGamma](float v) {
                float fv =
                    alClamp(powf(v, invGamma) * 255.0f + 0.5f, 0.0f, 255.0f);
                return static_cast<uint8_t>(fv);
            };
            dsp.r = f2u(rgbf.r);
            dsp.g = f2u(rgbf.g);
            dsp.b = f2u(rgbf.b);
        }
    }
    return true;
}
