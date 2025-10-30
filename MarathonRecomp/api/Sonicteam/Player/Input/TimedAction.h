#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::Input
{
    struct TimedAction
    {
        be<float> m_Time;
        be<SoX::Input::KeyState> m_Action;
        bool m_Field08;
        bool m_IsDoubleTapped;
        bool m_Field0A;
        bool m_Field0B;
        be<float> m_Field0C;
    };
}
