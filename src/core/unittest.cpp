#include "pch.hpp"
#include "core/logging.hpp"
#include "core/unittest.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
struct TestDesc
{
public:
    //
    std::string testName;
    // 実行されるテスト
    std::function<void(void)> testFunc;
public:
    TestDesc(
        const std::string& aTestName,
        std::function<void(void)> aTestFunc)
        :testName(aTestName),
        testFunc(aTestFunc)
    {}
};

/*
-------------------------------------------------
-------------------------------------------------
*/
typedef std::unordered_map<std::string, TestDesc> TestsPerCategory;
static std::unordered_map<std::string, TestsPerCategory>& getTests()
{
    static std::unordered_map<std::string, TestsPerCategory> tests;
    return tests;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void detail::registerTest(
    const char* testCategory,
    const char* testName,
    std::function<void(void)> testFunc )
{
    auto& tests = getTests();
    auto nameCategory = tests.find(testCategory);
    if (nameCategory == tests.end())
    {
        tests.insert(std::make_pair(testCategory, TestsPerCategory()));
        nameCategory = tests.find(testCategory);
    }
    //
    TestsPerCategory& category = nameCategory->second;
    category.insert(std::make_pair(testName, TestDesc(testName, testFunc)));
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void doTest()
{
    const auto startTime = std::chrono::system_clock::now();
    //
    auto& tests = getTests();
    // 全てのテストの数をカウントする
    const auto numTestTotal = [&tests]()
    {
        int32_t numTestTotal = 0;
        for (const auto& testCategorys : tests)
        {
            numTestTotal += int32_t(testCategorys.second.size());
        }
        return numTestTotal;
    }();

    //
    int32_t testCount = 0;
    for (const auto& testCategorys : tests)
    {
        const std::string& categoryName = testCategorys.first;
        for (const auto& test : testCategorys.second)
        {
            const std::string& testName = test.first;
            logging("UNIT TEST (%d/%d) [%s:%s]", testCount, numTestTotal, categoryName.c_str(), testName.c_str());
            ++testCount;
            //
            const auto testDesc = test.second;
            testDesc.testFunc();
            //
        }
    }
    // 一定時間以上時間をテストに使っていたら警告を出す
    const auto elapseTimeRaw = std::chrono::system_clock::now() - startTime;
    const int32_t elapseTimeInMs = int32_t(std::chrono::duration_cast<std::chrono::milliseconds>(elapseTimeRaw).count());
    loggingWarningIf(elapseTimeInMs > 3000, "UnitTest is too waste time(%dms).", elapseTimeInMs);
}
