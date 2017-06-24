#include "pch.hpp"
#include "mesh.hpp"
#include "core/math.hpp"
#include "core/logging.hpp"

/*
-------------------------------------------------
readFFormat1()
f v1 v2 v3 ....のフォーマット
-------------------------------------------------
*/
static bool readFFormat1(
    const char* str,
    int32_t* vi0,
    int32_t* vi1,
    int32_t* vi2,
    int32_t* ti0,
    int32_t* ti1,
    int32_t* ti2,
    int32_t* ni0,
    int32_t* ni1,
    int32_t* ni2 )
{
    //
    static_cast<void>(ti0);
    static_cast<void>(ti1);
    static_cast<void>(ti2);
    static_cast<void>(ni0);
    static_cast<void>(ni1);
    static_cast<void>(ni2);
    // "f 272 204 205"
    if (sscanf(
                str,
                "f %d %d %d",
                vi0, vi1, vi2 ) != 3)
    {
        return false;
    }
    return true;
}

/*
-------------------------------------------------
readFFormat2()
f v1/t1/n1 v2/t2/n2 v3/t3/n3 のフォーマット
-------------------------------------------------
*/
static bool readFFormat2(
    const char* str,
    int32_t* vi0,
    int32_t* vi1,
    int32_t* vi2,
    int32_t* ti0,
    int32_t* ti1,
    int32_t* ti2,
    int32_t* ni0,
    int32_t* ni1,
    int32_t* ni2 )
{
    // "f 272/1/1 204/2/2 205/3/3"
    if (sscanf(
                str,
                "f %d/%d/%d %d/%d/%d %d/%d/%d",
                vi0, ti0, ni0,
                vi1, ti1, ni1,
                vi2, ti2, ni2 ) != 9)
    {
        return false;
    }
    return true;
}

/*
-------------------------------------------------
readFFormat2()
f v1//n1 v2//n2 v3//n3 のフォーマット
-------------------------------------------------
*/
static bool readFFormat3(
    const char* str,
    int32_t* vi0,
    int32_t* vi1,
    int32_t* vi2,
    int32_t* ti0,
    int32_t* ti1,
    int32_t* ti2,
    int32_t* ni0,
    int32_t* ni1,
    int32_t* ni2)
{
    static_cast<void>(ti0);
    static_cast<void>(ti1);
    static_cast<void>(ti2);

    // "f 272//1 204//2 205//3"
    if (sscanf(
                str,
                "f %d//%d %d//%d %d//%d",
                vi0, ni0,
                vi1, ni1,
                vi2, ni2) != 6)
    {
        return false;
    }
    return true;
}

/*
-------------------------------------------------
readFFormat4()
f v1/t1 v2/t2 v3/t3 v4/t4 のフォーマット
-------------------------------------------------
*/
static bool readFFormat4(
    const char* str,
    int32_t* vi0,
    int32_t* vi1,
    int32_t* vi2,
    int32_t* ti0,
    int32_t* ti1,
    int32_t* ti2,
    int32_t* ni0,
    int32_t* ni1,
    int32_t* ni2)
{
    static_cast<void>(ni0);
    static_cast<void>(ni1);
    static_cast<void>(ni2);

    // "f 272//1 204//2 205//3"
    if (sscanf(
                str,
                "f %d/%d %d/%d %d/%d",
                vi0, ti0,
                vi1, ti1,
                vi2, ti2) != 6)
    {
        return false;
    }
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsVert(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(currentMaterialId);
    static_cast<void>(materialIds);
    // 頂点行
    if (strncmp(readBuffer, "v ", 2) == 0)
    {
        float x,y,z;
        if (sscanf(
                    readBuffer,
                    "v %f %f %f",
                    &x, &y, &z) != 3)
        {
            loggingError("invalild vertex format\n");
            return false;
        }
        Vec3 v(x,y,z);
        this_.vs.push_back(v);
        return true;
    }
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsFace(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(materialIds);
    //
    if (strncmp(readBuffer, "f ", 2) != 0)
    {
        return false;
    }
    //
    int32_t vertexIndex[3] = { 0, 0, 0 };
    int32_t normalIndex[3] = { 0, 0, 0 };
    int32_t uvIndex[3] = { 0, 0, 0 };
    //
    typedef std::function < bool(
        const char*,
        int32_t*, int32_t*, int32_t*,
        int32_t*, int32_t*, int32_t*,
        int32_t*, int32_t*, int32_t*
    ) > ReadFFormatFunc;
    static int32_t prevIndex = 0;
    const ReadFFormatFunc readFaceFuncs[4] =
    {
        readFFormat1,
        readFFormat2,
        readFFormat3,
        readFFormat4
    };
    if (readFaceFuncs[prevIndex](
                readBuffer,
                &vertexIndex[0],
                &vertexIndex[1],
                &vertexIndex[2],
                &uvIndex[0],
                &uvIndex[1],
                &uvIndex[2],
                &normalIndex[0],
                &normalIndex[1],
                &normalIndex[2]
            ))
    {
    }
    else
    {
        int32_t i = 0;
        for (i = 0; i < 4; ++i)
        {
            if (readFaceFuncs[i](
                        readBuffer,
                        &vertexIndex[0],
                        &vertexIndex[1],
                        &vertexIndex[2],
                        &uvIndex[0],
                        &uvIndex[1],
                        &uvIndex[2],
                        &normalIndex[0],
                        &normalIndex[1],
                        &normalIndex[2]
                    ))
            {
                prevIndex = i;
                break;
            }
        }
        loggingErrorIf(i == 4, "invalild index format");
    }
    /*
    objファイルのインデックスは次のような仕様を持つ
    - 正であった場合。インデックスは1から始まる。
    - 負であった場合。インデックスは(インデックスの対象の数-インデックス)が正しいインデックスになる。
    またreadFaceFuncs()は「存在しないインデックスは初期値のままにしている(0にしている)」
    これらを考慮しインデックスを使用可能な形式に変換する
    */
    const auto correctIndex = [](int32_t index, int32_t total) -> int32_t
    {
        if (index > 0)
        {
            return index - 1;
        }
        else if (index < 0)
        {
            return total + index;
        }
        else
        {
            return 0;
        }
    };
    const int32_t vn = int32_t(this_.vs.size());
    const int32_t tn = int32_t(this_.ts.size());
    const int32_t nn = int32_t(this_.ns.size());
    MeshFace meshFace;
    meshFace.vi[0] = correctIndex(vertexIndex[0], vn);
    meshFace.vi[1] = correctIndex(vertexIndex[1], vn);
    meshFace.vi[2] = correctIndex(vertexIndex[2], vn);
    meshFace.ti[0] = correctIndex(uvIndex[0], tn);
    meshFace.ti[1] = correctIndex(uvIndex[1], tn);
    meshFace.ti[2] = correctIndex(uvIndex[2], tn);
    meshFace.ni[0] = correctIndex(normalIndex[0], nn);
    meshFace.ni[1] = correctIndex(normalIndex[1], nn);
    meshFace.ni[2] = correctIndex(normalIndex[2], nn);
    meshFace.mi = currentMaterialId;
    this_.faces.emplace_back(meshFace);
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsEmptyLine(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(currentMaterialId);
    static_cast<void>(materialIds);
    //
    if (readBuffer[0] == '\0' || readBuffer[0] == '\n')
    {
        return true;
    }
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsComment(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(currentMaterialId);
    static_cast<void>(materialIds);
    //
    if (strncmp(readBuffer, "#", 1) == 0)
    {
        return true;
    }
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsVertexNormal(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    // 頂点法線は計算するのでメッシュデータのものは無視する
    static_cast<void>(currentMaterialId);
    static_cast<void>(materialIds);
    if (strncmp(readBuffer, "vn ", 3) == 0)
    {
        // vn x y z の形式
        float x,y,z;
        if (sscanf(readBuffer, "vn %f %f %f", &x, &y, &z) != 3)
        {
            loggingError("invalild vt format\n");
            return false;
        }
        Vec3 vn(x,y,z);
        this_.ns.push_back(vn);
        return true;
    }
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsUV(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(currentMaterialId);
    //static_cast<void>(materialMgr);
    //
    if (strncmp(readBuffer, "vt ", 3) != 0)
    {
        return false;
    }
    // vt u v の形式
    float u, v;
    if (sscanf(readBuffer, "vt %f %f", &u, &v) != 2)
    {
        loggingError("invalild vt format\n");
        return false;
    }
    //  UVが範囲から外れている場合はここで補正しておく
    if (u < 0.0f)
    {
        u = 1.0f - fmodf(fabsf(u), 1.0f);
    }
    if (1.0f < u)
    {
        u = fmodf(u, 1.0f);
    }
    if (v < 0.0f)
    {
        v = 1.0f - fmodf(fabsf(v), 1.0f);
    }
    if (1.0f < v)
    {
        v = fmodf(v, 1.0f);
    }

    // HACK: なぜかUVにすると正しくなる。後段で間違っているか、この段階で逆になっているのか不明
    //this_.ts.push_back(Vec2(u, v));
    this_.ts.push_back(Vec2(v, u));
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsGroup(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(currentMaterialId);
    static_cast<void>(materialIds);
    //
    return (strncmp(readBuffer, "g ", 2) == 0);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsSmoothing(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(currentMaterialId);
    static_cast<void>(materialIds);
    //
    return (strncmp(readBuffer, "s ", 2) == 0);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsMtlLib(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(currentMaterialId);
    static_cast<void>(materialIds);
    //
    return (strncmp(readBuffer, "mtllib", 5) == 0);
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsUseMTL(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(currentMaterialId);
    //
    if (strncmp(readBuffer, "usemtl ", 7) == 0)
    {
        //
        std::stringstream stream(readBuffer);
        std::string name;
        std::string tag("usemtl ");
        stream >> tag >> name;
        //
        const auto ite = materialIds.find(name);
        if (ite == materialIds.end())
        {
            currentMaterialId = static_cast<int32_t>(materialIds.size());
            materialIds.insert(std::make_pair(name,currentMaterialId));
        }
        else
        {
            currentMaterialId = ite->second;
        }
        return true;
    }
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool checkAsO(
    Mesh& this_,
    int32_t& currentMaterialId,
    MaterialIds& materialIds,
    const char* readBuffer)
{
    static_cast<void>(this_);
    static_cast<void>(currentMaterialId);
    static_cast<void>(materialIds);
    //
    if (strncmp(readBuffer, "o ", 2) == 0)
    {
        return true;
    }
    return false;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
bool Mesh::loadFromoObj(const std::string& fileName)
{
    logging("OBJ: start loading. [%s]", fileName.c_str());
    FILE* file = fopen(fileName.c_str(), "rt");
    if (!file)
    {
        loggingError("OBJ: can't load obj file [%s]\n", fileName.c_str());
        return false;
    }
    // usemtlがないobjファイルでもデフォルトマテリアルが割り当てられるように0にしておく
    int32_t currentMaterialId = 0;
    const int32_t readBufferSize = 0xFFF;
    char readBuffer[readBufferSize];
    while (fgets(readBuffer, readBufferSize, file))
    {
        typedef std::function<bool(Mesh&, int32_t&, MaterialIds&, const char*)> CheckLineFunc;
        const CheckLineFunc checkLineFuncs[] =
        {
            checkAsVert,
            checkAsFace,
            checkAsEmptyLine,
            checkAsComment,
            checkAsVertexNormal,
            checkAsUV,
            checkAsGroup,
            checkAsSmoothing,
            checkAsMtlLib,
            checkAsUseMTL,
            checkAsO,
        };
        static int32_t prevCheckFuncNo = 0;
        int32_t funcNo = 0;
        const int32_t NUM_FUNC = sizeof(checkLineFuncs) / sizeof(*checkLineFuncs);
        // 前回と同じフォーマットである可能性が高いのでまず同じフォーマットでチェック
        if (checkLineFuncs[prevCheckFuncNo](*this, currentMaterialId, materialIds_,readBuffer))
        {
        }
        // フォーマットが変わっている場合は全ての場合について再度チェック
        else
        {
            for (funcNo = 0; funcNo < NUM_FUNC; ++funcNo)
            {
                if (checkLineFuncs[funcNo](*this, currentMaterialId, materialIds_, readBuffer))
                {
                    //logging("OBJ: Change line format[%d]->[%d]", prevCheckFuncNo, funcNo);
                    prevCheckFuncNo = funcNo;
                    break;
                }
            }
            if (funcNo == NUM_FUNC)
            {
                loggingError("OBJ: invalid line [%s]\n", readBuffer);
                return false;
            }
        }
    }
    // UVが存在しないメッシュ用に末尾番にダミーを入れておく
    ts.push_back(Vec2(0.0f));

    // 法線がなければ生成する
    if (ns.empty())
    {
        recalcNormal();
    }

    //
    logging("OBJ: finish loading. [%s]", fileName.c_str());
    //
    return true;
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Mesh::applyTransform(const Transform& transform)
{
    for (auto& v : vs)
    {
        v = transform.toWorld(v);
    }
}

/*
-------------------------------------------------
recalcNormal()
cf. http://iquilezles.org/www/articles/normals/normals.htm
-------------------------------------------------
*/
void Mesh::recalcNormal()
{
    ns.resize(vs.size());
    for (auto& n : ns)
    {
        n.zero();
    }
    // 各面の法線を足してく
    for (const auto& f : faces)
    {
        const int32_t i0 = f.vi[0];
        const int32_t i1 = f.vi[1];
        const int32_t i2 = f.vi[2];
        const Vec3& v0 = vs[i0];
        const Vec3& v1 = vs[i1];
        const Vec3& v2 = vs[i2];
        const Vec3 e1 = v1 - v0;
        const Vec3 e2 = v2 - v0;
        const Vec3 n = Vec3::cross(e1, e2);
        ns[i0] += n;
        ns[i1] += n;
        ns[i2] += n;
    }
    // 正規化
    for (auto& n : ns)
    {
        n.normalize();
    }
}

/*
-------------------------------------------------
-------------------------------------------------
*/
void Mesh::recalcBound()
{
    aabb.clear();
    // 全頂点を追加していく
    for (const auto& v : vs)
    {
        aabb.addPoint(v);
    }
    //
    const Vec3& mn = aabb.min();
    const Vec3& mx = aabb.max();
    logging("AABB(%.2f,%.2f,%.2f)-(%.2f,%.2f,%.2f)", mn.x(), mn.y(), mn.z(), mx.x(), mx.y(), mx.z());
}

/*
-------------------------------------------------
-------------------------------------------------
*/
const MaterialIds& Mesh::materialIds() const
{
    return materialIds_;
}
