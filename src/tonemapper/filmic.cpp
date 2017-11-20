#include "pch.hpp"
#include "tonemapper/tonemapper.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class Filmic AL_FINAL
    : public Tonemapper
{
public:
    Filmic(const ObjectProp& objectProp);
    bool process(const Image& src, ImageLDR& dst) const override;
};

REGISTER_OBJECT(Tonemapper,Filmic);

/*
-------------------------------------------------
-------------------------------------------------
*/
Filmic::Filmic(const ObjectProp& objectProp)
    :Tonemapper(objectProp)
{
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Filmic::process(const Image& src, ImageLDR& dst) const
{
    // TODO: https://www.shadertoy.com/view/lslGzl を参考に実装
    return false;
}
