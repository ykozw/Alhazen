#include "pch.hpp"
#include "util.hpp"

FileSystem g_fileSystem;

/*
-------------------------------------------------
-------------------------------------------------
*/
void
FileSystem::init(const char* sceneFilePath, const char* exeFilePath)
{
    // 出力フォルダを作成する
    std::string sceneFileDir;
    std::string fileName;
    getDirPath(sceneFilePath, sceneFileDir, fileName);
    // ディレクトリ作成作成
    outputDir_ = sceneFileDir + "/output/";
#if defined(WINDOWS)
    _mkdir(outputDir_.c_str());
#else
    /*
     777を指定してもumaskが設定してある環境だとmkdir()で指定したパーミッション通り作られない。
     そのためchmod()を明示的に呼び出す必要がある。
     */
    mkdir(outputDir_.c_str(), 0777);
    chmod(outputDir_.c_str(), 0777);
#endif
    //
    std::string exeFile;
    getDirPath(std::string(exeFilePath), exeDir_, exeFile);
}

/*
-------------------------------------------------

-------------------------------------------------
*/
std::string
FileSystem::getOutputFolderPath() const
{
    if (outputDir_ != "" && outputDir_ != "/output/") {
        return outputDir_;
    }
    // 出力フォルダが決定していないときはexeがあるパスにする
    else {
        return exeDir_;
    }
}

/*
-------------------------------------------------

-------------------------------------------------
*/
const char*
FileSystem::getExt(const char* fileName)
{
    const char* lastDot = strrchr(fileName, '.');
    if (lastDot == NULL) {
        return "";
    } else {
        return lastDot;
    }
}

/*
-------------------------------------------------
TODO:
ポータビリティ、特殊なケースに対応していないので、C++0zでfilesystemが入ったら改築する
-------------------------------------------------
*/
void
FileSystem::getDirPath(const std::string& fullPath,
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
-------------------------------------------------
*/
std::string
FileSystem::readTextFileAll(const std::string& filePath)
{
    // TODO: 実装
    FILE* file = fopen(filePath.c_str(), "rt");
    if (file == nullptr) {
        return "";
    }
    // ファイルサイズを取得する
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    //　ファイルの内容を全てロードする
    std::string ret;
    ret.resize(fileSize);
    fread((void*)ret.data(), ret.size(), 1, file);
    //
    fclose(file);
    //
    return ret;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
TimeUtil g_timeUtil;

/*
-------------------------------------------------
-------------------------------------------------
*/
TimeUtil::TimeUtil()
{
    startTime_ = std::chrono::system_clock::now();
}

/*
-------------------------------------------------
-------------------------------------------------
*/
uint32_t
TimeUtil::elapseTimeInMs()
{
    const auto duration = std::chrono::system_clock::now() - startTime_;
    int64_t elapsedInMs =
      std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return uint32_t(elapsedInMs);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void
SpinLock::lock()
{
    using clock_t = std::chrono::high_resolution_clock;
    const auto before = clock_t::now();
    long long elapsed = 0;
    //
    while (!mtx_.try_lock()) {
        elapsed = (clock_t::now() - before).count();
        // いつもよりも2倍以上待ってもロックできなければ通常のロックに入る
        if (elapsed >= predictedWaitTime_ * 2) {
            mtx_.lock();
            break;
        }
    }
    // 次回以降の予想時間を今回の待ち時間に少し近づける
    predictedWaitTime_ += (elapsed - predictedWaitTime_) / 8;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void
SpinLock::unlock()
{
    mtx_.unlock();
}
