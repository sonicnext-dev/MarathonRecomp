#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Engine
{
    class RenderScheduler
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x8);
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0xC);
    };

    MARATHON_ASSERT_SIZEOF(RenderScheduler, 0x10);
}
