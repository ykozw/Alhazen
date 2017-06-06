#pragma once

#include "pch.hpp"
#include "core/object.hpp"
#include "core/image.hpp"

//-------------------------------------------------
// Denoiser
//-------------------------------------------------
class Denoiser
    :public Object
{
public:
    Denoiser(){}
    virtual ~Denoiser(){}
    virtual void denoise(const Image& src, Image& dst) = 0;
protected:
};
typedef std::shared_ptr<Denoiser> DenoiserPtr;
