#pragma once

#include <Marathon.inl>
#include <Kaim/CEdge.h>
#include <Kaim/CEntity.h>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CPathObjectManager;
    class CSpatialGraph;

    class CPathObject : public CEntity
    {
    public:
        class CLinkedEdge : public CObject
        {
        public:
            xpointer<CSpatialGraph> m_graph;
            xpointer<CEdge> m_edge;
            xpointer<CPathObject> m_nextPathObject;
        };

        xpointer<CLinkedEdge> m_linkedEdges;
        be<uint32_t> m_nbLinkedEdges;
        xpointer<CPathObjectManager> m_pom;
        be<uint32_t> m_linkedEdgesMax;
        be<float> m_hasChangedTime;
        xpointer<CEntity> m_hasChangedParam;
    };

    MARATHON_ASSERT_OFFSETOF(CPathObject::CLinkedEdge, m_graph, 0x04);
    MARATHON_ASSERT_OFFSETOF(CPathObject::CLinkedEdge, m_edge, 0x08);
    MARATHON_ASSERT_OFFSETOF(CPathObject::CLinkedEdge, m_nextPathObject, 0x0C);
    MARATHON_ASSERT_SIZEOF(CPathObject::CLinkedEdge, 0x10);

    MARATHON_ASSERT_OFFSETOF(CPathObject, m_linkedEdges, 0x70);
    MARATHON_ASSERT_OFFSETOF(CPathObject, m_nbLinkedEdges, 0x74);
    MARATHON_ASSERT_OFFSETOF(CPathObject, m_pom, 0x78);
    MARATHON_ASSERT_OFFSETOF(CPathObject, m_linkedEdgesMax, 0x7C);
    MARATHON_ASSERT_OFFSETOF(CPathObject, m_hasChangedTime, 0x80);
    MARATHON_ASSERT_OFFSETOF(CPathObject, m_hasChangedParam, 0x84);
    MARATHON_ASSERT_SIZEOF(CPathObject, 0x88);
}
