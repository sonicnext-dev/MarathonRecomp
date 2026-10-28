#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/DocMode.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    class DevTitleMode : public SoX::Engine::DocMode
    {
    public:
        struct Mode
        {
            xpointer<const char> Name;
            MARATHON_INSERT_PADDING(0x04);
        };

        be<uint32_t> m_SelectedIndex;
        MARATHON_INSERT_PADDING(0x04);
        stdx::vector<Mode> m_Modes;
        MARATHON_INSERT_PADDING(0x2C);
    };

    MARATHON_ASSERT_OFFSETOF(DevTitleMode::Mode, Name, 0x00);

    MARATHON_ASSERT_OFFSETOF(DevTitleMode, m_SelectedIndex, 0x50);
    MARATHON_ASSERT_OFFSETOF(DevTitleMode, m_Modes, 0x58);
    MARATHON_ASSERT_SIZEOF(DevTitleMode, 0x94);
}
