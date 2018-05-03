#pragma once


/*
-------------------------------------------------
for (auto i : Range(0, 10)){}
のように使います
-------------------------------------------------
*/
struct Range
{
public:
    class Ite
    {
    public:
        INLINE Ite(int_fast32_t c) : counter_(c) {}
        INLINE const int_fast32_t& operator*() { return counter_; }
        INLINE Ite& operator++()
        {
            ++counter_;
            return *this;
        }
        INLINE bool operator!=(const Ite& v) { return counter_ != v.counter_; }
    private:
        int_fast32_t counter_ = 0;
    };
public:
    INLINE Range(int_fast32_t b, int_fast32_t e)
        :begin_(b), end_(e)
    {}
    INLINE Ite begin() { return Ite(begin_); }
    INLINE Ite end() { return Ite(end_); }
public:
    int_fast32_t begin_ = 0;
    int_fast32_t end_ = 0;
};

/*
-------------------------------------------------
for (auto i : step(10)){}
のように使います
-------------------------------------------------
*/
INLINE Range step(int_fast32_t count)
{
    return Range(0, count);
}
