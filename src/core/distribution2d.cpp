#include "pch.hpp"
#include "core/distribution2d.hpp"
#include "core/logging.hpp"
#include "core/rng.hpp"
#include "core/unittest.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution2D::Distribution2D()
{
    construct({ { { { 1, 1 } }, { { 1, 1 } } } });
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution2D::Distribution2D(const std::vector<std::vector<float>>& values)
{
    construct(values);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution2D::Distribution2D(const std::initializer_list<std::initializer_list<float>>& values)
{
    std::vector<std::vector<float>> tmp;
    for (const auto& row : values)
    {
        tmp.push_back(std::vector<float>(row.begin(), row.end()));
    }
    construct(tmp);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution2D::Distribution2D(const float* values, int32_t raw, int32_t column)
{
    std::vector<std::vector<float>> tmp;
    for (int32_t i = 0; i < column; ++i)
    {
        const float* begin = values + i * raw;
        const float* end = values + (i + 1) * raw;
        tmp.push_back(std::vector<float>(begin, end));
    }
    construct(tmp);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Distribution2D::Distribution2D(std::function<float(float, float)> valueGen, int32_t raw, int32_t column)
{

}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Distribution2D::construct(const std::vector<std::vector<float>>& values)
{
    // 長さが違う行がないかチェック
    const int32_t rowLength = static_cast<int32_t>(values[0].size());
    for (const auto& row : values)
    {
        AL_ASSERT_DEBUG(row.size() == rowLength);
    }
    //
    row_ = (float)rowLength;
    col_ = (float)values.size();
    invRow_ = 1.0f / row_;
    invCol_ = 1.0f / col_;
    //
    std::vector<float> columnTmp;
    raws_.resize(values.size());
    for (int32_t i = 0; i < raws_.size(); ++i)
    {
        //
        float total = 0.0f;
        for (const float& v : values[i])
        {
            total += v;
        }
        columnTmp.push_back(total);
        //
        raws_[i] = Distribution1D(values[i]);
    }
    columns_ = Distribution1D(columnTmp);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Distribution2D::sample(
    float u,
    float v,
    _Out_ float* pdf,
    _Out_ Vec2* samplePoint,
    int32_t* offsetU,
    int32_t* offsetV) const
{
    //
    int32_t yOffset;
    int32_t xOffset;
    float pdfY;
    float pdfX;
    samplePoint->y = columns_.sample(v, &pdfY, &yOffset);
    samplePoint->x = raws_[yOffset].sample(u, &pdfX, &xOffset);
    // PDFの算出
    *pdf = pdfY * pdfX;
    //
    if (offsetU)
    {
        *offsetU = xOffset;
    }
    if (offsetV)
    {
        *offsetV = yOffset;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t Distribution2D::row() const
{
    return (int32_t)row_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
int32_t Distribution2D::column() const
{
    return (int32_t)col_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution2D::invRow() const
{
    return invRow_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution2D::invColumn() const
{
    return invCol_;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
float Distribution2D::pdf(int32_t x, int32_t y) const
{
    const float pdfX = raws_[y].pdf(x);
    const float pdfY = columns_.pdf(y);
    const float pdf = pdfX * pdfY;
    return pdf;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(Distribution, TwoDimention)
{
    AL_ASSERT_ALWAYS(false);
    // 推定が正しいかチェック
    {
        XorShift128 rng;
        std::vector<std::vector<float>> samples({ { { { 0.1f, 0.6f } }, { { 0.6f, 1.1f } } } });
        Distribution2D d2(samples);
        const int32_t NUM_SAMPLE = 1000;
        float total = 0.0f;
        for (int32_t i = 0; i < NUM_SAMPLE; ++i)
        {
            //
            float pdf;
            Vec2 sp;
            d2.sample(rng.nextFloat(), rng.nextFloat(), &pdf, &sp);
            const float v0 = alLerp(samples[0][0], samples[0][1], sp.x);
            const float v1 = alLerp(samples[1][0], samples[1][1], sp.x);
            const float v = alLerp(v0, v1, sp.y);
            total += v / pdf;
        }
        total /= (float)NUM_SAMPLE;
        const float rate = total / 0.6f;
        if (fabsf(rate - 1.0f) > 0.02f)
        {
            return;
        }
    }
    return;
}

