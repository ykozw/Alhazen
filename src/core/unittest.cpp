﻿#include "pch.hpp"
#include "core/logging.hpp"
#include "core/unittest.hpp"

//-------------------------------------------------
//
//-------------------------------------------------
#pragma warning(disable:4073) // init_segの使用
#pragma warning(disable:4074) // init_segの使用
#pragma init_seg(lib)

struct TestDesc
{
public:
    //
    std::string testName;
    // 実行されるテスト
    std::function<void(detail::TestContext& tcx)> testFunc;
    // doTest(void)により自動実行されるか
    bool doTestImplicit;
public:
    TestDesc(
        const std::string& aTestName,
        std::function<void(detail::TestContext& tcx)> aTestFunc,
        bool aDoTestImplicit)
        :testName(aTestName),
         testFunc(aTestFunc),
         doTestImplicit(aDoTestImplicit)
    {}
};

typedef std::unordered_map<std::string, TestDesc> TestsPerCategory;
static std::unordered_map<std::string, TestsPerCategory> g_tests;

//-------------------------------------------------
//
//-------------------------------------------------
void detail::registerTest(
    const char* testCategory,
    const char* testName,
    std::function<void(TestContext& tcx)> testFunc,
    bool doTestImplicit)
{
    auto& nameCategory = g_tests.find(testCategory);
    if (nameCategory == g_tests.end())
    {
        g_tests.insert(std::make_pair(testCategory, TestsPerCategory()));
        nameCategory = g_tests.find(testCategory);
    }
    //
    TestsPerCategory& category = nameCategory->second;
    category.insert(std::make_pair(testName, TestDesc(testName, testFunc, doTestImplicit)));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void detail::addTestResult(TestContext& tcx, bool result, const char* fileName, int32_t line)
{
    ++tcx.totalCheck;
    if (result)
    {
        ++tcx.okayCheckCount;
    }
    else
    {
        // テストが通らなかったところは行数を表示
        logging("TEST FAILED %s:%d", fileName, line );
    }
}

//-------------------------------------------------
//
//-------------------------------------------------
void doTest()
{
    const auto startTime = std::chrono::system_clock::now();
    //
    for (const auto& testCategorys : g_tests)
    {
        const std::string& categoryName = testCategorys.first;
        detail::TestContext tcx;
        for (const auto& test : testCategorys.second)
        {
            const std::string& testName = test.first;
            const auto testDesc = test.second;
            if( !testDesc.doTestImplicit )
            {
                continue;
            }
            testDesc.testFunc(tcx);
        }
        logging("TEST CATEGORY: %s (%d/%d)", categoryName.c_str(), tcx.okayCheckCount, tcx.totalCheck);
    }
    // 一定時間以上時間をテストに使っていたら警告を出す
    const auto elapseTimeRaw = std::chrono::system_clock::now() - startTime;
    const int32_t elapseTimeInMs = int32_t(std::chrono::duration_cast<std::chrono::milliseconds>(elapseTimeRaw).count());
    loggingWarningIf(elapseTimeInMs > 3000, "UnitTest is too waste time(%dms).", elapseTimeInMs);
}

//-------------------------------------------------
//
//-------------------------------------------------
void doTest(const char* testCategory, const char* testName)
{
    const auto& category = g_tests.find(testCategory);
    if (category == g_tests.end())
    {
        return;
    }
    const auto& test = category->second.find(testName);
    if (test == category->second.end())
    {
        return;
    }
    const auto& testDesc = test->second;
    detail::TestContext tcx;
    testDesc.testFunc(tcx);
}
