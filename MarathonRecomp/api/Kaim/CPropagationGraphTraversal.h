#pragma once

#include <Marathon.inl>
#include <Kaim/CSmartGraphTraversal.h>
#include <Kaim/CVertex.h>

namespace Kaim
{
    class CPropagationGraphTraversal : public CSmartGraphTraversal
    {
    public:
        xpointer<be<uint32_t>> m_edgeIds;
        xpointer<be<float>> m_edgeDistances;
        be<uint32_t> m_edgeIdsSize;
        be<uint32_t> m_edgeIdsCount;
        be<uint32_t> m_currentIndex;
        be<uint32_t> m_propagationId;
        xpointer<CVertex> m_startingVertex;
        bool m_freeMemory;
        be<float> m_squareDistanceMax;
    };

    MARATHON_ASSERT_OFFSETOF(CPropagationGraphTraversal, m_edgeIds, 0x18);
    MARATHON_ASSERT_OFFSETOF(CPropagationGraphTraversal, m_edgeDistances, 0x1C);
    MARATHON_ASSERT_OFFSETOF(CPropagationGraphTraversal, m_edgeIdsSize, 0x20);
    MARATHON_ASSERT_OFFSETOF(CPropagationGraphTraversal, m_edgeIdsCount, 0x24);
    MARATHON_ASSERT_OFFSETOF(CPropagationGraphTraversal, m_currentIndex, 0x28);
    MARATHON_ASSERT_OFFSETOF(CPropagationGraphTraversal, m_propagationId, 0x2C);
    MARATHON_ASSERT_OFFSETOF(CPropagationGraphTraversal, m_startingVertex, 0x30);
    MARATHON_ASSERT_OFFSETOF(CPropagationGraphTraversal, m_freeMemory, 0x34);
    MARATHON_ASSERT_OFFSETOF(CPropagationGraphTraversal, m_squareDistanceMax, 0x38);
    MARATHON_ASSERT_SIZEOF(CPropagationGraphTraversal, 0x3C);
}
