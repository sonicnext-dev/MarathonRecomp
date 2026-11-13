#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/Input/IListener.h>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/Player/IVariable.h>

namespace Sonicteam::Player::Input
{
    class ListenerVehicle : public IListener, public IPlugIn, public IVariable, public IStepable
    {
    public:
        MARATHON_INSERT_PADDING(0x34);
        bool m_Field74;
        MARATHON_INSERT_PADDING(0x08);
    };

    MARATHON_ASSERT_OFFSETOF(ListenerVehicle, m_Field74, 0x74);
    MARATHON_ASSERT_SIZEOF(ListenerVehicle, 0x80);
}
