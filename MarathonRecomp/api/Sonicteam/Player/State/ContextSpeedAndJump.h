#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::State
{
    class ContextSpeedAndJump 
    {
        be<float> m_BaseSpeedForward;
        be<float> m_GimmickSpeedForward;
        be<float> m_BaseSpeedVertical;
        be<float> m_GimmickSpeedVertical;
    };
}
