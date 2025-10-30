#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/Input/TimedAction.h>

namespace Sonicteam::Player::Input
{
    class ListenerNormal : public IListener, public IPlugIn, public IVariable, public IStepable
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<bool> m_pIsListening;
        be<uint32_t> m_State;
        MARATHON_INSERT_PADDING(0x50);
        be<SoX::Input::KeyState> m_ActionA;
        be<SoX::Input::KeyState> m_ActionB;
        be<SoX::Input::KeyState> m_ActionC;
        be<SoX::Input::KeyState> m_ActionD;
        TimedAction m_TimedAction;
    };
}
