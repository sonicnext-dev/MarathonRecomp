#pragma once

#include <Marathon.inl>
#include <Sonicteam/Message/MsgHUDMainMenuChangeState.h>

namespace Sonicteam::Message
{
    struct MsgHUDMainMenuSetCursor : MsgHUDMainMenuChangeState
    {
        be<uint32_t> CursorIndex;

        MsgHUDMainMenuSetCursor() {}

        MsgHUDMainMenuSetCursor(uint32_t state)
        {
            State = state;
        }

        MsgHUDMainMenuSetCursor(uint32_t state, uint32_t cursorIndex)
        {
            State = state;
            CursorIndex = cursorIndex;
        }
    };

    MARATHON_ASSERT_OFFSETOF(MsgHUDMainMenuSetCursor, CursorIndex, 0x08);
}
