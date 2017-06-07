#include "pch.hpp"
#include "core/svgwriter.hpp"
#include "core/aabb.hpp"

#if defined(WINDOWS)
//-------------------------------------------------
//
//-------------------------------------------------
SVGPath::SVGPath(SVGWriter* svgWriter, float x, float y, float offsetX, float offsetY )
{
    svgWriter_ = svgWriter;
    strokeWidth_ = 0.1f;
    isFill_ = false;
    strokeColor_ = "black";
    offsetX_ = offsetX;
    offsetY_ = offsetY;
    //
    dAttribute_ << "M " << x + offsetX_ << " " << y + offsetY_ << " ";
}

//-------------------------------------------------
//
//-------------------------------------------------
SVGPath::SVGPath(const SVGPath& other)
{
	// TODO: ちゃんと作る
	assert(false);
}

//-------------------------------------------------
//
//-------------------------------------------------
SVGPath::~SVGPath()
{
}

//-------------------------------------------------
//
//-------------------------------------------------
SVGPath& SVGPath::setStrokeWidth(float strokeWidth)
{
    strokeWidth_ = strokeWidth;
    return *this;
}

//-------------------------------------------------
//
//-------------------------------------------------
SVGPath& SVGPath::setIsFill(bool isFill)
{
    isFill_ = isFill;
    return *this;
}

//-------------------------------------------------
//
//-------------------------------------------------
SVGPath& SVGPath::setStrokeColor(const std::string& color)
{
    strokeColor_ = color;
    return *this;
}

//-------------------------------------------------
//
//-------------------------------------------------
SVGPath& SVGPath::addLine(float x, float y)
{
    dAttribute_ << "L " << x + offsetX_ << " " << y + offsetY_ << " ";
    const auto hoge = dAttribute_.str();
    return *this;
}

//-------------------------------------------------
// addArc()
// 円弧を描画。なす角度は常に鋭角前提
//-------------------------------------------------
SVGPath& SVGPath::addArc(float r, float ex, float ey, bool isClockWise)
{
    const float xAxisRotation = 0.0f;
    const int32_t isLargeArc = 0;
    const int32_t sweapFlag = isClockWise ? 0 : 1;
    dAttribute_
            << "A " << r << " " << r << " "
            << xAxisRotation << " " << isLargeArc << " "
            << sweapFlag << " " << ex + offsetX_ << " " << ey + offsetY_ << " ";
    //
    return *this;
}

//-------------------------------------------------
// endPath()
//-------------------------------------------------
void SVGPath::endPath()
{
    // path要素の作成
    std::stringstream line;
    line << "\t\t\t<svg:path ";
    line << "d=\"" << dAttribute_.str() << "\" ";
    line << "stroke=\"" << strokeColor_ << "\" ";
    line << "stroke-width=\"" << strokeWidth_ << "\" ";
    line << "fill=\"none\" ";
    line << "/>\n";
    // 転送
    svgWriter_->writeLine(line.str());
}

//-------------------------------------------------
// SVGWriter()
//-------------------------------------------------
SVGWriter::SVGWriter(const std::string& fileName, float mnx, float mny, float mxx, float mxy)
    :file_( NULL )
{
    // mnx/mnyが負の場合は全体のオフセットを計算しておく
    offsetX_ = 0.0f;
    offsetY_ = 0.0f;
    if (mnx < 0)
    {
        offsetX_ = -mnx;
        mxx += -mnx;
        mnx = 0;
    }
    if (mny < 0)
    {
        offsetY_ = -mny;
        mxy += -mny;
        mny = 0;
    }
    //
    fopen_s(&file_, fileName.c_str(), "wt");
    AL_ASSERT_DEBUG(file_ != NULL );
    // ヘッダ書き込み
    const char* header =
        "<!DOCTYPE html>\n"
        "<html\n"
        "\txmlns = \"http://www.w3.org/1999/xhtml\"\n"
        "\txmlns:svg = \"http://www.w3.org/2000/svg\"\n"
        "\txmlns:xlink = \"http://www.w3.org/1999/xlink\"\n"
        "\txml:lang = \"ja\">\n"
        "\t<head>\n"
        "\t<meta charset = \"utf-8\"/>\n"
        "\t<title></title>\n"
        "</head>\n";
    fprintf(file_, header);
    //
    fprintf(file_, "\t<body>\n");
    fprintf(file_, "\t\t<svg:svg viewBox = \"%f %f %f %f\">\n", mnx, mny, mxx, mxy);
}

//-------------------------------------------------
// ~SVGWriter()
//-------------------------------------------------
SVGWriter::~SVGWriter()
{
    //
    fprintf(file_, "\t\t</svg:svg>\n");
    fprintf(file_, "\t</body>\n");
    fprintf(file_, "</html>\n");
    //
    fclose(file_);
}

//-------------------------------------------------
//
//-------------------------------------------------
SVGPath SVGWriter::beginPath(float x, float y)
{
    return SVGPath(this, x, y, offsetX_, offsetY_ );
}

//-------------------------------------------------
//
//-------------------------------------------------
void SVGWriter::writeLine(const std::string& line)
{
    fprintf(file_, line.c_str());
}

#if 0
//-------------------------------------------------
//
//-------------------------------------------------
class SVGWriterTest
{
public:
    SVGWriterTest()
    {
        {
            SVGWriter svg("test.svg", 0.0f, 0.0f, 10.0f, 10.0f);
            svg.beginPath(0.0f, 0.0f)
            .setStrokeColor("blue")
            .setStrokeWidth(1.0f)
            .addLine(10.0f, 0.0f)
            .endPath();
            svg.beginPath(1.0f, -1.0f)
            .setStrokeColor("green")
            .setStrokeWidth(0.2f)
            .addArc(5.0f, 1.0f, 1.0f, true)
            .endPath();
        }
    }
} test;
#endif

#endif
