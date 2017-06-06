#pragma once

#include "pch.hpp"

void initialzeLog();
void finalizeLog();

#define logging( format, ...)        loggingCore(0, format,__VA_ARGS__)
#define loggingWarning( format, ...) loggingCore(1, format,__VA_ARGS__)
#define loggingError( format, ...)   loggingCore(2, format,__VA_ARGS__)
#define loggingWarningIf( condition, format, ...) if( condition ){ loggingCore(1, format,__VA_ARGS__ ); }
#define loggingErrorIf( condition, format, ...) if( condition ){ loggingCore(2, format,__VA_ARGS__ ); }
void loggingCore(int level, const char* format, ...);

