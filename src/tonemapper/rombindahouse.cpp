#include "pch.hpp"
#include "tonemapper/tonemapper.hpp"

//-------------------------------------------------
// RBDH
//-------------------------------------------------
class RBDH
    : public Tonemapper
{
public:
    RBDH(const ObjectProp& objectProp);
    bool process(const Image& src, ImageLDR& dst) const override;
};

REGISTER_OBJECT(Tonemapper, RBDH);

//-------------------------------------------------
// RBDH
//-------------------------------------------------
RBDH::RBDH(const ObjectProp& objectProp)
    :Tonemapper(objectProp)
{}

//-------------------------------------------------
// process
//-------------------------------------------------
bool RBDH::process(const Image& src, ImageLDR& dst) const
{
    // TODO: https://www.shadertoy.com/view/lslGzl を参考に実装
    return false;
}
