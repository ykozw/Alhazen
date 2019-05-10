#include "core/spectrum.hpp"
#include "core/image.hpp"
#include "core/object.hpp"
#include "tonemapper/tonemapper.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
SpectrumRGB SpectrumRGB::Black = SpectrumRGB(0.0f, 0.0f, 0.0f);
SpectrumRGB SpectrumRGB::White = SpectrumRGB(1.0f, 1.0f, 1.0f);
