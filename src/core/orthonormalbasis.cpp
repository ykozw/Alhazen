#include "pch.hpp"
#include "core/unittest.hpp"
#include "core/orthonormalbasis.hpp"
#include "core/rng.hpp"
#include "core/floatstreamstats.hpp"
#include "core/logging.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
template <typename Type>
void testLocalCoord(Type localCoord) {
  XorShift128 rng;
  FloatStreamStats dotErrorStats;
  FloatStreamStats lengthErrorStats;
  //
  std::vector<Vec3> ns;
#if 0
    ns.resize(1024 * 1024 * 128);
    for (Vec3& n : ns)
    {
        n = Vec3(
            rng.nextFloat(),
            rng.nextFloat(),
            rng.nextFloat()).normalized();
    }
#endif
  // 秘孔
  // ns.push_back(Vec3(0.00038527316, 0.00038460016, -0.99999988079));
  /*ns.push_back(Vec3(-0.00019813581, -0.00008946839, -0.99999988079));*/
  ns.push_back(Vec3(0.0003860202f, 0.0003860202f, -0.9999998808f));

  // const auto startTime = std::chrono::system_clock::now();

  //
  for (const Vec3 n : ns) {
    localCoord.set(n);
#if 1
    const Vec3 s = localCoord.getS();
    const Vec3 t = localCoord.getT();
    dotErrorStats.add(fabsf(Vec3::dot(n, s)));
    dotErrorStats.add(fabsf(Vec3::dot(n, t)));
    dotErrorStats.add(fabsf(Vec3::dot(s, t)));
    lengthErrorStats.add(fabsf(1.0f - s.length()));
    lengthErrorStats.add(fabsf(1.0f - t.length()));
#endif
  }
  // const uint32_t elapsed = timeGetTime() - start;
  // printf("Elapsed: %d\n", elapsed);
  logging("dot error    max:%g mean:%g", dotErrorStats.max(),
          dotErrorStats.mean());
  logging("length error max:%g mean:%g", lengthErrorStats.max(),
          lengthErrorStats.mean());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(OrthonormalBasis, 0) {
  /*
  ONB_METHOD::Naive
  dot error    max:9.68575e-08 mean:8.71558e-09
  length error max:4.76837e-07 mean:5.56348e-08
  ONB_METHOD::Moller99
  dot error    max:8.9407e-08 mean:7.8073e-09
  length error max:4.17233e-07 mean:4.05193e-08
  ONB_METHOD::Frisvad12
  dot error    max:4.93135e-07 mean:2.84859e-08
  length error max:2.38419e-07 mean:1.50975e-08
  ONB_METHOD::Duff17
  dot error    max:5.25266e-07 mean:2.85039e-08
  length error max:2.38419e-07 mean:1.51165e-08
  ONB_METHOD::Nelson17
  dot error    max:5.02041e-07 mean:2.8488e-08
  length error max:2.38419e-07 mean:1.51295e-08

  ほとんどかわらないので、もう少し特異点などをついたテストにするべきなのかもしれない
  */
  logging("ONB_METHOD::Naive");
  testLocalCoord(OrthonormalBasis<ONB_METHOD::Naive>());
  //
  logging("ONB_METHOD::Moller99");
  testLocalCoord(OrthonormalBasis<ONB_METHOD::Moller99>());
  //
  logging("ONB_METHOD::Frisvad12");
  testLocalCoord(OrthonormalBasis<ONB_METHOD::Frisvad12>());
  //
  logging("ONB_METHOD::Duff17");
  testLocalCoord(OrthonormalBasis<ONB_METHOD::Duff17>());
  //
  logging("ONB_METHOD::Nelson17");
  testLocalCoord(OrthonormalBasis<ONB_METHOD::Nelson17>());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
AL_TEST(OrthonormalBasis, 1) {
  // ちゃんと元の方向に戻るかテスト
  const Vec3 n = Vec3(0.5f, 1.0f, 1.5f).normalized();
  const OrthonormalBasis<> local(n);
  const Vec3 d0 = Vec3(0.0f, -1.0f, 0.0f).normalized();
  const Vec3 d1 = local.local2world(local.world2local(d0));
  AL_ASSERT_ALWAYS(Vec3::dot(d0, d1) - 0.98f > 0.0f);
}
