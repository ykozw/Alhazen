#pragma once

#include "pch.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class CriticalSection
{
public:
    // no copyable
    CriticalSection(CriticalSection& other) = delete;
    CriticalSection& operator =(const CriticalSection& other) = delete;
    //
    CriticalSection()
    {
        InitializeCriticalSection(&cs_);
    }
    ~CriticalSection()
    {
        DeleteCriticalSection(&cs_);
    }
    void enter()
    {
        EnterCriticalSection(&cs_);
    }
    void leave()
    {
        LeaveCriticalSection(&cs_);
    }
private:
    CRITICAL_SECTION cs_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
class ScopedCriticalSection
{
public:
    ScopedCriticalSection(CriticalSection& cs)
        :cs_(cs)
    {
        cs_.enter();
    }
    ~ScopedCriticalSection()
    {
        cs_.leave();
    }
private:
    CriticalSection& cs_;
};
