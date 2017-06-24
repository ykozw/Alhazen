#include "pch.hpp"
#include "tonemapper/tonemapper.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class UC2
    : public Tonemapper
{
public:
    UC2(const ObjectProp& objectProp);
    bool process(const Image& src, ImageLDR& dst) const override;
};

REGISTER_OBJECT(Tonemapper,UC2);

/*
-------------------------------------------------
-------------------------------------------------
*/
UC2::UC2(const ObjectProp& objectProp)
    :Tonemapper(objectProp)
{}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool UC2::process(const Image& src, ImageLDR& dst) const
{
    // TODO: https://www.shadertoy.com/view/lslGzl を参考に実装
    return false;
}
