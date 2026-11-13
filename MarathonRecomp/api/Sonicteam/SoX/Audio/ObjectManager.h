#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Audio
{
    class ObjectManager
    {
    public:
        xpointer<void> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(ObjectManager, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(ObjectManager, 0x04);
}
