#pragma once

/*
 ------------------------------------------------
 参考:
 ------------------------------------------------
 */
template<typename Fn>
void
parallelFor(int32_t n, Fn&& jobFun)
{
    if (n == 0) {
        return;
    }
#if 1
    // https://github.com/PixarAnimationStudios/USD/blob/master/pxr/base/lib/work/loops.h
    // TBB
    class TbbAdapter
    {
    public:
        TbbAdapter(Fn& fn)
          : _fn(fn)
        {}
        void operator()(const tbb::blocked_range<int32_t>& r) const
        {
            std::forward<Fn>(_fn)(r.begin(), r.end());
        }

    private:
        Fn& _fn;
    };
    /*
     isolated指定は、tbb::parallel_for()戻った後に
     呼び出したスレッドに戻ってくる事を意味している。
     これが指定されない場合は、TLSに保存している値に触れられなくなってしまうため指定している。
     */
    tbb::task_group_context ctx(tbb::task_group_context::isolated);
    tbb::parallel_for(
      tbb::blocked_range<int32_t>(0, n), TbbAdapter(jobFun), ctx);
#elif 0
    // OMP
    const int32_t GRAIN_SIZE = 16;
    const int32_t numGrain = (n + GRAIN_SIZE - 1) / GRAIN_SIZE;
#pragma omp parallel for schedule(dynamic, GRAIN_SIZE)
    for (int32_t i = 0; i < numGrain; ++i) {
        const int32_t begin = i * GRAIN_SIZE;
        const int32_t end = std::min(((i + 1) * GRAIN_SIZE), n);
        jobFun(begin, end);
    }
#elif 0
    // MSVC
    const int32_t GRAIN_SIZE = 16;
    const int32_t numGrain = (n + GRAIN_SIZE - 1) / GRAIN_SIZE;
    Concurrency::parallel_for(0, numGrain, [=](int32_t idx) {
        const int32_t begin = GRAIN_SIZE * idx;
        const int32_t end = std::min(((i + 1) * GRAIN_SIZE), n);
        jobFun(begin, end);
    });
#else
    // シリアル実行
    jobFun(0, n);
#endif
}
