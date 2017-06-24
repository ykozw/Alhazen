#include "pch.hpp"
#include "denoiser/denoiser.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class NullDenoiser
    :public Denoiser
{
public:
    NullDenoiser(const ObjectProp& prop);;
    void denoise(const Image& src, Image& dst) override;
private:
};
REGISTER_OBJECT(Denoiser,NullDenoiser);

/*
-------------------------------------------------
-------------------------------------------------
*/
NullDenoiser::NullDenoiser(const ObjectProp& objectProp)
{

}

/*
-------------------------------------------------
-------------------------------------------------
*/
void NullDenoiser::denoise(const Image& src, Image& dst)
{
    dst = src;
}
