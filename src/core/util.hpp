#pragma once


/*
-------------------------------------------------
-------------------------------------------------
*/
class FileSystem AL_FINAL
{
public:
    void init(const char* sceneFilePath, const char* exeFilePath);
    std::string getOutputFolderPath() const;
    std::string getSceneFileFolderPath() const;
    //
    static const char* getExt(const char* fileName);
    static void getDirPath(const std::string& fullPath,
                           std::string& aDirPath,
                           std::string& aFileName);
    static std::string readTextFileAll(const std::string& filePath);

private:
    std::string sceneFileDir_;
    std::string outputDir_;
    std::string exeDir_;
};
extern FileSystem g_fileSystem;

/*
-------------------------------------------------
-------------------------------------------------
*/
class TimeUtil AL_FINAL
{
public:
    TimeUtil();
    uint32_t elapseTimeInMs();

private:
    std::chrono::system_clock::time_point startTime_;
};
extern TimeUtil g_timeUtil;

/*
-------------------------------------------------
スピンロック
基本的にはスピンロックだが、
lock() unlock()間の時間が平均よりも大幅に増えていたら
std::mutex::lock()を呼び出す
cf. https://hackernoon.com/building-a-c-hybrid-spin-mutex-f98de535b4ac

平均的なロック時間は常にSpinLockになっていることに注意
-------------------------------------------------
*/
class SpinLock AL_FINAL
{
public:
    void lock();
    void unlock();

private:
    std::mutex mtx_;
    std::atomic<uint64_t> predictedWaitTime_ = { 1 };
};

#if 0
/*
-------------------------------------------------
最適化時に行われてしまう変数削除の最適化を
パフォーマンス計測のために抑制する
-------------------------------------------------
*/
#pragma optimize("", off)
template <class T, bool doMemFence = false>
void doNotOptimize(T&& datum)
{
    datum = datum;
    if (doMemFence)
    {
        std::_Atomic_thread_fence(std::memory_order_seq_cst);
    }
}
#pragma optimize("", on)
#endif
