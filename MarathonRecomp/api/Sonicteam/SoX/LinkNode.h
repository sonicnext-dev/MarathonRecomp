#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNeighbor.h>

namespace Sonicteam::SoX
{
    template <typename T>
    class LinkNode : public LinkNeighbor<LinkNode<T>>
    {
    public:
        xpointer<T> m_pData;
    };

    MARATHON_ASSERT_OFFSETOF(LinkNode<void>, m_pData, 0x08);
    MARATHON_ASSERT_SIZEOF(LinkNode<void>, 0x0C);
}
