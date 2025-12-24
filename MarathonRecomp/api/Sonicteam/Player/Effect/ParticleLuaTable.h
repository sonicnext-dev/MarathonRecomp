#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::Effect
{
    struct ParticleLuaInfo
    {
        be<uint16_t> BankID;
        be<uint16_t> ParticleID;
        bool IsAcroarts;
        bool IsReverse;
        be<uint32_t> Direction;
        xpointer<const char> pNodeName;
        be<float> OffsetX;
        be<float> OffsetY;
        be<float> OffsetZ;
    };

    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, BankID, 0x00);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, IsAcroarts, 0x04);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, IsReverse, 0x05);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, Direction, 0x08);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, pNodeName, 0x0C);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, OffsetX, 0x10);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, OffsetY, 0x14);
    MARATHON_ASSERT_OFFSETOF(ParticleLuaInfo, OffsetZ, 0x18);
    MARATHON_ASSERT_SIZEOF(ParticleLuaInfo, 0x1C);
}
