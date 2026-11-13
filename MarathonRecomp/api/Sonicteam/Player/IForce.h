#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/IPlugIn.h>

namespace Sonicteam::Player
{
    class IForce : public IPlugIn, public IFlagCommunicator
    {
    public:
        be<uint32_t> m_Flags;
        MARATHON_INSERT_PADDING(0x08);
    };

    MARATHON_ASSERT_OFFSETOF(IForce, m_Flags, 0x24);
    MARATHON_ASSERT_SIZEOF(IForce, 0x30);
}
