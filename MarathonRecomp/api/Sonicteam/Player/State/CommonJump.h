#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/CommonObject.h>

namespace Sonicteam::Player::State
{
    class CommonJump : public CommonObject
    {
    public:
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_SIZEOF(CommonJump, 0x24);
}
