#pragma once

#include <Marathon.inl>

namespace Sonicteam::Prop
{
    class IActorCreator
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IActorCreator, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IActorCreator, 0x04);
}
