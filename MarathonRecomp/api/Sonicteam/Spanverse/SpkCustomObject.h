#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpangleObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Graphics/Frame.h>

namespace Sonicteam::Spanverse
{
    class SpkCustomObject : public SpangleObject
    {
    public:
        SoX::RefSharedPointer<SoX::Graphics::Frame> m_spFrame;
    };

    MARATHON_ASSERT_OFFSETOF(SpkCustomObject, m_spFrame,0x130);
    MARATHON_ASSERT_SIZEOF(SpkCustomObject, 0x140);
}
