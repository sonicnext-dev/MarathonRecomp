#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPostureSupportEdge
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x6C);
    };
}
