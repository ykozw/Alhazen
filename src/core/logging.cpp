#include "pch.hpp"
#include "logging.hpp"
#include "util.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
void loggingCore(LoggingLevel level, const char* format, ...)
{
    //
    const char* levelStr =
        (level == LoggingLevel::INFO) ?   "[INFO] " :
        (level == LoggingLevel::TRACE) ?  "[TRACE]" :
        (level == LoggingLevel::WARNING) ?"[WARN] " :
        (level == LoggingLevel::ERROR) ?  "[ERR]  " : 
                                          "[???]  ";
    const time_t timer = time(nullptr);
    tm date;
    localtime_s(&date, &timer);
    char timeStamp[0xff];
    asctime_s(timeStamp, &date);
    printf("%s [%04d/%02d/%02d %02d:%02d:%02d] ",
        levelStr,
        date.tm_year + 1900,
        date.tm_mon + 1,
        date.tm_mday,
        date.tm_hour,
        date.tm_min,
        date.tm_sec);

    // 本体の出力
    va_list valist;
    va_start(valist, format);
    vprintf(format, valist);
    va_end(valist);
    //
    printf("\n");
    // errorの場合はここで終了させてしまう
    if(level == LoggingLevel::ERROR)
    {
        fflush(stdout);
        exit(-1);
    }
}
