#include "database/Database.h"
#include <string>
#include <iostream>
#include <fstream>
#include "utils/Performance.h"

namespace Figaro
{
    ErrorCode Database::loadDatabaseRelationsSchema(const json& jsonRelInfos)
    {
        for (const auto& jsonRelInfo: jsonRelInfos)
        {
            Relation relation(jsonRelInfo);
            std::string relationName = relation.getName();
            m_relations.emplace(relationName, std::move(relation));
        }
        return ErrorCode::NO_ERROR;
    }

    ErrorCode Database::loadDatabaseSchema(const std::string& schemaConfigPath)
    {
        std::ifstream inputFileStream(schemaConfigPath);
        json jsonDbConfig;

        if (inputFileStream.fail())
        {
            FIGARO_LOG_ERROR("Database configuration path incorrect", schemaConfigPath);
            return ErrorCode::WRONG_PATH;
        }
        inputFileStream >> jsonDbConfig;
        FIGARO_LOG_INFO("Database Configuration", jsonDbConfig);

        loadDatabaseRelationsSchema(jsonDbConfig["database"]["relations"]);
        return ErrorCode::NO_ERROR;
    }

    std::tuple<std::string, std::string>
    Database::saveQRResult(
            std::tuple<Relation*, Relation*> qrResult)
    {
        auto [pR, pQ] = qrResult;
        std::string rName = "";
        std::string qName = "";
        Relation& matR = *pR;
        Relation& matQ = *pQ;
        if (pR != nullptr)
        {
            rName = pR->getName();
            m_relations.emplace(rName, std::move(matR));

        }
        if (pQ != nullptr)
        {
            qName = pQ->getName();
            m_relations.emplace(qName, std::move(matQ));
        }
        return std::make_tuple(rName, qName);
    }

    Database::Database(const std::string& schemaConfigPath)
    {
        initializationErrorCode = loadDatabaseSchema(schemaConfigPath);
    }

    ErrorCode Database::loadData(void)
    {
        for (auto& [relName, relation]: m_relations)
        {
            ErrorCode errorCode = relation.loadData();
            if (errorCode != ErrorCode::NO_ERROR)
            {
                return errorCode;
            }
        }
        return ErrorCode::NO_ERROR;
    }

    void Database::resetComputations(void)
    {
        for (auto& [relName, relation]: m_relations)
        {
            relation.resetComputations();
        }
    }

    void Database::sortData(void)
    {
        for (auto& [relName, relation]: m_relations)
        {
            relation.sortData();
        }
    }

    void Database::sortRelation(const std::string& relationName, const std::vector<std::string>&  vSortAttributeNames)
    {
        Relation& relation = m_relations.at(relationName);
        relation.sortData(vSortAttributeNames);
    }

    std::vector<std::string> Database::getRelationAttributeNames(const std::string& relationName)
    {
        auto& relation = m_relations.at(relationName);
        return relation.getAttributeNames();
    }

    void Database::dropAttributesFromRelations(
            const std::vector<std::string>& vDropAttrNames)
    {
        for (auto& [relName, relation]: m_relations)
        {
            relation.dropAttributes(vDropAttrNames);
        }
    }

    void Database::updateSchemaOfRelation(
        const std::string& relationName,
        const std::vector<std::string>& vAttrNames)
    {
        auto& relation = m_relations.at(relationName);
        return relation.updateSchema(vAttrNames);
    }

    void Database::oneHotEncodeRelations(void)
    {
        for (auto& [relName, relation]: m_relations)
        {
            FIGARO_LOG_INFO("One hot encoding relation", relName)
            relation.oneHotEncode();
        }
    }

    void Database::renameRelation(const std::string& oldRelationName,
            const std::string& newRelationName)
    {
        Relation& rel = m_relations.at(oldRelationName);
        rel.renameRelation(newRelationName);
        auto nodeHandler = m_relations.extract(oldRelationName);
        nodeHandler.key() = newRelationName;
        m_relations.insert(std::move(nodeHandler));
    }

    void Database::persistRelation(const std::string& relationName)
    {
        Relation& rel = m_relations.at(relationName);
        rel.persist();
    }

    std::string Database::joinRelations(const std::string& relationName,
            const std::vector<std::string>& vChildRelNames,
            const std::vector<std::string>& vJoinAttrNames,
            const std::vector<std::string>& vParJoinAttrNames,
            const std::vector<std::vector<std::string> >& vvJoinAttributeNames,
            bool trackProvenance)
    {
        std::vector<Relation*> vpChildRels;
        Relation& rel = m_relations.at(relationName);
        FIGARO_LOG_INFO("Relation names", vChildRelNames)
        for (const auto childRelName: vChildRelNames)
        {
            Relation* pRel = &m_relations.at(childRelName);
            vpChildRels.push_back(pRel);
        }

        Relation relJoin = rel.joinRelations(
            vpChildRels, vJoinAttrNames,
            vParJoinAttrNames,  vvJoinAttributeNames, trackProvenance);
        std::string relJoinName = relJoin.getName();
        m_relations.emplace(relJoin.getName(), std::move(relJoin));;
        FIGARO_LOG_INFO("New rel name", relJoin.getName())
        return relJoinName;
    }


    void Database::computeDownCounts(
        const std::string& relationName,
        const std::vector<std::string>& vChildRelNames,
        const std::vector<std::string>& vJoinAttrNames,
        const std::vector<std::string>& vParJoinAttrNames,
        const std::vector<std::vector<std::string> >& vvJoinAttributeNames,
        bool isRootNode)
    {
        std::vector<Relation*> vpChildRels;
        Relation& rel = m_relations.at(relationName);

        for (const auto childRelName: vChildRelNames)
        {
            Relation* pRel = &m_relations.at(childRelName);
            vpChildRels.push_back(pRel);
        }
        rel.computeDownCounts(vpChildRels, vJoinAttrNames, vParJoinAttrNames,
            vvJoinAttributeNames, isRootNode);
    }


    void Database::computeUpAndCircleCounts(
        const std::string& relationName,
        const std::vector<std::string>& vChildRelNames,
        const std::vector<std::string>& vParJoinAttrNames,
        const std::vector<std::vector<std::string> >& vvJoinAttributeNames,
        bool isRootNode)
    {
        std::vector<Relation*> vpChildRels;
        Relation& rel = m_relations.at(relationName);

        for (const auto childRelName: vChildRelNames)
        {
            Relation* pRel = &m_relations.at(childRelName);
            vpChildRels.push_back(pRel);
        }
        rel.computeUpAndCircleCounts(vpChildRels, vParJoinAttrNames,
            vvJoinAttributeNames, isRootNode);
    }

    std::map<std::vector<uint32_t>, uint32_t> Database::getDownCounts(
        const std::string& relationName)
    {
        Relation& rel = m_relations.at(relationName);
        return rel.getDownCounts();
    }


    std::map<std::vector<uint32_t>, uint32_t> Database::getParDownCnts(
            const std::string& relationName,
            const std::vector<std::string>& vParJoinAttrNames)
    {
        Relation& rel = m_relations.at(relationName);
        return rel.getParDownCntsFromHashTable(vParJoinAttrNames);
    }

    std::map<std::vector<uint32_t>, uint32_t> Database::getParUpCnts(
            const std::string& relationName,
            const std::vector<std::string>& vParJoinAttrNames)
    {
        Relation& rel = m_relations.at(relationName);
        return rel.getParUpCntsFromHashTable(vParJoinAttrNames);
    }

    std::map<std::vector<uint32_t>, uint32_t> Database::getCircCounts(
        const std::string& relationName)
    {
        Relation& rel = m_relations.at(relationName);
        return rel.getCircCounts();
    }

    std::tuple<std::string, std::string> Database::computeHeadsAndTails(
        const std::string& relationName,
        const std::vector<std::string>& vJoinAttrNames,
        bool isLeafNode)
    {
        Relation& rel = m_relations.at(relationName);
        auto [relHeads, relTails] = rel.computeHeadsAndTails(vJoinAttrNames, isLeafNode);
        auto headsName = relHeads.getName();
        auto tailName = relTails.getName();
        m_relations.emplace(relHeads.getName(), std::move(relHeads));
        m_relations.emplace(relTails.getName(), std::move(relTails));
        return {headsName, tailName};
    }

    std::string Database::aggregateAwayChildrenRelations(
            const std::string& relationName,
            const std::string& relHeadName,
            const std::vector<std::string>& vChildRelNames,
            const std::vector<std::string>& vChildHeadRelNames,
            const std::vector<std::string>& vJoinAttributeNames,
            const std::vector<std::vector<std::string> >& vvJoinAttributeNames)
    {
        Relation& rel = m_relations.at(relationName);
        Relation& relHead = m_relations.at(relHeadName);
        std::vector<Relation*> vpChildRels;
        std::vector<Relation*> vpChildHeadRels;

        for (const auto childRelName: vChildRelNames)
        {
            Relation* pRel = &m_relations.at(childRelName);
            vpChildRels.push_back(pRel);
        }
        FIGARO_LOG_INFO(vChildHeadRelNames);
        for (const auto childHeadRelName: vChildHeadRelNames)
        {
            Relation* pRel = &m_relations.at(childHeadRelName);
            vpChildHeadRels.push_back(pRel);
        }
        FIGARO_LOG_INFO(vChildRelNames);

        Relation relAggAway = rel.aggregateAwayChildrenRelations(&relHead, vpChildRels,
            vpChildHeadRels, vJoinAttributeNames, vvJoinAttributeNames);
        std::string aggregatedAwayName = relAggAway.getName();
        m_relations.emplace(aggregatedAwayName, std::move(relAggAway));
        return aggregatedAwayName;
    }

    std::tuple<std::string, std::string>
    Database::computeAndScaleGeneralizedHeadAndTail(
        const std::string& relationName,
        const std::string& aggrAwayRelName,
        const std::vector<std::string>& vJoinAttributeNames,
        const std::vector<std::string>& vParJoinAttributeNames,
        bool isRootNode)
    {
        Relation& rel = m_relations.at(relationName);
        Relation& aggAwayRel = m_relations.at(aggrAwayRelName);
        auto [genHeadRel, genTailRel] =
        rel.computeAndScaleGeneralizedHeadAndTail(
            &aggAwayRel,
            vJoinAttributeNames, vParJoinAttributeNames,
            isRootNode);
        std::string genHeadRelname = genHeadRel.getName();
        std::string genTailRelname = genTailRel.getName();

        m_relations.emplace(genHeadRelname, std::move(genHeadRel));
        m_relations.emplace(genTailRelname, std::move(genTailRel));

        return {genHeadRelname, genTailRelname};
    }

    std::tuple<std::string, std::string>
    Database::computePostprocessing(
            const std::vector<std::string>& vRelationOrder,
            const std::string& genHeadRoot,
            const std::vector<std::string>& vTailRels,
            const std::vector<std::string>& vGenTailRels,
            Figaro::QRGivensHintType qrHintType,
            bool saveResult,
            const std::string& joinRelName)
    {
        std::vector<Relation*> vpRels;
        std::vector<Relation*> vpRelTails;
        std::vector<Relation*> vpRelGenTails;
        Relation* pRootRel;
        Relation* pJoinRel = nullptr;
        for (const auto relName: vRelationOrder)
        {
            Relation* pRel = &m_relations.at(relName);
            vpRels.push_back(pRel);
        }
        for (const auto relName: vTailRels)
        {
            Relation* pRel = &m_relations.at(relName);
            FIGARO_LOG_ASSERT(pRel != nullptr)
            vpRelTails.push_back(pRel);
            pRel->computeQRInPlace(qrHintType);
        }

        for (const auto relName: vGenTailRels)
        {
            Relation* pRel = &m_relations.at(relName);
            FIGARO_LOG_ASSERT(pRel != nullptr)
            vpRelGenTails.push_back(pRel);
            pRel->computeQRInPlace(qrHintType);
        }

        pRootRel = &m_relations.at(genHeadRoot);
        pRootRel->computeQROfGeneralizedHead(vpRelTails, qrHintType);

        if (joinRelName != "")
        {
            pJoinRel = &m_relations.at(joinRelName);
        }
        auto qrResult = pRootRel->computeQROfConcatenatedGeneralizedHeadAndTails(
            vpRels,
            pRootRel,
            vpRelTails, vpRelGenTails,
            qrHintType, saveResult, pJoinRel);
        return saveQRResult(qrResult);
    }

    void Database::changeMemoryLayout(void)
    {
        for (auto& [relName, relation]: m_relations)
        {
            FIGARO_LOG_INFO("One hot encoding relation", relName)
            relation.changeMemoryLayout(MemoryLayout::COL_MAJOR);
        }
    }

    std::tuple<std::string, std::string>
    Database::evalPostprocessing(
            const std::string& relName,
            Figaro::QRGivensHintType qrHintType,
            Figaro::MemoryLayout memoryLayout,
            bool computeQ,
            bool saveResult)
    {
        Relation& rel = m_relations.at(relName);
        auto qrResult = rel.computeQR(qrHintType, memoryLayout, computeQ, saveResult);
        return saveQRResult(qrResult);
    }

    bool Database::destroyTemporaryRelation(const std::string& relationName)
    {
        Relation& rel = m_relations.at(relationName);
        if (rel.isTmp())
        {
            m_relations.erase(relationName);
            return true;
        }
        return false;
    }

    std::string Database::createDummyGenTailRelation(const std::string& relationName)
    {
        Relation& rel = m_relations.at(relationName);
        Relation dummyGenTail = rel.createDummyGenTailRelation();
        std::string dummRelName = dummyGenTail.getName();
        m_relations.emplace(dummRelName, std::move(dummyGenTail));
        return dummRelName;
    }

    void Database::outputRelationToFile(std::ostream& out, const std::string& relationName,
        char sep, uint32_t precision, bool header)
    {
        const Relation& rel = m_relations.at(relationName);
        rel.outputToFile(out, sep, precision, header);
    }

    const Relation::MatrixDT& Database::getHead(const std::string& relationName) const
    {
        const Relation& rel = m_relations.at(relationName);
        return rel.getHead();
    }

    const Relation::MatrixDT& Database::getTail(const std::string& relationName) const
    {
        const Relation& rel = m_relations.at(relationName);
        return rel.getTail();
    }

    const Relation::MatrixDT& Database::getGeneralizedTail(const std::string& relationName) const
    {
        const Relation& rel = m_relations.at(relationName);
        return rel.getGeneralizedTail();
    }

    const Relation::MatrixDT& Database::getScales(const std::string& relationName) const
    {
        const Relation& rel = m_relations.at(relationName);
        return rel.getScales();
    }

    const Relation::MatrixDT& Database::getDataScales(const std::string& relationName) const
    {
        const Relation& rel = m_relations.at(relationName);
        return rel.getDataScales();
    }
}

