#include "database/query/ASTJoinAttributesComputeVisitor.h"

namespace Figaro
{
    void ASTJoinAttributesComputeVisitor::visitNodeRelation(ASTNodeRelation* pElement)
    {
        pElement->checkAndUpdateJoinAttributes();
        const auto& relationName = pElement->getRelationName();
        const auto& formJoinAttrNames = getFormateJoinAttributeNames(pElement->getJoinAttributeNames());
        FIGARO_LOG_DBG("relation", relationName, "joinAttributeNames", formJoinAttrNames);
    }

    void ASTJoinAttributesComputeVisitor::visitNodeJoin(ASTNodeJoin* pElement)
    {
        FIGARO_LOG_DBG("Join");
        FIGARO_LOG_DBG("Central");
        for (const auto& pChild: pElement->getChildren())
        {
            FIGARO_LOG_DBG("Child");
            pChild->accept(this);
        }
        pElement->checkAndUpdateJoinAttributes();
        pElement->checkAndUpdateChildrenParJoinAttributes();

        const auto& relationName = pElement->getCentralRelation()->getRelationName();
        const auto& formJoinAttrNames = getFormateJoinAttributeNames(pElement->getJoinAttributeNames());
        FIGARO_LOG_DBG("relation", relationName, "joinAttributeNames", formJoinAttrNames);
    }

    void ASTJoinAttributesComputeVisitor::visitNodeQRGivens(ASTNodeQRGivens* pElement)
    {
        FIGARO_LOG_DBG("QR Givens");
        FIGARO_LOG_DBG("Relation order", pElement->getRelationOrder())
        for (const auto& relName: pElement->getRelationOrder())
        {
            m_vpASTNodeRelation.push_back(m_mRelNameASTNodeRel.at(relName));
        }
        pElement->getOperand()->accept(this);

    }

}