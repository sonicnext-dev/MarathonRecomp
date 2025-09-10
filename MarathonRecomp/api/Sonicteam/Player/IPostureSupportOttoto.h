#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPostureSupportOttoto
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x70 - 4);
    };
}
