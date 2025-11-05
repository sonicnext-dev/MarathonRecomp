#pragma once

#include <Marathon.inl>
#include <stdx/string.h>

namespace Sonicteam::Player
{
    class IPlugIn
    {
    public:
        xpointer<void> m_pVftable;
        stdx::string m_Name;
    };

    MARATHON_ASSERT_OFFSETOF(IPlugIn, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(IPlugIn, m_Name, 0x04);
    MARATHON_ASSERT_SIZEOF(IPlugIn, 0x20);
}
