#include "pch.hpp"
#include "ior.hpp"

//-------------------------------------------------
//
//-------------------------------------------------
void BasicIOR::init(float nd, float vd)
{
	const float tmp = (nd - 1.0f) / vd;
	a_ = nd + 1.5168f * tmp;
	b_ = 0.523655f * tmp;
}

//-------------------------------------------------
//
//-------------------------------------------------
float BasicIOR::ior(const float waveLength ) const
{
	const float iw = 1.0f / waveLength;
	const float iw2 = iw * iw;
	const float ior = a_ + iw2 * b_;
	return ior;
}

