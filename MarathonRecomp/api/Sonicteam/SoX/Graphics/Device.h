#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::SoX::Graphics
{
    class Device
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x3C);
        Math::Matrix4x4 m_Field40;
        be<float> m_Field80;
        MARATHON_INSERT_PADDING(0x3C);
        SoX::RefSharedPointer<SoX::RefCountObject> m_FrameBufferObject;
        MARATHON_INSERT_PADDING(0xC);
    };
}
