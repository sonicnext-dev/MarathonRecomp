#pragma once

#include <Marathon.inl>

namespace Sonicteam::Message::Mission
{
    struct MsgGetGlobalFlag : SoX::Message<0x1E004>
    {
        be<uint32_t> FlagID{};
        be<uint32_t> FlagValue{};

        MsgGetGlobalFlag(uint32_t flagId) : FlagID(flagId) {}
    };

    MARATHON_ASSERT_OFFSETOF(MsgGetGlobalFlag, FlagID, 0x04);
    MARATHON_ASSERT_OFFSETOF(MsgGetGlobalFlag, FlagValue, 0x08);
}
