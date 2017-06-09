#include "pch.hpp"
#include "core/taskscheduler.hpp"
#include "core/threadid.hpp"
#include "core/logging.hpp"

/*
-------------------------------------------------
Readers窶努riter lock縺ェconcurrent_queue
-------------------------------------------------
*/
template <class T>
class concurrent_queue
{
private:
    std::deque<T> que_;
    mutable std::shared_timed_mutex mutex_;
public:
    /*
    -------------------------------------------------
    -------------------------------------------------
    */
    concurrent_queue(){}
    concurrent_queue(const concurrent_queue&) = delete;
    concurrent_queue& operator=(const concurrent_queue&) = delete;

    /*
    -------------------------------------------------
    -------------------------------------------------
    */
    void push(const T& x)
    {
        std::lock_guard<std::shared_timed_mutex> lock(mutex_);
        que_.push_back(x);
    }

    /*
    -------------------------------------------------
    -------------------------------------------------
    */
    void push(T&& x)
    {
        std::lock_guard<std::shared_timed_mutex> lock(mutex_);
        que_.push_back(std::move(x));
    }

    /*
    -------------------------------------------------
    -------------------------------------------------
    */
    bool try_pop(T& x) noexcept
    {
        std::lock_guard<std::shared_timed_mutex> lock(mutex_);
        if (que_.empty())
        {
            return false;
        }

        x = std::move(que_.front());
        que_.pop_front();
        return true;
    }

    /*
    -------------------------------------------------
    -------------------------------------------------
    */
    void clear()
    {
        std::lock_guard<std::shared_timed_mutex> lock(mutex_);
        que_.clear();
    }

    /*
    -------------------------------------------------
    -------------------------------------------------
    */
    std::size_t size() const
    {
        std::shared_lock<std::shared_timed_mutex> lock(mutex_);
        return que_.size();
    }
    /*
    -------------------------------------------------
    -------------------------------------------------
    */
    bool empty() const
    {
        std::shared_lock<std::shared_timed_mutex> lock(mutex_);
        return que_.empty();
    }
};

//-------------------------------------------------
//
//-------------------------------------------------
class SimpleTaskScheduler::Impl
{
public:
    Impl();
    ~Impl() = default;
    int32_t numThread() const;
    void start(int32_t graySize);
    void shutdown();
    bool isTaskConsumed();
    void add(const TaskFunc& task);
private:
    concurrent_queue<TaskFunc> tasks_;
    std::vector<std::thread> threads_;
    std::atomic<bool> exitFlag_;
};

//-------------------------------------------------
//
//-------------------------------------------------
SimpleTaskScheduler::SimpleTaskScheduler()
{
    impl_ = std::make_unique<SimpleTaskScheduler::Impl>();
}
SimpleTaskScheduler::Impl::Impl()
{
}

//-------------------------------------------------
//
//-------------------------------------------------
SimpleTaskScheduler::~SimpleTaskScheduler()
{

}

//-------------------------------------------------
//
//-------------------------------------------------
int32_t SimpleTaskScheduler::numThread() const
{
    return impl_->numThread();
}
int32_t SimpleTaskScheduler::Impl::numThread() const
{
#if 1
    return (int32_t)std::thread::hardware_concurrency() - 1;
#else
    return 1;
#endif
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SimpleTaskScheduler::add(const TaskFunc& task)
{
    impl_->add(task);
}
void SimpleTaskScheduler::Impl::add(const TaskFunc& task)
{
    tasks_.push(task);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SimpleTaskScheduler::start(int32_t graySize)
{
    impl_->start(graySize);
}
void SimpleTaskScheduler::Impl::start(int32_t graySize)
{
    // 繝ッ繝シ繧ォ繝シ繧ケ繝ャ繝ラ繧ケ繝ャ繝ラ繧定オキ蜍輔☆繧    const int32_t workerThreadNum = numThread();
    for (int32_t threadNo = 0; threadNo < workerThreadNum; ++threadNo)
    {
        threads_.push_back(
            std::thread([this, threadNo]()
        {
            // スレッドIDを格納しておく
            Job::threadId = threadNo + 1;
            //
            for (;;)
            {
                // 終了フラグが立っていれば終了
                if (exitFlag_.load(std::memory_order_acquire))
                {
                    break;
                }

                // タスクがなければ停止して待つ
                if (tasks_.empty())
                {
#if 0
                    using namespace std::chrono_literals;
                    std::this_thread::sleep_for(10ms);
#else
                    std::this_thread::yield();
#endif
                    continue;
                }
                // タスクの取り出しと実行
                TaskFunc task;
                if (tasks_.try_pop(task))
                {
                    task(threadNo);
                }
            }
            // スレッドIDを格納しておく
            Job::threadId = threadNo; // TODO: 戻す操作がとても恰好が悪い
        }));
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void SimpleTaskScheduler::shutdown()
{
    impl_->shutdown();
}
void SimpleTaskScheduler::Impl::shutdown()
{
    // 終了フラグを立てる
    exitFlag_.store(true, std::memory_order_release);
    // 全てのスレッドが終了していることを確認する
    for (auto& thread : threads_)
    {
        thread.join();
    }
}

/*
-------------------------------------------------
キューにタスクがなくなったか？
※ 現在実行中のタスクは存在しうる
-------------------------------------------------
*/
bool SimpleTaskScheduler::isTaskConsumed()
{
    return impl_->isTaskConsumed();
}
bool SimpleTaskScheduler::Impl::isTaskConsumed()
{
    return tasks_.empty();
}
