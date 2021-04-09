#include "utils/Utils.h"

#include <iostream>
#include <fstream>
#include <iomanip>

namespace Figaro
{
    uint32_t getNumberOfLines(const std::string& filePath)
    {
        std::string line;
        std::ifstream file(filePath);
        uint32_t cntLines = 0;
        if (file.fail())
        {
            FIGARO_LOG_ERROR("Path to the file is wrong:", filePath);
            return 0;
        }

        while (std::getline(file, line))
        {
            cntLines ++;
        }
        return cntLines;
    }

    std::vector<std::string> setIntersection(
        const std::vector<std::string>& vStr1,
        const std::vector<std::string>& vStr2)
    {
        std::map<std::string, bool> sStrAppears;
        std::vector<std::string> vIntersection;
        //FIGARO_LOG_DBG("set1", vStr1, "set2", vStr2)
        for (const auto& str: vStr1)
        {
            sStrAppears[str] = false;
        }
        for (const auto& str: vStr2)
        {
            if (sStrAppears.find(str) != sStrAppears.end())
            {
                sStrAppears[str] = true;
            }
        }
        for (const auto&[key, exists]: sStrAppears)
        {
            if (exists)
            {
                vIntersection.push_back(key);
            }
        }
        return vIntersection;
    }
}

std::ostream& Figaro::outputMatrixTToCSV(std::ostream& out,
    const Figaro::MatrixEigenT& matrix, char sep, uint32_t precision)
{
    for (uint32_t row = 0; row < matrix.rows(); row ++)
    {
        for (uint32_t col = 0; col < matrix.cols(); col++)
        {

            out << std::setprecision(precision) << std::fixed << matrix(row, col);
            if (col != (matrix.cols() - 1))
            {
                out << sep;
            }
        }
        out << std::endl;
    }
    return out;
}


std::ostream& operator<<(std::ostream& out, const Figaro::MatrixEigenT& matrix)
{
    out << "Matrix Eigen" << std::endl;
    out <<  "Matrix dimensions: " << matrix.rows() << " " << matrix.cols() << std::endl;
    return Figaro::outputMatrixTToCSV(out, matrix);
}