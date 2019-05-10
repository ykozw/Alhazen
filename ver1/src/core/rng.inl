#include "rng.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE uint32_t makeBitmask(uint32_t n)
{
    uint32_t bitmask = n;
    bitmask |= bitmask >> 1;
    bitmask |= bitmask >> 2;
    bitmask |= bitmask >> 4;
    bitmask |= bitmask >> 8;
    bitmask |= bitmask >> 16;
    return bitmask;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE XorShift128::XorShift128()
{
    x = std::random_device()();
    y = std::random_device()();
    z = std::random_device()();
    w = std::random_device()();
    // とても低い確率だが、全て0になると動かなくなってしまうため対策
    if (x == 0 &&
        y == 0 &&
        z == 0 &&
        w == 0)
    {
        x = 123456789;
        y = 362436069;
        z = 521288629;
        w = 88675123;
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE XorShift128::XorShift128(uint32_t seed)
{
    setSeed(seed);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE void XorShift128::setSeed(uint32_t seed)
{
    uint32_t s = seed;
    x = s = 1812433253U * (s ^ (s >> 30)) + 0;
    y = s = 1812433253U * (s ^ (s >> 30)) + 1;
    z = s = 1812433253U * (s ^ (s >> 30)) + 2;
    w = s = 1812433253U * (s ^ (s >> 30)) + 3;
}

/*
-------------------------------------------------
[0,1)
-------------------------------------------------
*/
INLINE float XorShift128::nextFloat() const
{
#if 0
    const float t = float(uint32_t(-1));
    const float s = 1.0f / (float)nextafterf(t, 2.0f*t);
#else
    const float s = 2.32830616e-010f;
#endif
    return (float)nextUint() * s;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
INLINE uint32_t XorShift128::nextUint() const
{
    uint32_t t = x ^ (x << 11);
    x = y; y = z; z = w;
    return w = w ^ (w >> 19) ^ t ^ (t >> 8);
}

/*
-------------------------------------------------
nextSize()
[0,指定した数値)
-------------------------------------------------
*/
INLINE uint32_t XorShift128::nextSize(int32_t size) const
{
    if (size == 0)
    {
        return 0;
    }
    const uint32_t mask = makeBitmask(size);
    //
    uint32_t result;
    while ((result = (nextUint() & mask)) >= static_cast<uint32_t>(size)) {}
    return result;
}

/*
-------------------------------------------------
shuffle
Fisher–Yates shuffleを行う
-------------------------------------------------
*/
template <typename Iterator>
INLINE void XorShift128::shuffle(Iterator it1, Iterator it2)
{
    for (Iterator it = it2 - 1; it > it1; --it)
    {
        const int32_t index = static_cast<int32_t>(it - it1);
        std::iter_swap(it, it1 + nextSize(index));
    }
}
