#include "database/query/ASTQRVisitor.h"

namespace Figaro
{
    std::string ASTQRVisitor::getFormateJoinAttributeNames(
        const std::vector<std::string>& vJoinAttributeNames)
    {
        std::string formatedStr = "";

        for (uint32_t idx = 0; idx < vJoinAttributeNames.size(); idx++)
        {
            if (idx > 0)
            {
                formatedStr += ",";
            }
            formatedStr += vJoinAttributeNames[idx];
        }
        return formatedStr;
    }

}