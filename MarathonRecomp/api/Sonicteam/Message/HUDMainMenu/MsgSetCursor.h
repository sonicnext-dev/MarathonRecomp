#pragma once

#include <Marathon.inl>
#include <Sonicteam/Message/HUDMainMenu/MsgChangeState.h>

namespace Sonicteam::Message::HUDMainMenu
{
    struct MsgSetCursor : MsgChangeState
    {
        be<uint32_t> CursorIndex{};

        MsgSetCursor() {}

        MsgSetCursor(uint32_t state)
        {
            State = state;
        }

        MsgSetCursor(uint32_t state, uint32_t cursorIndex)
        {
            State = state;
            CursorIndex = cursorIndex;
        }
    };

    MARATHON_ASSERT_OFFSETOF(MsgSetCursor, CursorIndex, 0x08);
}
