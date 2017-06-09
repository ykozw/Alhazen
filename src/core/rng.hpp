#pragma once

#include "pch.hpp"

//-------------------------------------------------
// XorShift128
//-------------------------------------------------
class XorShift128
{
public:
    XorShift128();
    XorShift128(uint32_t seed);
    void setSeed(uint32_t seed);
    float nextFloat() const;
    uint32_t nextUint() const;
    uint32_t nextSize(int32_t size) const;
    template <typename Iterator>
    void shuffle(Iterator it1, Iterator it2);
private:
    mutable uint32_t x;
    mutable uint32_t y;
    mutable uint32_t z;
    mutable uint32_t w;
};

// TODO: PCGを追加する

typedef XorShift128 RNG;

#include "rng.inl"
