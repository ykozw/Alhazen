#include "pch.hpp"
#include "denoiser/denoiser.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class AdaptivePolynomial AL_FINAL : public Denoiser {
 public:
  AdaptivePolynomial(const ObjectProp& prop);
  void denoise(const Image& src, Image& dst) override;

 private:
};
REGISTER_OBJECT(Denoiser, AdaptivePolynomial);

/*
-------------------------------------------------
-------------------------------------------------
*/
AdaptivePolynomial::AdaptivePolynomial(const ObjectProp& objectProp) {}

/*
-------------------------------------------------
-------------------------------------------------
*/
void AdaptivePolynomial::denoise(const Image& src, Image& dst) {
  dst = src;
  logging("Start denoising(AdaptivePolynomial)");
  const int32_t w = src.width();
  const int32_t h = src.height();
  for (int32_t chNo = 0; chNo < 3; ++chNo) {
    for (int32_t y = 0; y < w; ++y) {
      for (int32_t x = 0; x < h; ++x) {
        // ウィンドウサイズ
        const int32_t WS = 8;
        // ウィンドウサイズの半分
        const int32_t HW = WS / 2;
        float tile[WS * WS];
        float* tp = tile;
        // ギャザーをする
        for (int32_t tx = -HW; tx <= HW; ++tx) {
          for (int32_t ty = -HW; ty <= HW; ++ty) {
            //
            const int32_t sx = alClamp(x + tx, 0, w);
            const int32_t sy = alClamp(y + ty, 0, h);
            Spectrum spec = src.pixel(sx, sy);
            *tp = *(&spec.r + chNo);
            ++tp;
          }
        }
        // TODO: 指定の範囲内で
      }
    }
  }

  //
  logging("Finish denoising(AdaptivePolynomial)");
}
