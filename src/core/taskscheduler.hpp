#pragma once

#include "pch.hpp"

/*
-------------------------------------------------
シンプルなタスク実行
-------------------------------------------------
*/
class SimpleTaskScheduler
{
public:
    typedef std::function<void(int32_t threadNo)> TaskFunc;
public:
    SimpleTaskScheduler();
    ~SimpleTaskScheduler();
    int32_t numThread() const;
    void start(int32_t graySize);
    void shutdown();
    bool isTaskConsumed();
    // TODO: std::function<>をそのまま使うのではなく、
    //       カスタムアロケーター付きでいくようにしたい
    void add(const TaskFunc& task);
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};
