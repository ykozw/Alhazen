#include "pch.hpp"
#include "core/rng.hpp"
#include "core/floatstreamstats.hpp"
#include "core/unittest.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(RNGTest, Xor128Test) {
  // 一様に分布しているかのチェック
  XorShift128 rng;

  FloatStreamStats statRnd;
  for (int32_t i = 0; i < 10000; ++i) {
    statRnd.add(rng.nextFloat());
  }
  const float m = statRnd.mean();
  const float v = statRnd.variance();
  // 一様分布の分散は(b-a)^2/12
  const float ratio = v / (1.0f / 12.0f);
  if (fabs(m - 0.5f) > 0.01f) {
    return;
  }
  if (fabs(1.0f - ratio) > 0.01f) {
    return;
  }
  //
  return;
}
