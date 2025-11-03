#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IExportPostureRequestFlag
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IExportPostureRequestFlag, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IExportPostureRequestFlag, 4);
}
