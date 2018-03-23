#include "pch.hpp"
#include "core/util.hpp"
#include "util/mathmaticautil.hpp"

/*
-------------------------------------------------
-------------------------------------------------
*/
void MathmaticaUtil::writePlotPoint2D(const std::string& fileName,
                                      const std::vector<Vec2>& points)
{
    const std::string fullPath = g_fileSystem.getOutputFolderPath() + fileName;
    //
    std::ofstream file;
    file.open(fullPath.c_str(), std::ios_base::out);
    if (!file)
    {
        return;
    }
    //
    file << "A = {" << std::endl;
    for (int32_t pi = 0; pi < points.size(); ++pi)
    {
        Vec2 v = points[pi];
        file << "{" + std::to_string(v.x()) + "," + std::to_string(v.y()) + "}";
        if (pi != (points.size() - 1))
        {
            file << ",";
        }
        file << std::endl;
    }
    file << "};" << std::endl;
    file << "ListPlot[A]";
}
/*
-------------------------------------------------
-------------------------------------------------
*/
void MathmaticaUtil::writePlotPoint3D(const std::string& fileName,
                                      const std::vector<Vec3>& points)
{
    const std::string fullPath = g_fileSystem.getOutputFolderPath() + fileName;
    //
    std::ofstream file;
    file.open(fullPath.c_str(), std::ios_base::out);
    if (!file)
    {
        return;
    }
    //
    file << "A = {" << std::endl;
    for (int32_t pi = 0; pi < points.size(); ++pi)
    {
        Vec3 v = points[pi];
        file << "{" + std::to_string(v.x()) + "," + std::to_string(v.y()) +
                    "," + std::to_string(v.z()) + "}";
        if (pi != (points.size() - 1))
        {
            file << ",";
        }
        file << std::endl;
    }
    file << "};" << std::endl;
    file << "ListPointPlot3D[A]";
}
