#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Engine
{
    class Application
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(8);
    };

    MARATHON_ASSERT_OFFSETOF(Application, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(Application, 0x0C);
}
