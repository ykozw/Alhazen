#pragma once

#include "pch.hpp"

/*
 -------------------------------------------------
 ログのレベル
 -------------------------------------------------
 */
enum class LoggingLevel
{
    LOG_INFO,
    LOG_WARNING,
    LOG_TRACE,
    LOG_ERROR,
};

/*
 -------------------------------------------------
 -------------------------------------------------
 */
#define logging(format, ...)                                                   \
    loggingCore(LoggingLevel::LOG_INFO, format, ##__VA_ARGS__)
#define loggingWarning(format, ...)                                            \
    loggingCore(LoggingLevel::LOG_WARNING, format, ##__VA_ARGS__)
#define loggingError(format, ...)                                              \
    loggingCore(LoggingLevel::LOG_ERROR, format, ##__VA_ARGS__)
#define loggingWarningIf(condition, format, ...)                               \
    if (condition) {                                                           \
        loggingCore(LoggingLevel::LOG_WARNING, format, ##__VA_ARGS__);             \
    }
#define loggingErrorIf(condition, format, ...)                                 \
    if (condition) {                                                           \
        loggingCore(LoggingLevel::LOG_ERROR, format, ##__VA_ARGS__);               \
    }

/*
 -------------------------------------------------
 -------------------------------------------------
 */
void
loggingCore(LoggingLevel level, const char* format, ...);
