#ifndef _FIGARO_RELATION_H_
#define _FIGARO_RELATION_H_

#include "utils/Utils.h"
#include "database/storage/Matrix.h"
#include <vector>
#include <unordered_map>

// TODO: Optimize tuple instanciated Relation based on number of attributes.
namespace Figaro
{
    /**
     * @class Relation
     *
     * @brief We prevent attributes with the same name. The order of attributes
     * represent the order in which they are stored in the corresponding
     * csv file. In the constructor, the relation schema is initalized from
     * json object. The data is not loaded until requested
     * with function @see loadData.
     */
    class Relation
    {
        static constexpr char DELIMITER = ',';

        static bool compareTuples(const double pTuple1[], const double pTuple2[],
                const std::vector<uint32_t>& vAttrIdxs)
        {
            for (const auto& attrIdx: vAttrIdxs)
            {
                if (pTuple1[attrIdx] != pTuple2[attrIdx])
                {
                    return true;
                }
            }
            return false;
        }
    public:
        // By default we will map strings to int
        enum class AttributeType
        {
            FLOAT, INTEGER, CATEGORY
        };
        // key: PK values -> value: corresponding aggregate
        typedef std::map<std::vector<double>, double> GroupByT;
        typedef Figaro::Matrix<double> MatrixDT;

        /**
         * 1) Allocates matrix of type MatrixEigen @p matEig in memory.
         * 2) Copies the content of @p matDT to @p matEig.
         */
        static void copyMatrixDTToMatrixEigen(const MatrixDT& matDT, MatrixEigenT& matEig);

        /**
         * @struct Attribute
         *
         * This structure containts metadata about the attribute.
         * Metadata includes name, type, if the attribute is part of
         * primary key.
         */
        struct Attribute
        {
            std::string m_name = "";
            AttributeType m_type = AttributeType::FLOAT;
            bool m_isPrimaryKey = false;
            const std::map<AttributeType, std::string> mapTypeToStr =
            {
                std::make_pair(AttributeType::INTEGER, "int"),
                std::make_pair(AttributeType::FLOAT, "float"),
                std::make_pair(AttributeType::FLOAT, "double"),
                std::make_pair(AttributeType::CATEGORY, "category")
            };
            Attribute(const json& jsonAttributeInfo)
            {
                std::string strType;
                const static std::map<std::string, AttributeType> mapStrTypeToType =
                {
                    std::make_pair("int", AttributeType::INTEGER),
                    std::make_pair("float", AttributeType::FLOAT),
                    std::make_pair("double", AttributeType::FLOAT),
                    std::make_pair("category", AttributeType::CATEGORY)
                };
                m_name = jsonAttributeInfo["name"];
                strType = jsonAttributeInfo["type"];
                m_type = mapStrTypeToType.at(strType);
            }

            Attribute& operator=(const Attribute& other)
            {
                if (this != &other)
                {
                    m_name = other.m_name;
                    m_type = other.m_type;
                    m_isPrimaryKey = m_isPrimaryKey;
                }
                return *this;
            }

            friend void swap(Attribute& attr1, Attribute& attr2)
            {
                std::swap(attr1.m_name, attr2.m_name);
                std::swap(attr1.m_type, attr2.m_type);
                std::swap(attr1.m_isPrimaryKey, attr2.m_isPrimaryKey);
            }
        };
    private:
        std::string m_name;
        ErrorCode initializationErrorCode = ErrorCode::NO_ERROR;
        std::vector<Attribute> m_attributes;
        std::string m_dataPath;

        MatrixDT m_data;

        MatrixDT m_dataHead;
        MatrixDT m_dataTails;
        MatrixDT m_dataTailsGen;
        MatrixDT m_dataTails1;
        MatrixDT m_dataTails2;

        MatrixDT m_scales;
        MatrixDT m_dataScales;
        std::vector<double> m_allScales;
        /**
         * Contains the names of all relations in the subtree rooted
         * at this relation including this relation. The order of relations
         * is preorder.
         */
        std::vector<std::string> m_vSubTreeRelNames;
        /**
         * Contains the data ofsets of all relations in the subtree rooted
         * at this relation including this relation. The order of relations
         * is preorder.
         */
        std::vector<uint32_t> m_vSubTreeDataOffsets;

        MatrixDT m_countsJoinAttrs;
        MatrixDT m_countsParJoinAttrs;
        std::vector<uint32_t> m_vParBlockStartIdxs;

        // TODO: Delete this in children nodes since it is not needed.
        void* m_pHTParCounts;

        uint32_t m_cntsParIdxD;
        uint32_t m_cntsParIdxU;
        uint32_t m_cntsJoinIdxD;
        uint32_t m_cntsJoinIdxP;
        uint32_t m_cntsJoinIdxV;
        uint32_t m_cntsJoinIdxC;

        uint32_t getAttributeIdx(const std::string& attributeName) const;

        /**
         * For each attribute denoted by name stored in vector @p vAttributeNames, return
         * the corresponding column index stored in @p vAttributeIdx.
         * The computed  order of indices is the same as the order of attribute names
         * provided by @p vAttributeNames.
         */
        void getAttributesIdxs(
            const std::vector<std::string>& vAttributeNames,
            std::vector<uint32_t>& vAttributeIdxs) const;

        void getAttributesIdxsComplement(const std::vector<uint32_t>& vAttributeIdxs,
            std::vector<uint32_t>& vAttributesCompIdxs) const;

        uint32_t getNumberOfPKAttributes(void) const;

        uint32_t getNumberOfNonPKAttributes(void) const;

        uint32_t getNumberOfAttributes(void) const
        {
            return m_attributes.size();
        }

        /**
         * For each part of a composite PK compute the corresponding column index. The
         * order of returned indices is the same as specified initially by the
         * relational schema.
         */
        void getPKAttributeNames(
            std::vector<std::string>& vAttributeNamesPKs) const;

        void getNonPKAttributeNames(
            std::vector<std::string>& vAttributeNamesNonPKs) const;

        void getPKAttributeIndices(std::vector<uint32_t>& vPkAttrIdxs) const;

        /**
         * Returns a vector of indices of the attributes that are not part
         * of composite primary key for this relation. Indexing of attributes
         * starts from 0.
         */
        void getNonPKAttributeIdxs(std::vector<uint32_t>& vNonPkAttrIdxs) const;

        void schemaJoin(const Relation& relation, bool swapAttributes = false);

        /**
         *  Updates schema of the current relation such that
         * non-join attributes of @p vpChildRels are appended to the end.
         * @p m_vSubTreeRelNames and @p m_vSubTreeDataOffsets are also
         * updated accordingly.
         */
        void schemaJoins(
            const std::vector<Relation*>& vpChildRels,
            const std::vector<std::vector<uint32_t> >& vvJoinAttrIdxs,
            const std::vector<std::vector<uint32_t> >& vvNonJoinAttrIdxs);


        void schemaRemoveNonParJoinAttrs(
            const std::vector<uint32_t>& vJoinAttrIdxs,
            const std::vector<uint32_t>& vParJoinAttrIdxs);

        /**
         *  Builds hash index where key is @p vJoinAttrIdx over the @p data
         * returns it as a pointer @p pHashTablePt.
         * @note Destructor needs to be called after the usage.
         */
        void getHashTableRowIdxs(
            const std::vector<uint32_t>& vParJoinAttrIdxs,
            void*& pHashTablePt,
            const MatrixDT& data);

        /**
         * Looks up in the hash table @p hashTabRowPt for join attributes from parent
         * @p vParJoinAttrIdxs and the value specified in @p dataParent [ @p rowIdx ]
         */
        uint32_t getChildRowIdx(uint32_t rowIdx,
            const std::vector<uint32_t>& vParJoinAttrIdxs,
            void*  hashTabRowPt,
            const MatrixDT& dataParent);

        void destroyHashTableRowIdxs(
            const std::vector<uint32_t>& vParJoinAttrIdxs,
            void*& pHashTablePt);


        void initHashTable(const std::vector<uint32_t>& vParAttrIdx,
            void*& pHashTablePt,
            uint32_t hashTableSize);

        void insertParDownCntFromHashTable(
            const std::vector<uint32_t>& vParAttrIdx,
            void*& pHashTablePt,
            const double* pRow,
            uint32_t downCnt);


        std::tuple<uint32_t, uint32_t>& getParCntFromHashTable(
            const std::vector<uint32_t>& vParJoinAttrIdxs,
            void*  htChildRowIdx,
            const double* pRow);

        uint32_t getDistinctValuesCount(const std::string& attributeName) const;

        void getAttributeDistinctValues(const std::string& attributeName,
                std::vector<double>& vDistinctValues) const;

        void getAttributeDistinctValues(const std::vector<uint32_t>& vAttrIdxs,
                std::vector<double>& vDistinctValues) const;

        void getRowPtrs(
            const std::string& attrName,
            std::unordered_map<double, const double*>& htRowPts) const;

        void getDistinctVals(
            const std::vector<uint32_t>& vAttrIdxs,
            const std::vector<uint32_t>& vParAttrIdxs,
            MatrixDT& cntDiffVals,
            std::vector<uint32_t>& vRowIdxParDiffVals);

        void getDistinctValuesRowPositions(const std::string& attributeName,
            std::vector<uint32_t>& vDistinctValuesRowPositions,
            bool preallocated = true) const;

        void getDistinctValuesRowPositions(
            const std::vector<uint32_t>& vAttrIdxs,
            std::vector<uint32_t>& vDistinctValuesRowPositions,
            const MatrixDT& data,
            bool preallocated = true) const;

        static void makeDiagonalElementsPositiveInR(MatrixEigenT& matR);
    public:
        Relation(const Relation&) = delete;
        Relation(Relation&& ) = default;
        Relation(json jsonRelationSchema);

        const std::vector<Attribute>& getAttributes(void) const
        {
            return m_attributes;
        }

        std::vector<std::string> getAttributeNames(void) const;

        const std::string& getName(void) const { return m_name; }


        uint32_t numberOfAttributes() const
        {
            return m_attributes.size();
        }

        const std::string& getAttributeName(uint32_t attributedIdx) const
        {
            return m_attributes.at(attributedIdx).m_name;
        }

        /**
         * Fills the table data from the file path specified by @p filePath .
         * The data should be formated in CSV format where the separator is |
         * and where the number of attribues is the same as in schema. The
         * attribute types of lodaded data need to agree with the relational
         * schema in this class.
         */
        ErrorCode loadData(void);

        /**
         * Sorts the data stored in @p m_data in ascending
         * order of PKs. For now, we assume PKs are leading attributes in
         * the relation schema.
         */
        void sortData(void);

        void sortData(const std::vector<std::string>& vAttributeNames);

        void computeDownCounts(
            const std::vector<Relation*>& vpChildRels,
            const std::vector<std::string>& vJoinAttrNames,
            const std::vector<std::string>& vParJoinAttrNames,
            const std::vector<std::vector<std::string> >& vvJoinAttributeNames,
            bool isRootNode);


        void computeUpAndCircleCounts(
            const std::vector<Relation*>& vpChildRels,
            const std::vector<std::string>& vParJoinAttrNames,
            const std::vector<std::vector<std::string> >& vvJoinAttributeNames,
            bool isRoot = false);

        /*********************** Testing getters for counts ***************/
        std::map<std::vector<double>, uint32_t> getDownCounts(void);

        std::map<std::vector<double>, uint32_t> getParDownCntsFromHashTable(
        const std::vector<std::string>& vParJoinAttrNames);

        std::map<std::vector<double>, uint32_t> getParUpCntsFromHashTable(
        const std::vector<std::string>& vParJoinAttrNames);

        std::map<std::vector<double>, uint32_t> getCircCounts(void);


        void destroyParCntHashTable(const std::vector<uint32_t>& vParJoinAttrIdxs,
            void*  htChildParAttrs);
        /**
         * It will copy the underlying data and apply head transformation onto it.
         * The Head and Tail transformation will be applied as if we had:
         * SELECT attrNames[0], attrNames[1], ... attrNames[n]
         *  HEAD(remaining attributes),
         *  TAIL(remaining attributes)
         * GROUP BY attrNames.
         */
        void computeHeadsAndTails(
            const std::vector<std::string>& vJoinAttrNames, bool isLeafNode);

        /**
         *  It will join relations by copying data from the children relations @p vpChildRels
         *  in the join tree to the current head data. The join attributes that are
         *  not in the relation ( @p vParJoinAttributeNames ) that is in the parent node
         *  will be omitted. We assume @p vParJoinAttributeNames is a subset of
         *  @p vJoinAttributeNames and each @p vvJoinAttributeNames[i] is a subset of
         *  @p vJoinAttributeNames .
         */
        void aggregateAwayChildrenRelations(
            const std::vector<Relation*>& vpChildRels,
            const std::vector<std::string>& vJoinAttributeNames,
            const std::vector<std::vector<std::string> >& vvJoinAttributeNames);

        void computeAndScaleGeneralizedHeadAndTail(
            const std::vector<std::string>& vJoinAttributeNames,
            const std::vector<std::string>& vParJoinAttributeNames,
            bool isRootNode);

        void computeQROfGeneralizedHead(uint32_t numNonJoinAttrs);

        void computeQROfTail(void);

        void computeQROfGeneralizedTail(void);


        // Should be called for a root relation.
        void computeQROfConcatenatedGeneralizedHeadAndTails(
            const std::vector<Relation*>& pRelationOrder,
            MatrixEigenT* pR);





        /**
         *  Returns computed head for the corresponding relation, without
         *  dividing by square roots.
         */
        const MatrixDT& getHead(void) const;

        const MatrixDT& getTail(void) const;

        const MatrixDT& getGeneralizedTail(void) const;

        /****************** OLD IMPLEMENTATION **************************/
        void getAttributeValuesCounts(const std::string& attributeName,
            std::unordered_map<double, uint32_t>& htCnts) const;

        void joinRelation(const Relation& relation,
             const std::vector<std::tuple<std::string, std::string> >& vJoinAttributeNames,
             bool bSwapAttributes);

        void computeAndScaleGeneralizedHeadAndTail(
            const std::string& attributeName,
            const std::unordered_map<double, uint32_t>& hashTabAttributeCounts);

        void extend(const Relation& rel, const std::string& attrIterName);

        void applyEigenQR(MatrixEigenT* pR = nullptr);

        friend std::ostream& operator<<(
            std::ostream& out,
            const Relation& relation);
    };
}
#endif