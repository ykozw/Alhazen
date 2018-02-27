#pragma once

/*
 -------------------------------------------------
 -------------------------------------------------
 */
class PlyFile
{
public:
    struct PlyHeader
    {
        // "PLY"
        bool hasPLY = false;
        // format
        enum class Format
        {
            NONE, ASCII, BINARY,
        } format = Format::NONE;
        // 頂点数
        int32_t numVertex = 0;
        // 面数
        int32_t numFace = 0;
    };
    
    struct PlyBody
    {
        std::vector<Vec3> vertex_;
        // NOTE: △のみ対応
        struct Face
        {
            int32_t idxs[3];
        };
        std::vector<Face> faces_;
    };
public:
    //
    void loadPly(const char* filePath);
    void debugPrint();
    
private:
    bool readHeader(FILE* file, PlyHeader& header) const;
    bool readBody(FILE* file, const PlyHeader& header, PlyBody& body) const;
    
private:
    PlyHeader header_;
    PlyBody body_;
};
