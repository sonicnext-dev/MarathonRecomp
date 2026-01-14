#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Frame.h>
#include <stdx/string.h>

namespace Sonicteam::SoX::Graphics
{
    class FrameGP : public Frame
    {
    public:
        MARATHON_INSERT_PADDING(0x84);
        stdx::string m_AttachPoint;
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(FrameGP, m_AttachPoint, 0xD0);
    MARATHON_ASSERT_SIZEOF(FrameGP, 0xF0);
}
