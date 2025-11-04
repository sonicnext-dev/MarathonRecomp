#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/FrameBufferObject.h>

namespace Sonicteam::SoX::Graphics::Xenon
{
    class FrameBufferObjectXenon : public FrameBufferObject
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x8);
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0x3C);
    };

    MARATHON_ASSERT_SIZEOF(FrameBufferObjectXenon, 0x80);
}
