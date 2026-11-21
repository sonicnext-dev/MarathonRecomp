#pragma once

#include <Marathon.inl>
#include <Kaim/IConstraint.h>

namespace Kaim
{
    class CSpatialGraph;

    class CConstraint : public IConstraint
    {
    public:
        xpointer<CSpatialGraph> m_graph;
    };

    MARATHON_ASSERT_OFFSETOF(CConstraint, m_graph, 0x04);
    MARATHON_ASSERT_SIZEOF(CConstraint, 0x08);
}
