#pragma once


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
    float sample(float u, float* pdf, int32_t* offset = nullptr) const;
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
    Distribution1D_Optimized(std::function<float(float)> valueGen,
                             int32_t split);
    float sample(float u, float* pdf, int32_t* offset = nullptr) const;
    float pdf(int32_t index) const;

private:
    void construct(const std::vector<float>& pdf);

private:
    float numValue_;
    float invNumValue_;
    // キャッシュ効率を上げるためにcdf_を幅優先にして並び変えたCDF
    std::vector<float> cdfBreadthFirst_;
};

/*
-------------------------------------------------
https://en.wikipedia.org/wiki/Alias_method
http://www.keithschwarz.com/darts-dice-coins/
-------------------------------------------------
*/
class Distribution1D_AliasMethod
{
public:
    Distribution1D_AliasMethod();
    Distribution1D_AliasMethod(const std::vector<float>& weights);
    Distribution1D_AliasMethod(const std::initializer_list<float>& pdf);
    void construct(const std::vector<float>& weightsRaw);
    float sample(float u, float* pdf, int32_t* offset = nullptr) const;
    float pdf(int32_t index) const;
    
private:
    //
    struct AliasProb
    {
        // エイリアスのインデックス
        int32_t alias;
        // 非エイリアスの選択確率
        float prob;
        // 元々の選択確率
        float pdf;
    };
    std::vector<AliasProb> aliasProbs_;
};

//
//typedef Distribution1D_Optimized Distribution1D;
//typedef Distribution1D_Naive Distribution1D;
typedef Distribution1D_AliasMethod Distribution1D;
