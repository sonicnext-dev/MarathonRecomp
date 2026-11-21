#pragma once

#include <Marathon.inl>
#include <Kaim/CObject.h>

namespace Kaim
{
    class CSpatialGraph;

    class CSmartGraphTraversal : public CObject
    {
    public:
        xpointer<CSpatialGraph> m_graph;
        be<int> m_aperiodicTaskId;
        be<int> m_estimatedTaskId;
        bool m_finished;
        be<uint32_t> m_configId;
    };

    MARATHON_ASSERT_OFFSETOF(CSmartGraphTraversal, m_graph, 0x04);
    MARATHON_ASSERT_OFFSETOF(CSmartGraphTraversal, m_aperiodicTaskId, 0x08);
    MARATHON_ASSERT_OFFSETOF(CSmartGraphTraversal, m_estimatedTaskId, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CSmartGraphTraversal, m_finished, 0x10);
    MARATHON_ASSERT_OFFSETOF(CSmartGraphTraversal, m_configId, 0x14);
    MARATHON_ASSERT_SIZEOF(CSmartGraphTraversal, 0x18);
}
