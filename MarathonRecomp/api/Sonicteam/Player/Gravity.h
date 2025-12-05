#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IForce.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player
{
    class Gravity : public IForce, public IVariable, public IStepable
    {
    public:
        be<float> c_gravity;
        be<float> c_pass_fluid;
        bool m_Enabled; // ground = false, air = true
        be<float> m_DownForce;
        SoX::Math::Vector m_DownDirection;

    };

    MARATHON_ASSERT_SIZEOF(Gravity, 0x60);
    MARATHON_ASSERT_OFFSETOF(Gravity, c_gravity, 0x38);
    MARATHON_ASSERT_OFFSETOF(Gravity, c_pass_fluid, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Enabled, 0x40);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_DownForce, 0x44);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_DownDirection, 0x50);
}
