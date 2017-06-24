#include "pch.hpp"
#include "logging.hpp"
#include "util.hpp"

FILE* file = nullptr;

//-------------------------------------------------
// initialzeLog()
// この関数はグローバル初期化では読んではいけない。
// シーンファイルのあるフォルダにログファイルを出力するため
// シーンファイルパスにカレントパスが移動した後に行う
//-------------------------------------------------
void initialzeLog()
{
    const std::string filePath = g_fileSystem.getOutputFolderPath() + "log.log";
    file = fopen(filePath.c_str(), "wt");
}

//-------------------------------------------------
//
//-------------------------------------------------
void finalizeLog()
{
    if (file)
    {
        fflush(file);
        fclose(file);
    }
}

//-------------------------------------------------
//
//-------------------------------------------------
void writeLine(const char* buffer)
{
    if (file)
    {
        fprintf(file, buffer);
    }
}

//-------------------------------------------------
//
//-------------------------------------------------
std::string getElapseStr()
{
    const int32_t elapse = g_timeUtil.elapseTimeInMs();
    // HH:MM:SSにする
    int32_t sec = elapse / 1000;
    int32_t min = sec / 60;
    const int32_t hour = min / 60;
    sec %= 60;
    min %= 60;
    char buffer[0xff];
    sprintf(buffer, "%02d:%02d:%02d", hour, min, sec);
    return buffer;
}

//-------------------------------------------------
//
//-------------------------------------------------
void loggingCore(LoggingLevel level, const char* format, ...)
{
#if !defined(WINDOWS) || true
    // 細かく対応するまではとりあえずただのprintfにしておく
    va_list valist;
    va_start(valist, format);
    vprintf(format, valist);
    va_end(valist);
    // errorの場合はここで終了させてしまう
    if(level == LoggingLevel::ERROR)
    {
        fflush(stdout);
        exit(-1);
    }
#else
    //
    AL_ASSERT_DEBUG(level == 0 || level == 1 || level == 2);
    //
    va_list valist;
    va_start(valist, format);
    const int numChar = _vscprintf(format, valist);
    char* logBuffer = (char*)alloca(numChar + 1);
    vsnprintf_s(logBuffer, numChar + 1, numChar, format, valist);
    va_end(valist);

    // タイムスタンプの作成
    const time_t timer = time(NULL);
    tm date;
    localtime_s(&date, &timer);
    char timeStamp[0xff];
    asctime_s(timeStamp, &date);
    sprintf_s(timeStamp, "%04d/%02d/%02d %02d:%02d:%02d",
        date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);

    // Warning/Error時の特別文字列
    const std::string warningErrorString = level == 1 ? "[WARNING]" : level == 2 ? "[ERROR]" : "";
    //
    std::string buffer;
    buffer += timeStamp + std::string(" ");
    buffer += std::string("(") + getElapseStr() + std::string(")");
    buffer += std::string("  ") + warningErrorString + logBuffer + "\n";

    // どのレベルでもファイルには出力する
    writeLine(buffer.c_str());

    // コンソールへの出力
    printf(buffer.c_str());

    // errorであればメッセージを出して終了する
    if (level == 2)
    {
        exit(-1);
    }
#endif
}
