#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IScore : public IPlugIn {};

    MARATHON_ASSERT_SIZEOF(IScore, 0x20);
}
