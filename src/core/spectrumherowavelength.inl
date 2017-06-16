


//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumHerowavelength::SpectrumHerowavelength()
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumHerowavelength::SpectrumHerowavelength(
    std::array<float, 4> aLambdas,
    std::array<float, 4> aIntensitys)
    :lambdas(aLambdas),
    intensitys(aIntensitys)
{}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumRGB SpectrumHerowavelength::toRGB() const
{
    std::array<float, 3> rgb;
    Vec3 xyz = toXYZ();
    xyz2rgb({ { xyz.x(), xyz.y(), xyz.z() } }, rgb);

    // SpectrumSingleWavelength::toRGB()と同じ定数
    const float sr = 2.33995581f;
    const float sg = 2.95689464f;
    const float sb = 3.10655570f;
    return SpectrumRGB(rgb[0] * sr, rgb[1] * sg, rgb[2] * sb);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE Vec3 SpectrumHerowavelength::toXYZ() const
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    for (int32_t i = 0; i < 4;++i)
    {
        const float lambda = lambdas[i];
        const float intensity = intensitys[i];
        x += CIE_X.sample(lambda) * intensity;
        y += CIE_Y.sample(lambda) * intensity;
        z += CIE_Z.sample(lambda) * intensity;
    }
    const float quat = 0.25f;
    x *= quat;
    y *= quat;
    z *= quat;
    return Vec3(x, y, z);
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumHerowavelength SpectrumHerowavelength::createFromHerowavelength(float w0, bool isImportanceSamplingXYZ)
{
    // 範囲チェック
    AL_ASSERT_DEBUG((SPECTRUM_LAMBDA_START <= w0) && (w0 <= SPECTRUM_LAMBDA_LAST));

    // XYZ等色関数を考慮して波長を選択する
    if (isImportanceSamplingXYZ)
    {
        // TODO: 実装
        AL_ASSERT_DEBUG(false);
        return SpectrumHerowavelength();
    }
    // 単純に等間隔でサンプルする
    else
    {
        const float dif = SPECTRUM_LAMBDA_RANGE * 0.25f;
        const float s = w0 - SPECTRUM_LAMBDA_START;
        const float w1 = SPECTRUM_LAMBDA_START + alFmod(s + dif*1.0f, SPECTRUM_LAMBDA_RANGE);
        const float w2 = SPECTRUM_LAMBDA_START + alFmod(s + dif*2.0f, SPECTRUM_LAMBDA_RANGE);
        const float w3 = SPECTRUM_LAMBDA_START + alFmod(s + dif*3.0f, SPECTRUM_LAMBDA_RANGE);
        return SpectrumHerowavelength({ { w0, w1, w2, w3 } }, { { 1.0f, 1.0f, 1.0f, 1.0f } });
    }
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumHerowavelength SpectrumHerowavelength::createFromXYZImportanceSampling(float w0)
{
    // TODO: 実装
    return SpectrumHerowavelength();
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumHerowavelength operator * (float scale, const SpectrumHerowavelength& spectrum)
{
    SpectrumHerowavelength ret;
    for (int32_t i = 0; i < 4; ++i)
    {
        ret.lambdas[i] = spectrum.lambdas[i];
        ret.intensitys[i] = spectrum.intensitys[i] * scale;
    }
    return ret;
}

//-------------------------------------------------
//
//-------------------------------------------------
INLINE SpectrumHerowavelength operator * (const SpectrumSampled& ss, const SpectrumHerowavelength& sh)
{
    SpectrumHerowavelength ret;
    for (int32_t i = 0; i < 4; ++i)
    {
        const float lambda = sh.lambdas[i];
        ret.lambdas[i] = lambda;
        ret.intensitys[i] = ss.sample(lambda);
    }
    return ret;
}
