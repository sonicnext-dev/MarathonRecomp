#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IExportWeaponRequestFlag
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IExportWeaponRequestFlag, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IExportWeaponRequestFlag, 4);
}
