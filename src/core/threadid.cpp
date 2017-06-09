#include "pch.hpp"
#include "threadid.hpp"

AL_TLS uint32_t Job::threadId = -1;

namespace
{
#pragma warning(disable:4073) // init_segの使用
#pragma warning(disable:4074) // init_segの使用
#pragma init_seg(lib)
class Init
{
public:
    // Global Constructorが走るスレッド = main()のスレッドという仮定がある
    Init()
    {
        Job::threadId = 0;
    }
}init;
};
