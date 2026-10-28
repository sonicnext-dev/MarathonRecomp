#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::DevTitleMode
{
    struct MsgSelectCursor : SoX::Message<0x10002>
    {
        MARATHON_INSERT_PADDING(0x04);
        bool IsSelected{};

        MsgSelectCursor(bool in_isSelected = true) : IsSelected(in_isSelected) {}
    };

    MARATHON_ASSERT_OFFSETOF(MsgSelectCursor, IsSelected, 0x08);
}
