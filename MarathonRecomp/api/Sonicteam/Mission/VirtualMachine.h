#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <Sonicteam/SoX/Array.h>
#include <stdx/string.h>

namespace Sonicteam::Mission
{
    class VirtualMachine : public SoX::MessageReceiver
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
        stdx::string m_MissionName;
        MARATHON_INSERT_PADDING(0x198);
        boost::weak_ptr<Game> m_wpGame;
        SoX::Array<be<int>, 64> m_TemporaryFlags;
        MARATHON_INSERT_PADDING(0x24C);
    };

    MARATHON_ASSERT_OFFSETOF(VirtualMachine, m_MissionName, 0x8);
    MARATHON_ASSERT_OFFSETOF(VirtualMachine, m_wpGame, 0x1BC);
    MARATHON_ASSERT_OFFSETOF(VirtualMachine, m_TemporaryFlags, 0x1C4);
    MARATHON_ASSERT_SIZEOF(VirtualMachine, 0x510);
}
