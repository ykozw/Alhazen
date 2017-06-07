#include "pch.hpp"
#include "core/spectrum.hpp"
#include "core/unittest.hpp"
#include "core/image.hpp"
#include "core/object.hpp"
#include "tonemapper/tonemapper.hpp"


//-------------------------------------------------
// 全てのRGB値が同じであれば反射用のSpectrumSampledの係数も常に全て同じであることのチェック
// cf. pbrt-v2 p275
//-------------------------------------------------
AL_TEST(spectrum, SpectrumSampled_SameAllCoeffWhenRGBisEqual)
{
    for (float v = 0.0f; v <= 2.0f; v += 0.1f)
    {
        const SpectrumSampled refColor = SpectrumSampled::createFromRGB({ { v, v, v } }, false);
        const float fv = refColor.samples[0];
        for (const float& s : refColor.samples)
        {
            //
            if (fv == 0.0f)
            {
                if (s != 0.0f)
                {
                    return;
                }
            }
            else
            {
                const float d = alFabsf(1.0f - s / fv);
                if (d > 0.002f)
                {
                    return;
                }
            }
        }
    }
}

//-------------------------------------------------
// RGB ⇔ SpectrumSampledの変換でおおよそ元に戻っていることのチェック
//-------------------------------------------------
AL_TEST(spectrum, SampledSpectrumInterconversion)
{
    const float w = 1.0f;
    for (int32_t i = 0; i < 2000; ++i)
    {
        const int32_t ri = i % 10;
        const int32_t gi = (i/10) % 10;
        const int32_t bi = (i / 100) % 10;
        const bool asIllum = (i >= 1000);
        const float r = float(ri) / 10.0f;
        const float g = float(gi) / 10.0f;
        const float b = float(bi) / 10.0f;
        const SpectrumSampled spectrum = SpectrumSampled::createFromRGB({ { r, g, b } }, asIllum);
        SpectrumRGB rgb;
        spectrum.toRGB(rgb);
        const float dr = fabsf(rgb.r - r);
        const float dg = fabsf(rgb.g - g);
        const float db = fabsf(rgb.b - b);
        const float eps = 0.18f; // HACK: この数値は適当
        if (dr > eps || dr > eps || dr > eps)
        {
            return;
        }
    }
}

//-------------------------------------------------
// SpectrumSampledの白色指定(1,1,1)が、D65(色温度6500度)と同じになっているかのチェック
// HACK: 色温度6500とD65は同じではない。何らかの方法でテストをする。
//-------------------------------------------------
AL_TEST(spectrum, SpectrumSampled_WhiteIlluminationIsD65)
{
#if 0
    const float w = 1.0f;
    const SpectrumSampled whiteColor = SpectrumSampled::createFromRGB({ { w, w, w } }, true);
    SpectrumRGB rgb;
    whiteColor.toRGB(rgb);
    rgb.r;
    rgb.g;
    rgb.b;

    const SpectrumSampled d65Color = SpectrumSampled::createFromColorTemp(6500.0f);
    for (int32_t i = 0; i < SPECTRUM_SAMPLED_NUM_SAMPLES; ++i)
    {

        const float d = alFabsf( 1.0f - d65Color.samples[i] / whiteColor.samples[i] );
        const float d2 = alFabsf(1.0f - whiteIllum.samples[i] / whiteColor.samples[i]);
        if (d > 0.02f)
        {
            return;
        }
    }
#endif
    //
    return;
}

//-------------------------------------------------
// test_SpectrumSampled_SamplingLambda()
// RGB -> Spectrum -> Spectrumの波長サンプル -> RGBでちゃんと元におおよそ戻るかのチェック
//-------------------------------------------------
AL_TEST(spectrum, SpectrumSampled_SamplingLambda)
{
    for (int32_t ci = 0; ci < 1000; ++ci)
    {
        //
        const int32_t ri = ci % 10;
        const int32_t gi = (ci / 10) % 10;
        const int32_t bi = (ci / 100) % 10;
        const float r = (float)ri / 10.0f;
        const float g = (float)gi / 10.0f;
        const float b = (float)bi / 10.0f;
        const bool asIllum = false;
        //
        SpectrumSampled refColor = SpectrumSampled::createFromRGB({ { r, g, b } }, asIllum);
        SpectrumRGB rgb;
        rgb.clear();
        const int32_t NUM_SAMPLE = 100;
        const float invNumSample = 1.0f / (float)(NUM_SAMPLE);
        for (int32_t i = 0; i < NUM_SAMPLE; ++i)
        {
            const float lambda = float(i) / float(NUM_SAMPLE) * SPECTRUM_LAMBDA_RANGE + SPECTRUM_LAMBDA_START;
            SpectrumSingleWavelength spectrum(lambda, 1.0f);
            const SpectrumSingleWavelength sampledColor = refColor * spectrum;
            rgb += sampledColor.toRGB() * invNumSample;
        }
        const float dr = fabsf(rgb.r - r);
        const float dg = fabsf(rgb.g - g);
        const float db = fabsf(rgb.b - b);
        const float eps = 0.04f; // 数字は適当
        if (dr > eps || dg > eps || db > eps)
        {
            return;
        }
    }
    return;
}

//-------------------------------------------------
// test_SpectrumSampled_SamplingLambdaHeroWavelength()
// RGB -> Spectrum -> Spectrumの波長サンプル(HeroWaveLength使用) -> RGBでちゃんと元におおよそ戻るかのチェック
//-------------------------------------------------
AL_TEST(spectrum, SpectrumSampled_SamplingLambdaHeroWavelength)
{
    for (int32_t ci = 0; ci < 1000; ++ci)
    {
        //
        const int32_t ri = ci % 10;
        const int32_t gi = (ci / 10) % 10;
        const int32_t bi = (ci / 100) % 10;
        const float r = (float)ri / 10.0f;
        const float g = (float)gi / 10.0f;
        const float b = (float)bi / 10.0f;
        /*const float r = 1.0f;
        const float g = 1.0f;
        const float b = 1.0f;*/
        const bool asIllum = false;
        //
        SpectrumSampled refColor = SpectrumSampled::createFromRGB({ { r, g, b } }, asIllum);
        SpectrumRGB rgb;
        rgb.clear();
        const int32_t NUM_SAMPLE = 100;
        const float invNumSample = 1.0f / (float)(NUM_SAMPLE);
        AL_ASSERT_DEBUG(NUM_SAMPLE % 4 == 0);
        for (int32_t i = 0; i < NUM_SAMPLE / 4; ++i)
        {
            const float lambda = float(i) / float(NUM_SAMPLE) * SPECTRUM_LAMBDA_RANGE + SPECTRUM_LAMBDA_START;
            SpectrumHerowavelength spectrum = SpectrumHerowavelength::createFromHerowavelength(lambda, false);
            const SpectrumHerowavelength sampledColor = refColor * spectrum;
            rgb += sampledColor.toRGB() * invNumSample;
        }
        rgb.r *= 4.0f;
        rgb.g *= 4.0f;
        rgb.b *= 4.0f;
        //
        const float dr = fabsf(rgb.r - r);
        const float dg = fabsf(rgb.g - g);
        const float db = fabsf(rgb.b - b);
        const float eps = 0.04f; // 数字は適当
        if (dr > eps || dg > eps || db > eps)
        {
            return;
        }
    }
    return;
}

//-------------------------------------------------
// sampleHerowavelength1()
//-------------------------------------------------
AL_TEST(spectrum, sampleHerowavelength1)
{
#if 0
    //
    const auto toneMapper = createObject<Tonemapper>("degamma");
    const int32_t NUM_SAMPLE = 64;
    const float invNumSample = 1.0f / (float)NUM_SAMPLE;
    //
    Image refImage;
    refImage.readBmp("dash.bmp", 1.0f / 2.2f);
    const int32_t width = refImage.width();
    const int32_t height = refImage.height();
    const int32_t totalPixel = width * height;

#if 1
    // 単純にRGB ⇔ Spectrum の相互変換
    {
        Image image(width, height);
        for (int32_t pi = 0; pi < totalPixel; ++pi)
        {
            SpectrumRGB s = refImage.pixel(pi);
            const SpectrumSampled refColor = SpectrumSampled::createFromRGB({ { s.r, s.g, s.b } }, true);
            refColor.toRGB(SpectrumRGB(image.pixel(pi)));
        }
        // LDR化し書き出し
        ImageLDR ldrImage(image.width(), image.height());
        toneMapper->process(image, ldrImage);
#if defined(WINDOWS)
        ldrImage.writeBmp("ref.bmp");
#else
        AL_ASSERT_ALWAYS(false);
#endif
    }
#endif

#if 1
    // SingleSample
    {
        XorShift128 rng;
        Image image(width, height);
        for (int32_t pi = 0; pi < totalPixel; ++pi)
        {
            Image::SpectrumType& p = image.pixel(pi);
            const Image::SpectrumType& st = refImage.pixel(pi);
            const SpectrumRGB s = SpectrumRGB(st);
            const SpectrumSampled refColor = SpectrumSampled::createFromRGB({ { s.r, s.g, s.b } }, false);
            //
            SpectrumRGB rgb;
            for (int32_t i = 0; i < NUM_SAMPLE; ++i)
            {
                const float lambda = rng.nextFloat() * SPECTRUM_LAMBDA_RANGE + SPECTRUM_LAMBDA_START;
                SpectrumSingleWavelength spectrum(lambda, 1.0f);
                const SpectrumSingleWavelength sampledColor = refColor * spectrum;
                rgb += sampledColor.toRGB() * invNumSample;
            }
            p = rgb;
        }
        // LDR化し書き出し
        ImageLDR ldrImage(image.width(), image.height());
        toneMapper->process(image, ldrImage);
#if defined(WINDOWS)
        ldrImage.writeBmp("sws.bmp");
#else
        AL_ASSERT_ALWAYS(false);
#endif
    }
#endif

#if 1
    // Herowavelength
    {
        XorShift128 rng;
        Image image(width, height);
        for (int32_t pi = 0; pi < totalPixel; ++pi)
        {
            auto& p = image.pixel(pi);
            const Image::SpectrumType& st = refImage.pixel(pi);
            const SpectrumRGB s = SpectrumRGB(st);
            const SpectrumSampled refColor = SpectrumSampled::createFromRGB({ { s.r, s.g, s.b } }, false);
            AL_ASSERT_DEBUG(NUM_SAMPLE % 4 == 0);
            SpectrumRGB rgb;
            for (int32_t i = 0; i < NUM_SAMPLE / 4; ++i)
            {
                const float lambda = rng.nextFloat() * SPECTRUM_LAMBDA_RANGE + SPECTRUM_LAMBDA_START;
                SpectrumHerowavelength spectrum = SpectrumHerowavelength::createFromHerowavelength(lambda, false);
                const SpectrumHerowavelength sampledColor = refColor * spectrum;
                rgb += sampledColor.toRGB() * invNumSample;
            }
            p = rgb * 4.0f;
        }
        // LDR化し書き出し
        ImageLDR ldrImage(image.width(), image.height());
        toneMapper->process(image, ldrImage);
#if defined(WINDOWS)
        ldrImage.writeBmp("hws.bmp");
#else
        AL_ASSERT_ALWAYS(false);
#endif
    }
#endif

#endif
}
