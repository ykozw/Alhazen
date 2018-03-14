#include "pch.hpp"

/*
テストに関して
- テストのOKか否かは全てassert()でよい。
- 単体テスト実行時は全てのテストを実行する。実行時引数でトリガーされる。
- 画像を出力したりするようなものでも全て含める。テストに種類は作らない。
-
デバッグ中など、優先して実行されてほしいテストに関してはそのとき用のマクロ(IMM)を使うが、コミット時は元に戻す
*/

namespace detail {
/*
-------------------------------------------------
-------------------------------------------------
*/
void registerTest(const char* testCategory, const char* testName,
                  std::function<void(void)> testFunc);
}  // namespace detail

/*
-------------------------------------------------
-------------------------------------------------
*/
void doTest();

/*
-------------------------------------------------
テストとうろ
-------------------------------------------------
*/
#define AL_TEST(TEST_CATEGORY, TEST_NAME)                    \
  void TEST_CATEGORY##TEST_NAME##_MAIN();                    \
  class Test_##TEST_CATEGORY##TEST_NAME {                    \
   public:                                                   \
    Test_##TEST_CATEGORY##TEST_NAME() {                      \
      detail::registerTest(#TEST_CATEGORY, #TEST_NAME,       \
                           TEST_CATEGORY##TEST_NAME##_MAIN); \
    }                                                        \
  } test2_##TEST_CATEGORY##TEST_NAME;                        \
  void TEST_CATEGORY##TEST_NAME##_MAIN()

/*
-------------------------------------------------
即時実行テスト
コミット時には全て外しておくこと
-------------------------------------------------
*/
#if 0
#define AL_TEST_IMM(TEST_CATEGORY, TEST_NAME)                                \
  void TEST_CATEGORY##TEST_NAME##_MAIN();                                    \
  class Test_##TEST_CATEGORY##TEST_NAME {                                    \
   public:                                                                   \
    Test_##TEST_CATEGORY##TEST_NAME() { TEST_CATEGORY##TEST_NAME##_MAIN(); } \
  } test2_##TEST_CATEGORY##TEST_NAME;                                        \
  void TEST_CATEGORY##TEST_NAME##_MAIN()
#else
#define AL_TEST_IMM(TEST_CATEGORY, TEST_NAME)
#endif
