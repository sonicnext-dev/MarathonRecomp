#pragma once

#include <Marathon.inl>
#include <Kaim/CVector.h>

namespace Kaim
{
    class CSpatialGraph;

    class CVertex
    {
    public:
        be<uint32_t> m_id;
        CVector m_position;
        xpointer<CSpatialGraph> m_graph;
    };

    MARATHON_ASSERT_OFFSETOF(CVertex, m_id, 0x00);
    MARATHON_ASSERT_OFFSETOF(CVertex, m_position, 0x04);
    MARATHON_ASSERT_OFFSETOF(CVertex, m_graph, 0x10);
    MARATHON_ASSERT_SIZEOF(CVertex, 0x14);
}
