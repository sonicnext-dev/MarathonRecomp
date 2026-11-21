#pragma once

#include <Marathon.inl>

namespace Kaim
{
    class CSpatialGraph;

    class CEdge
    {
    public:
        be<uint32_t> m_id;
        xpointer<CSpatialGraph> m_graph;
    };

    MARATHON_ASSERT_OFFSETOF(CEdge, m_id, 0x00);
    MARATHON_ASSERT_OFFSETOF(CEdge, m_graph, 0x04);
    MARATHON_ASSERT_SIZEOF(CEdge, 0x08);
}
