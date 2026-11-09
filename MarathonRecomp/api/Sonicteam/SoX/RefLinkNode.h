#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX
{
    template <typename T>
    class RefLinkNode
    {
    public:
        xpointer<T> m_pRef;
        LinkNode<RefLinkNode> m_Node;
    };

    MARATHON_ASSERT_OFFSETOF(RefLinkNode<void>, m_pRef, 0x00);
    MARATHON_ASSERT_OFFSETOF(RefLinkNode<void>, m_Node, 0x04);
    MARATHON_ASSERT_SIZEOF(RefLinkNode<void>, 0x10);
}
