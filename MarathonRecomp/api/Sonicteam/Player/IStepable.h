#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IStepable
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpStep;
        };

        xpointer<Vftable> m_pVftable;

        void Step(float delta)
        {
            GuestToHostFunction<void>(m_pVftable->fpStep, this, delta);
        }
    };

    MARATHON_ASSERT_OFFSETOF(IStepable, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IStepable, 4);
}
