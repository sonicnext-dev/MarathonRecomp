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
}
