#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/Object2.h>

namespace Sonicteam::Player::State
{
    class CommonObject : public Object2 {
    public:
        xpointer<CommonContext> m_pContext;
    };

    MARATHON_ASSERT_OFFSETOF(CommonObject, m_pContext, 0x8);
    MARATHON_ASSERT_SIZEOF(CommonObject, 0xC);
}
