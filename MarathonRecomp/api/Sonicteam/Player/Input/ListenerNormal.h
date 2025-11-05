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
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_pIsListening, 0x44);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_State, 0x48);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_ActionA, 0x9C);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_ActionB, 0xA0);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_ActionC, 0xA4);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_ActionD, 0xA8);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_TimedAction, 0xAC);
    MARATHON_ASSERT_SIZEOF(ListenerNormal, 0xC0);
}
