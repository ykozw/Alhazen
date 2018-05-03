#pragma once


/*
 -------------------------------------------------
 XorShift128
 -------------------------------------------------
 */
class XorShift128 AL_FINAL
{
public:
    XorShift128();
    XorShift128(uint32_t seed);
    void setSeed(uint32_t seed);
    float nextFloat() const;
    uint32_t nextUint() const;
    uint32_t nextSize(int32_t size) const;
    template<typename Iterator>
    void shuffle(Iterator it1, Iterator it2);

private:
    mutable uint32_t x;
    mutable uint32_t y;
    mutable uint32_t z;
    mutable uint32_t w;
};

// TODO: PCGを追加する

typedef XorShift128 RNG;

/*
 -------------------------------------------------
 std::hash<int32_t>は入力の値をそのまま返してくるので使わないものを別途用意している
 -------------------------------------------------
 */
class Hash AL_FINAL
{
public:
    //
    static uint32_t hash(uint32_t x)
    {
        x = ((x >> 16) ^ x) * UINT32_C(0x45d9f3b);
        x = ((x >> 16) ^ x) * UINT32_C(0x45d9f3b);
        x = (x >> 16) ^ x;
        return x;
    }
    //
    static uint64_t hash(uint64_t x)
    {
        x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
        x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
        x = x ^ (x >> 31);
        return x;
    }
};

#include "rng.inl"
