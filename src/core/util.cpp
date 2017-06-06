#include "pch.hpp"
#include "util.hpp"

/*
-------------------------------------------------
getExt()
-------------------------------------------------
*/
const char* getExt(const char* fileName)
{
    const char* lastDot = strrchr(fileName, '.');
    if (lastDot == NULL)
    {
        return "";
    }
    else
    {
        return lastDot;
    }
}

/*
-------------------------------------------------
getDirPath()
-------------------------------------------------
*/
void getDirPath(
    const std::string& fullPath,
    std::string& aDirPath,
    std::string& aFileName)
{
    char drive[_MAX_DRIVE + 1] = { '\0' };
    char dir[_MAX_DIR + 1] = { '\0' };
    char dirPath[_MAX_PATH + 1] = { '\0' };
    char fileName[_MAX_PATH + 1] = { '\0' };
    char extName[_MAX_PATH + 1] = { '\0' };
    _splitpath(fullPath.c_str(), drive, dir, fileName, extName);
    _makepath(dirPath, drive, dir, NULL, NULL);
    //
    aDirPath = dirPath;
    aFileName = std::string(fileName) + std::string(extName);
}

/*
-------------------------------------------------
getOutputFolderPath()
-------------------------------------------------
*/
std::string getOutputFolderPath()
{
    static std::string path = "";
    if (path != "")
    {
        return path;
    }
    // 現在の時間からフォルダ名を作成
    const time_t timer = time(NULL);
    tm date;
    localtime_s(&date, &timer);
    char outputFolderPath[0xff];
    asctime_s(outputFolderPath, &date);
    sprintf_s(outputFolderPath, "./output/%04d%02d%02d_%02d%02d%02d/",
              date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
    path = outputFolderPath;
    // パスを作成
    const std::string cmd = std::string("mkdir \"") + path + "\"";
    system(cmd.c_str());
    //
    return path;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
std::chrono::system_clock::time_point g_startTime;
class SetStartTime
{
public:
    SetStartTime()
    {
        g_startTime = std::chrono::system_clock::now();
    }
} setStartTime;
uint32_t elapseTimeInMs()
{
    const auto duration = std::chrono::system_clock::now() - g_startTime;
    int64_t elapsedInMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return uint32_t(elapsedInMs);
}
