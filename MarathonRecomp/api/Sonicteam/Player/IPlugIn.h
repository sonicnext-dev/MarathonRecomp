#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPlugIn
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x1C);
    };
}
