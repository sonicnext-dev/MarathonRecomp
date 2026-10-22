#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/CommonObject.h>

namespace Sonicteam::Player::State
{
    class CommonFall : public CommonObject
    {
    public:
        MARATHON_INSERT_PADDING(0x08);
    };

    MARATHON_ASSERT_SIZEOF(CommonFall, 0x14);
}
