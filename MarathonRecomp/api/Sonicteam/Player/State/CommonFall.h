#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/CommonObject.h>

namespace Sonicteam::Player::State
{
    class CommonFall : public CommonObject
    {
    public:
        be<float> m_FallTime;
        bool m_IsRunBorder;
    };

    MARATHON_ASSERT_OFFSETOF(CommonFall, m_FallTime, 0xC);
    MARATHON_ASSERT_OFFSETOF(CommonFall, m_IsRunBorder, 0x10);
    MARATHON_ASSERT_SIZEOF(CommonFall, 0x14);
}
