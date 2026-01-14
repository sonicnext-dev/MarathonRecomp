#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/State/Object2.h>

namespace Sonicteam::Player::State
{
    class CommonContext;

    class CommonObject : public Object2 
    {
    public:
        xpointer<CommonContext> m_pContext;
    };
}
