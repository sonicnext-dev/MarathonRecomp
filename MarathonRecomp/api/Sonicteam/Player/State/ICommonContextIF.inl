#pragma once

namespace Sonicteam::Player::State
{
    inline float ICommonContextIF::GetTotalVerticalImpulse()
    {
        return GuestToHostFunction<float>(m_pVftable->fpGetTotalVerticalImpulse, this);
    }

    inline float ICommonContextIF::GetTotalForwardImpulse()
    {
        return GuestToHostFunction<float>(m_pVftable->fpGetTotalForwardImpulse, this);
    }

    inline void ICommonContextIF::SetImpulse(float forward, float vertical, float lockInput)
    {
        GuestToHostFunction<void>(m_pVftable->fpSetImpulse, this, forward, vertical, lockInput);
    }
}
