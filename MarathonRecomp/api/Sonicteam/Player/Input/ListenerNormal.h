#pragma once

#include <Marathon.inl>
#include <Sonicteam/Camera/Cameraman.h>
#include <Sonicteam/Player/Input/AccumulativeAction.h>
#include <Sonicteam/Player/Input/TimedAction.h>
#include <Sonicteam/SoX/Input/Manager.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <stdx/deque.h>

namespace Sonicteam::Player::Input
{
    class ListenerNormal : public IListener, public IPlugIn, public IVariable, public IStepable
    {
    public:
        enum ListenerState : uint32_t
        {
            ListenerState_SouthPressed = (1 << 0),
            ListenerState_SouthDown = (1 << 1),
            ListenerState_SouthReleased = (1 << 2),
            ListenerState_LeftStickHorizontal = (1 << 3),
            ListenerState_NorthPressed1 = (1 << 4),
            ListenerState_SouthDoublePressed = (1 << 5),
            ListenerState_WestPressed1 = (1 << 6),
            ListenerState_Bit7 = (1 << 7),
            ListenerState_WestPressed2 = (1 << 8),
            ListenerState_WestDown = (1 << 9),
            ListenerState_WestReleased1 = (1 << 10),
            ListenerState_WestReleased2 = (1 << 11),
            ListenerState_Bit12 = (1 << 12),
            ListenerState_WestPressed3 = (1 << 13),
            ListenerState_WestReleased3 = (1 << 14),
            ListenerState_NorthPressed2 = (1 << 15),
            ListenerState_RightTriggerPressed = (1 << 16),
            ListenerState_RightTriggerDown = (1 << 17),
            ListenerState_RightTriggerReleased = (1 << 18),
            ListenerState_RightTriggerDoublePressed = (1 << 19),
            ListenerState_DPadRightPressed = (1 << 20),
            ListenerState_DPadLeftPressed = (1 << 21)
        };

        xpointer<xpointer<Camera::Cameraman>> m_ppCameraman;
        xpointer<bool> m_pIsListening;
        be<uint32_t> m_State;
        be<float> m_LeftStickLength;
        SoX::Math::Vector m_LeftStickNormal;
        stdx::deque<SoX::Math::Vector2> m_LeftStickHistory;
        be<float> m_Field74;
        TimedAction m_SouthTimedAction;
        AccumulativeAction m_WestAccumulativeAction;
        be<uint32_t> m_SouthAction;
        be<uint32_t> m_WestAction;
        be<uint32_t> m_NorthAction;
        be<uint32_t> m_RightTriggerAction;
        TimedAction m_RightTriggerTimedAction;
        be<float> m_InputBorder;
    };

    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_ppCameraman, 0x40);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_pIsListening, 0x44);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_State, 0x48);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_LeftStickLength, 0x4C);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_LeftStickNormal, 0x50);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_LeftStickHistory, 0x60);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_Field74, 0x74);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_SouthTimedAction, 0x78);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_WestAccumulativeAction, 0x88);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_SouthAction, 0x9C);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_WestAction, 0xA0);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_NorthAction, 0xA4);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_RightTriggerAction, 0xA8);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_RightTriggerTimedAction, 0xAC);
    MARATHON_ASSERT_OFFSETOF(ListenerNormal, m_InputBorder, 0xBC);
    MARATHON_ASSERT_SIZEOF(ListenerNormal, 0xC0);
}
