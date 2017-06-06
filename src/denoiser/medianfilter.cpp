#include "pch.hpp"
#include "denoiser/denoiser.hpp"

//-------------------------------------------------
// MedianFilter
//-------------------------------------------------
class MedianFilter
    :public Denoiser
{
public:
    MedianFilter(const ObjectProp& prop);;
    void denoise(const Image& src, Image& dst) override;
private:
};
REGISTER_OBJECT(Denoiser,MedianFilter);

//-------------------------------------------------
// MedianFilter()
//-------------------------------------------------
MedianFilter::MedianFilter(const ObjectProp& objectProp)
{

}

//-------------------------------------------------
// denoise()
//-------------------------------------------------
void MedianFilter::denoise(const Image& src, Image& dst)
{
    // HACK: なぜかここのprintが呼ばれない
    logging("Start denoising(median)");
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
            const int32_t mnx = alMax(x - 1, 0);
            const int32_t mxx = alMin(x + 1, w - 1);
            const int32_t mny = alMax(y - 1, 0);
            const int32_t mxy = alMin(y + 1, h - 1);
            const int32_t i0 = src.index(mnx, mny);
            const int32_t i1 = src.index(x, mny);
            const int32_t i2 = src.index(mxx, mny);
            const int32_t i3 = src.index(mnx, y);
            const int32_t i4 = src.index(x, y);
            const int32_t i5 = src.index(mxx, y);
            const int32_t i6 = src.index(mnx, mxy);
            const int32_t i7 = src.index(x, mxy);
            const int32_t i8 = src.index(mxx, mxy);
            //
            struct SpectrumWeight
            {
                Spectrum spectrum;
            };
            SpectrumWeight ps[9]
            {
                { src.pixel(i0) },
                { src.pixel(i1) },
                { src.pixel(i2) },
                { src.pixel(i3) },
                { src.pixel(i4) },
                { src.pixel(i5) },
                { src.pixel(i6) },
                { src.pixel(i7) },
                { src.pixel(i8) }
            };
            std::nth_element(ps, ps + 4, ps + 9,
                             [](const SpectrumWeight&lhs, const SpectrumWeight&rhs)
            {
                return lhs.spectrum.luminance() < rhs.spectrum.luminance();
            });
            const int32_t index = dst.index(x, y);
            dst.pixel(index) = ps[4].spectrum;
        }
    }
    logging("Finish denoising(median)");
}
