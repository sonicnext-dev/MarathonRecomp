#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPostureSupportInput
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x30 - 4);
    };
}
