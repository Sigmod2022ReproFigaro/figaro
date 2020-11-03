#ifndef _FIGARO_DATABASE_H_
#define _FIGARO_DATABASE_H_

#include "utils/Utils.h"
#include "Relation.h"

namespace Figaro
{
    class Database
    {
        std::map<std::string, Relation> m_relations; 
        ErrorCode initializationErrorCode = ErrorCode::NO_ERROR;

        ErrorCode loadDatabaseRelationsSchema(const json& jsonRelInfos);

        ErrorCode loadDatabaseSchema(const std::string& schemaConfigPath); 

       
    public:
        Database(const std::string& schemaConfigPath); 

        ErrorCode getInitializationErrorCode(void) const
         { return initializationErrorCode; }
        
        /**
         * Reads the data for each relation specified by the corresponding paths
         * and stores in the corresponding members of relations. 
         */
        ErrorCode loadData(void);


        /**
         * Sorts the data of each relation from the database.
         */
        void sortData(void);

        void sortRelation(const std::string& relationName, const std::vector<std::string> vSortAttributeNames);

        void joinRelations(std::vector<std::string> vRelationNames,
        const std::vector<std::tuple<std::string, std::string> >& vJoinAttributeNames);
        
        MatrixT* computeHead(const std::string& relationName);

        MatrixT* computeHead(const std::string& relName, const std::string& attrName);

        MatrixT* computeTail(const std::string& relationName);

        void computeScaledCartesianProduct(std::array<std::string, 2> relationNames,
            std::array<Eigen::VectorXd, 2> vectors);

        
    };
}
#endif 