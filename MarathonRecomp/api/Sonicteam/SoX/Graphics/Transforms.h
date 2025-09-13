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
}
