#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::State
{
    class ICommonContextIF
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpSetImpulse;
            be<uint32_t> fpfunc4;
            be<uint32_t> fpGetFlag;
            be<uint32_t> fpGetFlag2;
            be<uint32_t> fpGetFlag3;
            be<uint32_t> fpGetTotalVerticalImpulse;
            be<uint32_t> fpGetTotalForwardImpulse;
        };

        xpointer<Vftable> m_pVftable;

        float GetTotalVerticalImpulse();

        float GetTotalForwardImpulse();

        void SetImpulse(float forward, float vertical, float lockInput);

    };

    MARATHON_ASSERT_OFFSETOF(ICommonContextIF, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(ICommonContextIF, 4);
}

#include "ICommonContextIF.inl"
