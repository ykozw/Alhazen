#pragma once

#include "../pch.hpp"

/*
TODO list
- DBから引っ張ってくる場合のIORを追加する
*/

/*
-------------------------------------------------
指定した硝材のD線での屈折率と、アッベ数を返す
-------------------------------------------------
*/
void getGlassSpec(const std::string& glassName, float& nd, float& vd);

/*
-------------------------------------------------
Cauchyの式から算出されたIORを返す
-------------------------------------------------
*/
class BasicIOR
{
public:
	BasicIOR() = default;
	void init(float nd = 1.0f, float vd = std::numeric_limits <float>::max());
	float ior(const float waveLength = WAVE_LENGTH_D) const;
private:
	float a_;
	float b_;
};
