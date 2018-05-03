#include "sensor/sensor.hpp"
#include "sensor/sensorutil.hpp"
#include "core/transform.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class Perspective AL_FINAL : public Sensor
{
public:
    Perspective();
    Perspective(const ObjectProp& objectProp);
    Ray generateRay(float imageX, float imageY, float& pdf) const override;
    FilmPtr film() const override;

private:
    FilmPtr film_;
    float screenWidth_;
    float screenHeight_;
    float screenAspect_;
    float invScreenWidth_;
    float invScreenHeight_;
    Vec3 perspectivePos_;
    Vec3 rightDir_;
    Vec3 upDir_;
    Vec3 forwardDir_;
    float perspectiveFovy_;
};

REGISTER_OBJECT(Sensor, Perspective);

/*
-------------------------------------------------
-------------------------------------------------
*/
Perspective::Perspective() {}

/*
-------------------------------------------------
-------------------------------------------------
*/
Perspective::Perspective(const ObjectProp& objectProp)
{
    //
    const Transform transform(objectProp.findChildByTag("transform"));
    perspectivePos_ = transform.cameraOrigin();
    upDir_ = transform.cameraUp();
    forwardDir_ = transform.cameraDir();
    rightDir_ = Vec3::cross(forwardDir_, upDir_);
    upDir_ = Vec3::cross(rightDir_, forwardDir_);
    rightDir_.normalize();
    upDir_.normalize();
    forwardDir_.normalize();
    // TODO: kc
    perspectiveFovy_ = DEG2RAD(objectProp.findChildBy("name", "fov")
                                   .asFloat(90.0f)); // TODO: デフォルト値の調査

    //
    film_ = std::make_shared<Film>(objectProp.findChildByTag("film"));
    const auto& image = film_->image();
    screenWidth_ = static_cast<float>(image.width());
    screenHeight_ = static_cast<float>(image.height());
    screenAspect_ = screenWidth_ / screenHeight_;
    invScreenWidth_ = 1.0f / screenWidth_;
    invScreenHeight_ = 1.0f / screenHeight_;
    const float tf = tan(perspectiveFovy_ * 0.5f);
    const float tfAspect = tf * screenAspect_;
    rightDir_ *= tfAspect;
    upDir_ *= tf;
    // TODO: fovAxis
    // TODO: shutterOpen
    // TODO: shutterClose
#if 0
    const float nearClip = objectProp.findChildBy("name", "nearClip").asFloat(1e-2f * 0.0001f);
    const float farClip = objectProp.findChildBy("name", "farClip").asFloat(1e4f * 0.0001f);
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Ray Perspective::generateRay(float imageX, float imageY, float& pdf) const
{
#if 1
    // レイ方向の作成
    const float xf = imageX * invScreenWidth_ - 0.5f;
    const float yf = imageY * invScreenHeight_ - 0.5f;
#else
    // デバッグ用途で中心を抜けさせる時用
    const float xf = 0.0f;
    const float yf = 0.0f;
#endif
    const Vec3 rayDir = xf * rightDir_ + yf * upDir_ + forwardDir_;
    // 常にPDFは1
    pdf = 1.0f;
    // レイを返す
    return Ray(perspectivePos_, rayDir);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
FilmPtr Perspective::film() const { return film_; }
