#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Matrix.h>

namespace Sonicteam::SoX::Graphics
{
    class Device
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x3C);
        Math::Matrix4x4 m_Field40;
        be<float> m_Field80;
        MARATHON_INSERT_PADDING(0x4C);
    };
}
