#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>

namespace Sonicteam::Player
{
    class IZock : public IPlugIn {};

    MARATHON_ASSERT_SIZEOF(IZock, 0x20);
}
