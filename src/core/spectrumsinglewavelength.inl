
//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumSingleWavelength::SpectrumSingleWavelength()
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumSingleWavelength::SpectrumSingleWavelength(float aLambda, float aIntensity)
    :lambda(aLambda), intensity(aIntensity)
{
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumRGB SpectrumSingleWavelength::toRGB() const
{
#if 0
    // sr/sg/sbは次のように出す。
    const float xAve = CIE_X.accumrate() / (float)SPECTRUM_SAMPLED_NUM_SAMPLES;
    const float yAve = CIE_Y.accumrate() / (float)SPECTRUM_SAMPLED_NUM_SAMPLES;
    const float zAve = CIE_Z.accumrate() / (float)SPECTRUM_SAMPLED_NUM_SAMPLES;
    std::array<float, 3> rgbScaleInv;
    xyz2rgb({ { xAve, yAve, zAve } }, rgbScaleInv);
    const float sr = 1.0f / rgbScaleInv[0];
    const float sg = 1.0f / rgbScaleInv[1];
    const float sb = 1.0f / rgbScaleInv[2];
#else
    const float sr = 2.33995581f;
    const float sg = 2.95689464f;
    const float sb = 3.10655570f;
#endif
    std::array<float, 3> rgb;
    Vec3 xyz = toXYZ();
    xyz2rgb({ { xyz.x(), xyz.y(), xyz.z() } }, rgb);
    return SpectrumRGB(rgb[0] * sr, rgb[1] * sg, rgb[2] * sb );
}

//-------------------------------------------------
// 
//-------------------------------------------------
INLINE Vec3 SpectrumSingleWavelength::toXYZ() const
{
    const float x = CIE_X.sample(lambda) * intensity;
    const float y = CIE_Y.sample(lambda) * intensity;
    const float z = CIE_Z.sample(lambda) * intensity;
    return Vec3(x, y, z);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumSingleWavelength operator * (float scale, const SpectrumSingleWavelength& spectrum)
{
    return
        SpectrumSingleWavelength(
        spectrum.lambda,
        spectrum.intensity * scale);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumSingleWavelength operator * (const SpectrumSampled& specrumSampled, const SpectrumSingleWavelength& specrumSingle)
{
    return
        SpectrumSingleWavelength(
        specrumSingle.lambda,
        specrumSampled.sample(specrumSingle.lambda));
}
