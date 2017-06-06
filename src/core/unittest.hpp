#include "pch.hpp"

namespace detail
{
/*
-------------------------------------------------
-------------------------------------------------
*/
struct TestContext
{
    /*std::string testCategory;
    std::string testName;*/
    int32_t okayCheckCount = 0;
    int32_t totalCheck = 0;
};
/*
-------------------------------------------------
-------------------------------------------------
*/
void registerTest(
    const char* testCategory,
    const char* testName,
    std::function<void(TestContext& tcx)> testFunc,
    bool doTestImplicit);

/*
-------------------------------------------------
-------------------------------------------------
*/
void addTestResult(TestContext& tcx, bool result, const char* fileName, int32_t line);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void doTest();

/*
-------------------------------------------------
-------------------------------------------------
*/
void doTest(const char* testCategory, const char* testName);

/*
-------------------------------------------------
-------------------------------------------------
*/
#if 0
#define AL_TEST(TEST_CATEGORY,TEST_NAME) \
    void TEST_CATEGORY##TEST_NAME##_MAIN(detail::TestContext& txc); \
    class Test_##TEST_CATEGORY##TEST_NAME \
    { \
    public: \
        Test_##TEST_CATEGORY##TEST_NAME() \
        { \
            registerTest(#TEST_CATEGORY,#TEST_NAME,TEST_CATEGORY##TEST_NAME##_MAIN,true); \
        } \
    }test2_##TEST_CATEGORY##TEST_NAME; \
    void TEST_CATEGORY##TEST_NAME##_MAIN(detail::TestContext& _txc)
#else
#define AL_TEST(TEST_CATEGORY,TEST_NAME) \
    void TEST_CATEGORY##TEST_NAME##_MAIN(detail::TestContext& _txc)
#endif

#define AL_TEST_IMM(TEST_CATEGORY,TEST_NAME) \
    void TEST_CATEGORY##TEST_NAME##_MAIN(detail::TestContext& txc); \
    class Test_##TEST_CATEGORY##TEST_NAME \
    { \
    public: \
        Test_##TEST_CATEGORY##TEST_NAME() \
        { \
            detail::TestContext ctx; \
            TEST_CATEGORY##TEST_NAME##_MAIN(ctx); \
        } \
    }test2_##TEST_CATEGORY##TEST_NAME; \
    void TEST_CATEGORY##TEST_NAME##_MAIN(detail::TestContext& _txc)

/*
-------------------------------------------------
-------------------------------------------------
*/
#define AL_TEST_CHECK(expr)  detail::addTestResult(_txc, (expr),__FILE__,__LINE__);
