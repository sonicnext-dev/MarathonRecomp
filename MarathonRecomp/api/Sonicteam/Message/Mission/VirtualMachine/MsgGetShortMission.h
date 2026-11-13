#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::Mission::VirtualMachine
{
    struct MsgGetShortMission : SoX::Message<0x1E00C>
    {
        bool IsShortMission{};
    };

    MARATHON_ASSERT_OFFSETOF(MsgGetShortMission, IsShortMission, 0x04);
    MARATHON_ASSERT_SIZEOF(MsgGetShortMission, 0x08);
}
