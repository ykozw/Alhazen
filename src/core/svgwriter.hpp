#pragma once

#include "pch.hpp"
#include "core/math.hpp"
#include "core/aabb.hpp"

#if defined(WINDOWS)

//-------------------------------------------------
// SVGPath
// SVGのPath要素を作成する
//-------------------------------------------------
class SVGPath
{
public:
    friend class SVGWriter;
    SVGPath(const SVGPath& other);
    ~SVGPath();
    // ストローク設定
    SVGPath& setStrokeWidth(float strokeWidth);
    SVGPath& setIsFill(bool isFill);
    SVGPath& setStrokeColor(const std::string& color);

    // 描画
    SVGPath& addLine(float x, float y);
    SVGPath& addArc(float r, float ex, float ey, bool isClockWise);
    void endPath();
private:
    SVGPath(SVGWriter* svgWriter, float x, float y, float offsetX, float offsetY);
private:
    SVGWriter* svgWriter_;
    float offsetX_;
    float offsetY_;
    std::stringstream dAttribute_;
    std::string strokeColor_;
    float strokeWidth_;
    bool isFill_;
};

//-------------------------------------------------
// SVGWriter
//-------------------------------------------------
class SVGWriter
{
public:
    friend SVGPath;
    SVGWriter(const std::string& fileName, float mnx, float mny, float mxx, float mxy);
    ~SVGWriter();
    SVGPath beginPath(float x, float y);
    void writeLine(const std::string& line);
private:
private:
    float offsetX_;
    float offsetY_;
    FILE* file_;
};
#endif
