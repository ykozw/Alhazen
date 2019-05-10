#include "core/object.hpp"
#include "core/image.hpp"
#include "sampleable2d/sampleable2d.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class SampleableImage AL_FINAL : public Sampleable2d
{
public:
    SampleableImage(const ObjectProp& prop);
    virtual Spectrum sample(float u, float v) override;

private:
    Image image_;
};
REGISTER_OBJECT(Sampleable2d, SampleableImage);

/*
-------------------------------------------------
-------------------------------------------------
*/
SampleableImage::SampleableImage(const ObjectProp& prop)
{
    const std::string fileName =
        prop.findChildBy("name", "fileName").asString("none");
    const float gamma = 2.2f; // FIXME: objectPropから取り出すようにする
    image_.readBmp(fileName, gamma);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum SampleableImage::sample(float u, float v)
{
    return image_.sample(u, v);
}
