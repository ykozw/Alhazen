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
 TODO: ポータビリティ、特殊なケースに対応していないので、C++0zでfilesystemが入ったら改築する
-------------------------------------------------
*/
void getDirPath(
    const std::string& fullPath,
    std::string& aDirPath,
    std::string& aFileName)
{
#if defined(WINDOWS)
    const char split = '\\';
#else
    const char split = '/';
#endif
    const size_t fileStart = fullPath.find_last_of(split) + 1;
    aDirPath = fullPath.substr(0, fileStart);
    aFileName = fullPath.substr(fileStart, std::string::npos);
}

/*
-------------------------------------------------
getOutputFolderPath()
-------------------------------------------------
*/
std::string getOutputFolderPath()
{
    //
    static std::string path = "";
    if (path != "")
    {
        return path;
    }
    //
#if defined(WINDOWS)
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
#else
    //つぎはここから
    //const auto now = std::chrono::system_clock::now();
    AL_ASSERT_ALWAYS(false);
    return "";
#endif
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

/*
 -------------------------------------------------
 -------------------------------------------------
 */
std::string readTextFileAll(const std::string& filePath)
{
   // TODO: 実装
    FILE* file = fopen(filePath.c_str(),"rt");
    if( file == nullptr )
    {
        return "";
    }
    // ファイルサイズを取得する
    fseek(file,0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file,0, SEEK_SET);
    //　ファイルの内容を全てロードする
    std::string ret;
    ret.resize(fileSize);
    fread((void*)ret.data(),ret.size(),1,file);
    //
    fclose(file);
    //
    return ret;
}
