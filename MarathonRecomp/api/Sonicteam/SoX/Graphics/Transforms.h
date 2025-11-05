#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX::Graphics
{
    class Transforms
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(4);
            be<uint32_t> fpComputeProjectionMatrix;
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0xEC);
    };

    MARATHON_ASSERT_OFFSETOF(Transforms, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(Transforms, 0xF0);
}
