#pragma once

#include <Marathon.inl>
#include <Kaim/CConstraint.h>
#include <Kaim/CEntity.h>
#include <Kaim/CGraphData.h>
#include <Kaim/CHeuristic.h>
#include <Kaim/CObject.h>
#include <Kaim/CSmartGraphTraversal.h>
#include <Kaim/CStdArray.h>
#include <Kaim/CVarList.h>
#include <Kaim/CVertex.h>
#include <Kaim/CWorldService.h>

namespace Kaim
{
    class CSpatialGraph;

    class CGraphManager : public CWorldService
    {
    public:
        class CGraphMap : public CObject
        {
        public:
            char m_name[128];
            xpointer<CSpatialGraph> m_graph;
            bool m_updtateWithPathObjects;
            xpointer<CGraphData> m_graphData[32];
            be<uint32_t> m_graphDataCount;
            bool m_deleteWhenRemoved;
        };

        class TraversalData : public CObject
        {
        public:
            xpointer<char> configName;
            CVarList<xpointer<CSmartGraphTraversal>> traversalsPool;
        };

        xpointer<xpointer<CGraphMap>> m_graphes;
        be<uint32_t> m_grapheCount;
        be<uint32_t> m_graphesSize;
        bool m_initAstar;
        CStdArray<xpointer<TraversalData>> traversalData;
        be<uint32_t> m_maxVertexCount;
        xpointer<CVarList<xpointer<CVertex>>> m_opened;
        xpointer<CVarList<xpointer<CVertex>>> m_closed;
        xpointer<CVarList<xpointer<CVertex>>::CIterator> m_openedAcc;
        xpointer<CVarList<xpointer<CVertex>>::CIterator> m_closedAcc;
        xpointer<be<float>> m_costFromStart;
        xpointer<be<float>> m_estimatedCostToDest;
        xpointer<CVertex> m_currentlyFlagedVertex;
        be<uint32_t> m_flagedVertex;
        be<int> m_astarLoopTaskId;
        bool m_newAstar;
        xpointer<CVertex> m_lastStart;
        xpointer<CVertex> m_lastDest;
        xpointer<CEntity> m_lastBot;
        xpointer<CHeuristic> m_heuristic;
        xpointer<CConstraint> m_constraint;
    };

    MARATHON_ASSERT_OFFSETOF(CGraphManager::CGraphMap, m_name, 0x04);
    MARATHON_ASSERT_OFFSETOF(CGraphManager::CGraphMap, m_graph, 0x84);
    MARATHON_ASSERT_OFFSETOF(CGraphManager::CGraphMap, m_updtateWithPathObjects, 0x88);
    MARATHON_ASSERT_OFFSETOF(CGraphManager::CGraphMap, m_graphData, 0x8C);
    MARATHON_ASSERT_OFFSETOF(CGraphManager::CGraphMap, m_graphDataCount, 0x10C);
    MARATHON_ASSERT_OFFSETOF(CGraphManager::CGraphMap, m_deleteWhenRemoved, 0x110);
    MARATHON_ASSERT_SIZEOF(CGraphManager::CGraphMap, 0x114);

    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_graphes, 0x04);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_grapheCount, 0x08);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_graphesSize, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_initAstar, 0x10);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, traversalData, 0x14);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_maxVertexCount, 0x24);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_opened, 0x28);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_closed, 0x2C);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_openedAcc, 0x30);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_closedAcc, 0x34);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_costFromStart, 0x38);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_estimatedCostToDest, 0x3C);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_currentlyFlagedVertex, 0x40);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_flagedVertex, 0x44);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_astarLoopTaskId, 0x48);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_newAstar, 0x4C);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_lastStart, 0x50);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_lastDest, 0x54);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_lastBot, 0x58);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_heuristic, 0x5C);
    MARATHON_ASSERT_OFFSETOF(CGraphManager, m_constraint, 0x60);
    MARATHON_ASSERT_SIZEOF(CGraphManager, 0x64);
}
