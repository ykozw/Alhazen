#pragma once

#include "pch.hpp"
#include "bsdf/bsdf.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class Material
{
public:
    Material() = default;
    ~Material() = default;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class MatteMaterial : public Material
{
public:
    // TODO: テクスチャなどを受け付けるようにする
    MatteMaterial() = default;
    BSDFPtr genBSDF() const;
};
