#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>
#include <Sonicteam/SoX/MessageReceiver.h>

namespace Sonicteam::Message::Player
{
    struct MsgAmigoChange : SoX::Message<0x13010>
    {
        xpointer<SoX::MessageReceiver> pPlayer{};
        bool Field08{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgAmigoChange, pPlayer, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgAmigoChange, Field08, 0x08);
    MARATHON_ASSERT_SIZEOF(MsgAmigoChange, 0x0C);
}
