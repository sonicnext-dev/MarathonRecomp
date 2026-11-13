#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Message.h>

namespace Sonicteam::Message::Mission
{
    struct MsgGetGlobalFlag : SoX::Message<0x1E004>
    {
        be<uint32_t> FlagID{};
        be<uint32_t> FlagValue{};

        explicit MsgGetGlobalFlag(const uint32_t in_flagId) : FlagID(in_flagId) {}
    };

    MARATHON_ASSERT_OFFSETOF(MsgGetGlobalFlag, FlagID, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgGetGlobalFlag, FlagValue, 0x08);
    MARATHON_ASSERT_SIZEOF(MsgGetGlobalFlag, 0x0C);
}
