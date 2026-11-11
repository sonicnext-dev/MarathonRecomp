#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/System/Peripheral/IManagerImp.h>

namespace Sonicteam::System::Peripheral
{
    class ManagerImpXenon : public IManagerImp
    {
    public:
        MARATHON_INSERT_PADDING(0x54);
    };

    MARATHON_ASSERT_SIZEOF(ManagerImpXenon, 0x58);
}
