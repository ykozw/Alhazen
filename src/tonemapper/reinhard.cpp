﻿#include "pch.hpp"
#include "tonemapper/tonemapper.hpp"

//-------------------------------------------------
// Reinhard
// Y成分保存(白保存？)版Reinhard
//-------------------------------------------------
class Reinhard
    : public Tonemapper
{
public:
    Reinhard(const ObjectProp& objectProp);
    bool process(const Image& src, ImageLDR& dst) const override;
};

REGISTER_OBJECT(Tonemapper,Reinhard);

//-------------------------------------------------
// Reinhard
//-------------------------------------------------
Reinhard::Reinhard(const ObjectProp& objectProp)
    :Tonemapper(objectProp)
{}

//-------------------------------------------------
// process
//-------------------------------------------------
bool Reinhard::process(const Image& src, ImageLDR& dst) const
{
    // TODO: https://www.shadertoy.com/view/lslGzl を参考に実装
    // TODO: whitePreservingLumaBasedReinhardToneMapping
    return false;
}
