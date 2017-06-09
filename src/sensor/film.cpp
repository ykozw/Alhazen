#include "pch.hpp"
#include "sensor/sensor.hpp"

//-------------------------------------------------
// SubFilm()
//-------------------------------------------------
SubFilm::SubFilm()
{}

//-------------------------------------------------
// initialize()
//-------------------------------------------------
void SubFilm::initialize(const Region2D& region, Film* film)
{
    region_ = region;
    image_.resize(region.width(), region.height());
    film_ = film;
    addNum_ = 0;
}

//-------------------------------------------------
// region()
//-------------------------------------------------
const Region2D& SubFilm::region() const
{
    return region_;
}

//-------------------------------------------------
// image()
//-------------------------------------------------
Image& SubFilm::image()
{
    return image_;
}

//-------------------------------------------------
// transferAddFilm()
//-------------------------------------------------
void SubFilm::incAddNum()
{
    ++addNum_;
}

//-------------------------------------------------
// transferAddFilm()
//-------------------------------------------------
int32_t SubFilm::getAddNum() const
{
    return addNum_;
}

//-------------------------------------------------
// transferAddFilm()
//-------------------------------------------------
void SubFilm::transferAddFilm()
{
    const auto& src = image_;
    Image& dst = film_->image_;
    const int32_t w = region_.width();
    const int32_t h = region_.height();
    const int32_t dw = dst.width();
    const float inv = 1.0f / float(addNum_);
    //
    for (int32_t y = 0; y < h; ++y)
    {
        for (int32_t x = 0; x < w; ++x)
        {
            const int32_t px = x + region_.left;
            const int32_t py = y + region_.top;
            //
            const Spectrum& sp = src.pixel(x, y);
            Image::SpectrumType& dp = dst.pixel(px, py);
            dp = sp * inv;
            //
            const int32_t si = x + y * w;
            const int32_t di = px + py * dw;
        }
    }
}

//-------------------------------------------------
// Film()
//-------------------------------------------------
Film::Film(const ObjectProp& objectProp)
{
    const std::string type = objectProp.findChildBy("name","type").asString("hdrfilm");
    const int32_t width = objectProp.findChildBy("name","pixelWidth").asInt(128); // TODO: デフォルト値を知る
    const int32_t height = objectProp.findChildBy("name","pixelHeight").asInt(128); // TODO: デフォルト値を知る

    filmWidthInMM_ = objectProp.findChildBy("name","filmWidthMM").asFloat(36.0f);
    filmHeightInMM_ = objectProp.findChildBy("name","filmHeightMM").asFloat(24.0f);

    // TODO: fileFormat
    // TODO: pixelFormat
    // TODO: componentFormat
    // TODO: cropOffsetX
    // TODO: cropOffsetY
    // TODO: cropWidth
    // TODO: cropHeight
    // TODO: attachLog
    // TODO: banner
    // TODO: highQualityEdges
    // TODO: highQualityEdges
    const std::string rfilter = objectProp.findChildByTag("rfilter").attribute("type").asString("gaussian"); // TODO: デフォルト値を知る
    // タイルサイズで割り切れるかチェック
    const int32_t tileSize = 32;
    AL_ASSERT_DEBUG(width%tileSize == 0);
    AL_ASSERT_DEBUG(height%tileSize == 0);
    //
    const int32_t tileXNum = width / tileSize;
    const int32_t tileYNum = height / tileSize;
    const int32_t numTile = tileXNum * tileYNum;
    image_.resize(width, height);
    subFilms_.resize(numTile);
    for (int32_t tileNo = 0; tileNo < numTile; ++tileNo)
    {
        const int32_t tileX = tileNo % tileXNum;
        const int32_t tileY = (tileNo / tileXNum) % tileYNum;
        const int32_t x = tileX * tileSize;
        const int32_t y = tileY * tileSize;
        const Region2D region(x, y, x + tileSize, y + tileSize);
        auto& subFilm = subFilms_[tileNo];
        subFilm.initialize(region, this);
    }
}

//-------------------------------------------------
// ~Film()
//-------------------------------------------------
Film::~Film()
{}

//-------------------------------------------------
// subFilmNum()
//-------------------------------------------------
int32_t Film::subFilmNum() const
{
    return (int32_t)subFilms_.size();
}

//-------------------------------------------------
// getSubFilm()
//-------------------------------------------------
SubFilm& Film::subFilm(int32_t regionNo)
{
    return subFilms_[regionNo];
}

//-------------------------------------------------
// width()
//-------------------------------------------------
const Image& Film::image()const
{
    return image_;
}

//-------------------------------------------------
// filmWidthInMM()
//-------------------------------------------------
float Film::filmWidthInMM() const
{
    return filmWidthInMM_;
}

//-------------------------------------------------
// filmWidthInMM()
//-------------------------------------------------
float Film::filmHeightInMM() const
{
    return filmHeightInMM_;
}
