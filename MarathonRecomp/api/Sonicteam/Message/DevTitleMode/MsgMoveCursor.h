#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::DevTitleMode
{
    struct MsgMoveCursor : SoX::Message<0x10001>
    {
        enum CursorDirection
        {
            CursorDirection_Up,
            CursorDirection_Down = 180
        };

        be<CursorDirection> Direction{};

        MsgMoveCursor(CursorDirection in_direction = CursorDirection_Down)
            : Direction(in_direction) {}
    };

    MARATHON_ASSERT_OFFSETOF(MsgMoveCursor, Direction, 0x04);
}
