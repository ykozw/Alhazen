#pragma once

//-------------------------------------------------
// RefArray
//-------------------------------------------------
template<typename T>
class RefArray
{
public:
    RefArray()
        :values_(NULL), numElements_(0)
    {
    }
    RefArray(T* values, int32_t numElements)
        :values_(values), numElements_(numElements)
    {
    }
    int32_t size() const
    {
        return numElements_;
    }
    RefArray<T> subArray( int32_t start, int32_t end )
    {
        assert(0 <= start && start < numElements_);
        assert(0 <= end && end <= numElements_);
        return RefArray<T>(values_ + start, end - start);
    }
    T* begin()
    {
        return values_;
    }
    T* end()
    {
        return values_ + numElements_;
    }
    const T* begin()const
    {
        return values_;
    }
    const T* end()const
    {
        return values_ + numElements_;
    }
    T& operator[](int32_t index)
    {
        assert(0 <= index && index < numElements_);
        return values_[index];
    }
    const T& operator[](int32_t index) const
    {
        assert(0 <= index && index < numElements_);
        return values_[index];
    }
private:
    T* values_;
    int32_t numElements_;
};
