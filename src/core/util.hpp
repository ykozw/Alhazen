#pragma once

#include "pch.hpp"

const char* getExt(const char* fileName);
void getDirPath( const std::string& fullPath, std::string& aDirPath, std::string& aFileName);
std::string getOutputFolderPath();
uint32_t elapseTimeInMs();
bool isMasterThreadOrSerialRegion();
