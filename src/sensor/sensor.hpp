#pragma once

#include "core/math.hpp"
#include "core/image.hpp"
#include "core/ray.hpp"
#include "sampler/sampler.hpp"

class Film;

/*
-------------------------------------------------
-------------------------------------------------
*/
class SubFilm AL_FINAL
{
public:
    SubFilm();
    void initialize(const Region2D& region, Film* film);
    const Region2D& region() const;
    Image& image();
    void incAddNum();
    int32_t getAddNum() const;
    void transferAddFilm();

private:
    // 加算されたイメージ
    Image image_;
    // 加算回数
    int32_t addNum_ = 0;
    // 親のフィルムのどこにあるか
    Region2D region_;
    // 親のフィルム
    Film* film_ = nullptr;
};

/*
-------------------------------------------------
Film
全てSpectrum形式で持ち、出力の段階で次の変換をかける
- hdr/bmp/matlab
- tonemapping
-------------------------------------------------
*/
class Film AL_FINAL
{
public:
    friend SubFilm;

public:
    Film(const ObjectProp& objectProp);
    virtual ~Film();
    int32_t subFilmNum() const;
    SubFilm& subFilm(int32_t regionNo);
    const Image& image() const;
    //
    float filmWidthInMM() const;
    float filmHeightInMM() const;

private:
    Image image_;
    std::vector<SubFilm> subFilms_;
    float filmWidthInMM_;
    float filmHeightInMM_;
};
typedef std::shared_ptr<Film> FilmPtr;

/*
-------------------------------------------------
-------------------------------------------------
*/
class Sensor : public Object
{
public:
    virtual ~Sensor() {}
    // TOOD: これは(x,y)の[-0.5,0.5]に修正する
    virtual Ray generateRay(float imageX, float imageY, float& pdf) const = 0;
    virtual Ray4 generateRay4(float imageX, float imageY, float pdf[4]) const
    {
        return Ray4();
    }
    virtual FilmPtr film() const = 0;

protected:
};
typedef std::shared_ptr<Sensor> SensorPtr;
