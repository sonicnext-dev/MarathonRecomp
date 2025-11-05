#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class sonicXmaPlayer : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0xD8);
        bool m_IsPaused;
        MARATHON_INSERT_PADDING(0x108);
    };

    MARATHON_ASSERT_OFFSETOF(sonicXmaPlayer, m_IsPaused, 0xE0);
    MARATHON_ASSERT_SIZEOF(sonicXmaPlayer, 0x1EC);
}
