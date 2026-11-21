#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CSpatialGraph;

    class CGraphData : public CObject
    {
    public:
        xpointer<CSpatialGraph> m_graph;
    };

    MARATHON_ASSERT_OFFSETOF(CGraphData, m_graph, 0x04);
    MARATHON_ASSERT_SIZEOF(CGraphData, 0x08);
}
