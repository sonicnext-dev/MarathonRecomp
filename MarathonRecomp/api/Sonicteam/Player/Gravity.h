#pragma once

#include <Sonicteam/Player/IForce.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/Player/IVariable.h>

namespace Sonicteam::Player
{
    class Gravity : public IForce, public IVariable, public IStepable
    {
    public:
        be<float> m_Field38;
        be<float> m_Field3C;
        uint8_t m_Field40;
        be<float> m_Field44;
        be<uint32_t> m_Field48;
        be<uint32_t> m_Field4C;
        be<float> m_Field50;
        be<float> m_Field54;
        be<float> m_Field58;
        be<float> m_Field5C;
    };

    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field38, 0x38);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field3C, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field40, 0x40);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field44, 0x44);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field48, 0x48);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field4C, 0x4C);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field50, 0x50);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field54, 0x54);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field58, 0x58);
    MARATHON_ASSERT_OFFSETOF(Gravity, m_Field5C, 0x5C);
    MARATHON_ASSERT_SIZEOF(Gravity, 0x60);
}
