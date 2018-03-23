#include "pch.hpp"
#include "shape/plyfile.hpp"
#include "core/math.hpp"

/*
 -------------------------------------------------
 -------------------------------------------------
 */
void PlyFile::loadPly(const char* filePath)
{
    //
    FILE* file = fopen(filePath, "rb");
    if (file == nullptr)
    {
        return;
    }

    readHeader(file, header_);
    //
    if (!header_.hasPLY)
    {
        fclose(file);
        return;
    }
    // 本体のロード
    switch (header_.format)
    {
    case PlyHeader::Format::ASCII:
        readBodyAsAscii(file, header_, body_);
        break;
    case PlyHeader::Format::BINARY:
        readBodyAsBinary(file, header_, body_);
        break;
    default:
        AL_ASSERT_ALWAYS(false);
        break;
    }

    //
    fclose(file);
}

/*
 -------------------------------------------------
 Headerのロード
 -------------------------------------------------
 */
bool PlyFile::readHeader(FILE* file, PlyHeader& header) const
{
    if (file == nullptr)
    {
        return false;
    }
    //
    std::array<char, 0xff> buffer, tmp0, tmp1;
    fgets(buffer.data(), buffer.size(), file);
    if (strncmp(buffer.data(), "ply/n", 3) != 0)
    {
        return false;
    }
    header.hasPLY = true;
    //
    fgets(buffer.data(), buffer.size(), file);
    if (sscanf(buffer.data(), "format %s", tmp0.data()) != 1)
    {
        return false;
    }
    // binaryフォーマットは非対応
    if (strstr(tmp0.data(), "binary") != 0)
    {
        header.format = PlyHeader::Format::BINARY;
    }
    else
    {
        header.format = PlyHeader::Format::ASCII;
    }
    //
    for (;;)
    {
        if (fgets(buffer.data(), buffer.size(), file) == nullptr)
        {
            return false;
        }
        if (strncmp(buffer.data(), "end_header", 10) == 0)
        {
            break;
        }
        //
        if (sscanf(buffer.data(),
                   "element %[^ ] %[^ ]",
                   tmp0.data(),
                   tmp1.data()) == 2)
        {
            if (strcmp(tmp0.data(), "vertex") == 0)
            {
                header.numVertex = atoi(tmp1.data());
            }
            if (strcmp(tmp0.data(), "face") == 0)
            {
                header.numFace = atoi(tmp1.data());
            }
        }
    }

    return true;
}

/*
 -------------------------------------------------
 Bodyのロード
 -------------------------------------------------
 */
bool PlyFile::readBodyAsAscii(FILE* file,
                              const PlyFile::PlyHeader& header,
                              PlyBody& body) const
{
    std::array<char, 0xff> buffer, tmp0, tmp1, tmp2, tmp3;
    // 頂点データをロード
    auto& vtxs = body.vertex_;
    vtxs.reserve(header.numVertex);
    for (int32_t vi = 0; vi < header.numVertex; ++vi)
    {
        if (fgets(buffer.data(), buffer.size(), file) == nullptr)
        {
            return false;
            ;
        }
        // HACK: 最初の3要素がそのままXYZになっているフォーマットだけロード
        if (sscanf(buffer.data(),
                   "%[^ ] %[^ ] %[^ ]",
                   tmp0.data(),
                   tmp1.data(),
                   tmp2.data()) == 3)
        {
            const float x = atof(tmp0.data());
            const float y = atof(tmp1.data());
            const float z = atof(tmp2.data());
            const Vec3 pos(x, y, z);
            vtxs.push_back(pos);
        }
    }
    // 面データをロード
    auto& faces = body.faces_;
    faces.reserve(header.numFace);
    for (int32_t fi = 0; fi < header.numFace; ++fi)
    {
        if (fgets(buffer.data(), buffer.size(), file) == nullptr)
        {
            return false;
            ;
        }
        if (sscanf(buffer.data(),
                   "%[^ ] %[^ ] %[^ ] %[^ ]",
                   tmp0.data(),
                   tmp1.data(),
                   tmp2.data(),
                   tmp3.data()) == 4)
        {
            // HACK:
            // 最初の4要素がそのままindexになっているフォーマットだけロード
            if (atoi(tmp0.data()) == 3)
            {
                const int32_t idx0 = atoi(tmp1.data());
                const int32_t idx1 = atoi(tmp2.data());
                const int32_t idx2 = atoi(tmp3.data());
                faces.push_back({idx0, idx1, idx2});
            }
        }
    }

    //
    return true;
}

/*
 -------------------------------------------------
 Bodyのロード
 -------------------------------------------------
 */
bool PlyFile::readBodyAsBinary(FILE* file,
                               const PlyFile::PlyHeader& header,
                               PlyBody& body) const
{
    // 頂点データをロード
    auto& vtxs = body.vertex_;
    for (int32_t vi = 0; vi < header.numVertex; ++vi)
    {
        float x, y, z;
        fread(&x, sizeof(float), 1, file);
        fread(&y, sizeof(float), 1, file);
        fread(&z, sizeof(float), 1, file);
        const Vec3 pos(x, y, z);
        vtxs.push_back(pos);
    }

    return false;
}
