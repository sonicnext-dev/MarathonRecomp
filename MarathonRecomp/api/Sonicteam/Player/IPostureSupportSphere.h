#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IPostureSupportSphere
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0xCC);
    };
}
