#pragma once

#include "pch.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class FileSystem
{
public:
    void init(const char* sceneFilePath, const char* exeFilePath);
    std::string getOutputFolderPath() const;
    //
    static const char* getExt(const char* fileName);
    static void getDirPath(const std::string& fullPath, std::string& aDirPath, std::string& aFileName);
    static std::string readTextFileAll(const std::string& filePath);
private:
    std::string outputDir_;
    std::string exeDir_;
};
extern FileSystem g_fileSystem;

/*
-------------------------------------------------
-------------------------------------------------
*/
class TimeUtil
{
public:
    TimeUtil();
    uint32_t elapseTimeInMs();
private:
    std::chrono::system_clock::time_point startTime_;
};
extern TimeUtil g_timeUtil;

