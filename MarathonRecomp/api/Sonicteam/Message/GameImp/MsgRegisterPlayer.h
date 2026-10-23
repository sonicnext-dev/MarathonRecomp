#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::GameImp
{
    struct MsgRegisterPlayer : SoX::Message<0x15026>
    {
        be<uint32_t> ActorID{};
        be<uint32_t> Field08{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgRegisterPlayer, ActorID, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgRegisterPlayer, Field08, 0x08);
    MARATHON_ASSERT_SIZEOF(MsgRegisterPlayer, 0x0C);
}
