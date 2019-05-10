#include "opticalsystem.hpp"
#include "ior.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
class OpticalSystem::Impl
{
public:
    Impl() = default;
    ~Impl() = default;
    bool setupFromOtx(const char* fileName);

private:
    /*
    -------------------------------------------------
    -------------------------------------------------
    */
    struct LensSurface
    {
    public:
        // ストップ面(絞り面)か
        bool isStop;
        // 開き
        float apeX;
        float apeY;
        // 球面半径
        float invRadius;
        float radius;
        float radius2;
        // 面間距離
        float distance;
        // IOR
        BasicIOR ior_;
        // 撮像素子からの距離
        float z;

    public:
        LensSurface()
            : isStop(false), radius(1.0f), radius2(1.0f), distance(1.0f)
        {
        }
    };
    typedef std::vector<LensSurface> LensSet;
    LensSet lensSet_;
};

/*
-------------------------------------------------
-------------------------------------------------
*/
bool OpticalSystem::setupFromOtx(const char* fileName)
{
    return pimpl_->setupFromOtx(fileName);
}
bool OpticalSystem::Impl::setupFromOtx(const char* fileName)
{
    lensSet_.clear();
    std::ifstream file;
    file.open(fileName);
    AL_ASSERT_DEBUG(file);
    std::string buffer;
    // int32_t surfNo = 0;
    LensSurface* curLensSurface = NULL;
    //
    while (std::getline(file, buffer))
    {
        char tag[0xff + 1] = {'\0'};
        char value[0xff + 1] = {'\0'};
        //
        if (sscanf(buffer.c_str(), "%255s %255s", tag, value) < 1)
        {
            continue;
        }
        /*
        -------------------------------------------------
        面周り以外
        -------------------------------------------------
        */
        // 無視するタグ一覧
        const char* ignoreTags =
            "!|VERS|FILE|NRD|RAIM|RAIC|PIM|PSFPATH|FNO|WL|WTW|REF|OSPF|"
            "FTYP|NFLD|FLD|MFR|MFRF|MTFAVG|AFR|DEF|KLDR|"
            "PLSC|PLSC2|AF|TRPLANE|TRXLAM|REM";
        if (strstr(ignoreTags, tag) != NULL)
        {
            continue;
        }
        /*
        -------------------------------------------------
        ゴースト周り
        -------------------------------------------------
        */
        else if (strcmp(tag, "GHO") == 0)
        {
            // とりあえず現状全て無視
            continue;
        }
        /*
        -------------------------------------------------
        面周り
        -------------------------------------------------
        */
        // 面の交代
        else if (strcmp(tag, "SUR") == 0)
        {
            lensSet_.push_back(LensSurface());
            curLensSurface = &lensSet_[lensSet_.size() - 1];
        }
        // ストップ面の場合
        else if (strcmp(tag, "STO") == 0)
        {
            curLensSurface->isStop = true;
        }
        // 半径(の逆数)
        else if (strcmp(tag, "CUY") == 0)
        {
            const float invRadius = static_cast<float>(atof(value));
            curLensSurface->invRadius = invRadius;
            const float inf = std::numeric_limits<float>::infinity();
            const float radius = (invRadius == 0.0f) ? inf : 1.0f / invRadius;
            curLensSurface->radius = radius;
            curLensSurface->radius2 = radius * radius;
        }
        // 硝材
        else if (strcmp(tag, "GLA") == 0)
        {
            float nd = 0.0f;
            float vd = 0.0f;
            getGlassSpec(value, nd, vd);
            curLensSurface->ior_.init(nd, vd);
        }
        // 曲面タイプ
        else if (strcmp(tag, "SUT") == 0)
        {
            // "S"(球面)以外の場合は受け付けない
            AL_ASSERT_DEBUG(strcmp(value, "S") == 0);
        }
        // 面間距離
        else if (strcmp(tag, "THI") == 0)
        {
            curLensSurface->distance = static_cast<float>(atof(value));
        }
        // 口径
        else if (strcmp(tag, "APE") == 0)
        {
            int32_t numApe;
            float apeX;
            float apeY;
            std::array<float, 3> apes;
            std::array<int32_t, 3> dummys;
            if (sscanf(buffer.c_str(),
                       "  APE %d %f %f %f %f %f %d %d %d",
                       &numApe,
                       &apeX,
                       &apeY,
                       &apes[0],
                       &apes[1],
                       &apes[2],
                       &dummys[0],
                       &dummys[1],
                       &dummys[2]) != 9)
            {
                AL_ASSERT_DEBUG(false);
            }
            AL_ASSERT_DEBUG(numApe == 1);
            curLensSurface->apeX = apeX;
            curLensSurface->apeY = apeY;
        }
        //
        else if (strcmp(tag, "FH") == 0)
        {
        }
        else
        {
            AL_ASSERT_DEBUG(false);
        }
    }
    // 物体側の最初のはレンズではないので削除
    lensSet_.erase(lensSet_.begin());

    // 最終面(像面側)のレンズは曲面でないことを確認
    AL_ASSERT_DEBUG(lensSet_.back().invRadius == 0.0f);

    // 最終面も特に使わないので削除
    lensSet_.erase(lensSet_.begin() + lensSet_.size() - 1);

    // 後ろ玉に相当するものに無意味な面が一枚付いている場合がある。その場合も削除
    while (lensSet_.back().invRadius == 0.0f)
    {
        // さらに一つ奥のdistanceを調整しておく
        lensSet_[lensSet_.size() - 2].distance += lensSet_.back().distance;
        // 削除
        lensSet_.erase(lensSet_.begin() + lensSet_.size() - 1);
    }

    // TODO:
    // 曲率が強すぎて、撮像素子から見えない点があるような状態がないかのチェック

    // 一番後ろの撮像素子のzが0になるようにして座標系を設定
    // 同時にレンズの硝材データのインデックスを一つずらす(撮像素子側からトレースを行うため)
    float z = 0.0f;
    for (int32_t i = static_cast<int32_t>(lensSet_.size()) - 1; i >= 0; --i)
    {
        auto& lens = lensSet_[i];
        z += lens.distance;
        lens.z = z;
        // IORデータをずらす
        if (i > 0)
        {
            auto& lensNext = lensSet_[i - 1];
            lens.ior_ = lensNext.ior_;
        }
        // 前玉前面は常に空気
        else
        {
            lens.ior_.init();
        }
    }

    return false;
}
