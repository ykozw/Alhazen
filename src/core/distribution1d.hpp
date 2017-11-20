#pragma once

#include "pch.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class Distribution1D_Naive AL_FINAL
{
public:
    Distribution1D_Naive();
    Distribution1D_Naive(const std::vector<float>& values);
    Distribution1D_Naive(const std::initializer_list<float>& pdf);
    Distribution1D_Naive(const float* values, int32_t num);
    Distribution1D_Naive(std::function<float(float)> valueGen, int32_t split);
    float sample(float u, _Out_ float* pdf, int32_t* offset = NULL) const;
    float pdf(int32_t index) const;
private:
    void construct(const std::vector<float>& pdf);
private:
    float numValue_;
    float invNumValue_;
    // 昇順にソートされたCDF
    std::vector<float> cdf_;
};

/*
-------------------------------------------------
Distribution1D_Optimized
キャッシュ効率を上げるためにCDFを幅優先で並び変える。2割程度の速度向上になる。
cf. http://bannalia.blogspot.jp/2015/06/cache-friendly-binary-search.html
-------------------------------------------------
*/
class Distribution1D_Optimized AL_FINAL
{
public:
    Distribution1D_Optimized();
    Distribution1D_Optimized(const std::vector<float>& values);
    Distribution1D_Optimized(const std::initializer_list<float>& pdf);
    Distribution1D_Optimized(const float* values, int32_t num);
    Distribution1D_Optimized(std::function<float(float)> valueGen, int32_t split);
    float sample(float u, _Out_ float* pdf, int32_t* offset = NULL) const;
    float pdf(int32_t index) const;
private:
    void construct(const std::vector<float>& pdf);
private:
    float numValue_;
    float invNumValue_;
    // キャッシュ効率を上げるためにcdf_を幅優先にして並び変えたCDF
    std::vector<float> cdfBreadthFirst_;
};

// 
#if 0
typedef Distribution1D_Optimized Distribution1D;
#else
typedef Distribution1D_Naive Distribution1D;
#endif
