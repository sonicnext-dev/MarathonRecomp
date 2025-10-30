#pragma once

#include <Marathon.inl>

namespace Sonicteam::Message
{
    struct MsgMissionGetGlobalFlag : SoX::Message<0x1E004>
    {
        be<uint32_t> FlagID;
        be<uint32_t> FlagValue;

        MsgMissionGetGlobalFlag(uint32_t flagId) : FlagID(flagId) {}
    };
}
