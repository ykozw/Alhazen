#include "pch.hpp"
#include "sampleable2d/sampleable2d.hpp"
#include "core/object.hpp"

/*
-------------------------------------------------
OpticalResolutionChart
解像力チャート。レンズの評価に使用。
-------------------------------------------------
*/
class OpticalResolutionChart
    :public Sampleable2d
{
public:
    OpticalResolutionChart(const ObjectProp& prop);
    virtual Spectrum sample(float u, float v) override;
private:
};
REGISTER_OBJECT(Sampleable2d,OpticalResolutionChart);

/*
-------------------------------------------------
-------------------------------------------------
*/
OpticalResolutionChart::OpticalResolutionChart(const ObjectProp& prop)
{
    // TODO: 実装
}

/*
-------------------------------------------------
-------------------------------------------------
*/
Spectrum OpticalResolutionChart::sample(float u, float v)
{
    // TODO: 実装
    return Spectrum::createAsBlack();
}
