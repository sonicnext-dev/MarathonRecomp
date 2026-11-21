#pragma once

#include <Marathon.inl>
#include <Kaim/IHeuristic.h>

namespace Kaim
{
    class CSpatialGraph;

    class CHeuristic : public IHeuristic
    {
    public:
        xpointer<CSpatialGraph> m_graph;
    };

    MARATHON_ASSERT_OFFSETOF(CHeuristic, m_graph, 0x04);
    MARATHON_ASSERT_SIZEOF(CHeuristic, 0x08);
}
