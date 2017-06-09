#pragma once

#include "pch.hpp"

const char* getExt(const char* fileName);
void getDirPath( const std::string& fullPath, std::string& aDirPath, std::string& aFileName);
std::string getOutputFolderPath();
uint32_t elapseTimeInMs();
bool isMasterThreadOrSerialRegion();

// 指定したファイルの中身の全ての内容をテキストとして返す
std::string readTextFileAll(const std::string& filePath);
