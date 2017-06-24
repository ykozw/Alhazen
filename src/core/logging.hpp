#pragma once

#include "pch.hpp"

/*
 -------------------------------------------------
 ログのレベル
 -------------------------------------------------
 */
enum class LoggingLevel
{
    INFO,
    WARNING,
    TRACE,
    ERROR,
};

/*
 -------------------------------------------------
 -------------------------------------------------
 */
#define logging( format, ...)        loggingCore(LoggingLevel::INFO, format,##__VA_ARGS__)
#define loggingWarning( format, ...) loggingCore(LoggingLevel::WARNING, format,##__VA_ARGS__)
#define loggingError( format, ...)   loggingCore(LoggingLevel::ERROR, format,##__VA_ARGS__)
#define loggingWarningIf( condition, format, ...) if( condition ){ loggingCore(LoggingLevel::WARNING, format,##__VA_ARGS__ ); }
#define loggingErrorIf( condition, format, ...) if( condition ){ loggingCore(LoggingLevel::ERROR, format,##__VA_ARGS__ ); }

/*
 -------------------------------------------------
 -------------------------------------------------
 */
void loggingCore(LoggingLevel level, const char* format, ...);
