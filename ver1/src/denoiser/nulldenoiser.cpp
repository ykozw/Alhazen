#include "denoiser/denoiser.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class NullDenoiser AL_FINAL : public Denoiser
{
public:
    NullDenoiser(const ObjectProp& prop);
    ;
    void denoise(const Image& src, Image& dst) override;

private:
};
REGISTER_OBJECT(Denoiser, NullDenoiser);

/*
-------------------------------------------------
-------------------------------------------------
*/
NullDenoiser::NullDenoiser(const ObjectProp& objectProp) {}

/*
-------------------------------------------------
-------------------------------------------------
*/
void NullDenoiser::denoise(const Image& src, Image& dst) { dst = src; }
