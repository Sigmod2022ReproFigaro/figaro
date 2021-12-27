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

        std::tuple<std::string, std::string> saveQRResult(
            std::tuple<Relation*, Relation*> qrResult);

    public:
        Database(const std::string& schemaConfigPath);

        ErrorCode getInitializationErrorCode(void) const
         { return initializationErrorCode; }

        /**
         * Reads the data for each relation specified by the corresponding paths
         * and stores in the corresponding members of relations.
         */
        ErrorCode loadData(void);

        void resetComputations(void);

        /**
         * Sorts the data of each relation from the database.
         */
        void sortData(void);

        void sortRelation(const std::string& relationName, const std::vector<std::string>& vSortAttributeNames);

        std::vector<std::string> getRelationAttributeNames(const std::string& relationName);

        void dropAttributesFromRelations(
            const std::vector<std::string>& vDropAttrNames);

        void updateSchemaOfRelation(
            const std::string& relationName,
            const std::vector<std::string>& vAttrNames);

        void oneHotEncodeRelations(void);

        void renameRelation(
            const std::string& oldRelationName,
            const std::string& newRelationName);

        void persistRelation(const std::string& relationName);

        /**
         * @brief Join relation @p relationName with the children relations
         * specified in @p vChildRelNames on join attributes specified in @p vJoinAttrnames and @p vvJoinAttributeNames .
         * @p vParJoinAttrNames specifies which join attributes should remain after the join has finished.
         *
         * @return std::string a name of a tmp rotation that contains the join result.
         */
        std::string joinRelations(const std::string& relationName,
            const std::vector<std::string>& vChildRelNames,
            const std::vector<std::string>& vJoinAttrNames,
            const std::vector<std::string>& vParJoinAttrNames,
            const std::vector<std::vector<std::string> >& vvJoinAttributeNames,
            bool trackProvenance);

        void computeDownCounts(
            const std::string& relationName,
            const std::vector<std::string>& vChildRelNames,
            const std::vector<std::string>& vJoinAttrNames,
            const std::vector<std::string>& vParJoinAttrNames,
            const std::vector<std::vector<std::string> >& vvJoinAttributeNames,
            bool isRootNode);

        void computeUpAndCircleCounts(
            const std::string& relationName,
            const std::vector<std::string>& vChildRelNames,
            const std::vector<std::string>& vParJoinAttrNames,
            const std::vector<std::vector<std::string> >& vvJoinAttributeNames,
            bool isRootNode);

        std::tuple<std::string, std::string> computeHeadsAndTails(
            const std::string& relationName,
            const std::vector<std::string>& vJoinAttrNames,
            bool isLeafNode);

        std::string aggregateAwayChildrenRelations(
            const std::string& relationName,
            const std::string& relHeadName,
            const std::vector<std::string>& vChildRelNames,
            const std::vector<std::string>& vChildHeadRelNames,
            const std::vector<std::string>& vJoinAttributeNames,
            const std::vector<std::vector<std::string> >& vvJoinAttributeNames,
            const std::vector<std::string>& vSubTreeRelNames,
            const std::vector<std::vector<std::string> >& vvSubTreeRelnames);

        std::tuple<std::string, std::string>
        computeAndScaleGeneralizedHeadAndTail(
            const std::string& relationName,
            const std::string& aggrAwayRelName,
            const std::vector<std::string>& vJoinAttributeNames,
            const std::vector<std::string>& vParJoinAttributeNames,
            bool isRootNode,
            uint32_t numRelsSubTree);

        std::tuple<std::string, std::string>
        computePostprocessing(
            const std::vector<std::string>& vRelationOrder,
            const std::string& genHeadRoot,
            const std::vector<std::string>& vTailRels,
            const std::vector<std::string>& vGenTailRels,
            Figaro::QRGivensHintType qrHintType,
            bool saveResult,
            const std::string& joinRelName);

        void changeMemoryLayout(void);

        std::tuple<std::string, std::string> evalPostprocessing(
            const std::string& relName,
            Figaro::QRGivensHintType qrHintType,
            Figaro::MemoryLayout memoryLayout,
            bool computeQ,
            bool saveResult);

        /**
         * @brief Destroys temporary relation @p relName with its data.
         *
         * @param relName
         * @return true In a case when relation alongside data is removed from a database.
         * @return false In a case when relation is not temporary and thus, not removed.
         */
        bool destroyTemporaryRelation(const std::string& relationName);

        std::string createDummyGenTailRelation(const std::string& relName);

        void outputRelationToFile(std::ostream& out, const std::string& relationName,
            char delimiter, uint32_t precision, bool header = false);

        /********** Getters  used for testing **********/

        std::map<std::vector<uint32_t>, uint32_t> getDownCounts(const std::string& relationName);

        std::map<std::vector<uint32_t>, uint32_t> getParDownCnts(
            const std::string& relationName,
            const std::vector<std::string>& vParJoinAttrNames);

        std::map<std::vector<uint32_t>, uint32_t> getParUpCnts(
            const std::string& relationName,
            const std::vector<std::string>& vParJoinAttrNames);

        std::map<std::vector<uint32_t>, uint32_t> getCircCounts(const std::string& relationName);

        const Relation::MatrixDT& getHead(const std::string& relationName) const;

        const Relation::MatrixDT& getTail(const std::string& relationName) const;

        const Relation::MatrixDT& getGeneralizedTail(const std::string& relationName) const;

        const Relation::MatrixDT& getScales(const std::string& relationName) const;

        const Relation::MatrixDT& getDataScales(const std::string& relationName) const;
    };
}
#endif