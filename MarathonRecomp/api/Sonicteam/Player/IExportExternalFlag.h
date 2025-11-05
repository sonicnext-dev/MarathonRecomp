#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IExportExternalFlag
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IExportExternalFlag, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IExportExternalFlag, 4);
}
