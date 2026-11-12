#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/AxisAlignedBoxShape.h>
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::Prop
{
    class ActiveArea : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0x08);
        xpointer<ActiveArea> m_pThis;
        MARATHON_INSERT_PADDING(0x04);
        xpointer<AxisAlignedBoxShape> m_pAxisAlignedBoxShape;
        MARATHON_INSERT_PADDING(0x04);
        SoX::Math::Matrix4x4 m_Field20;
        SoX::Math::Vector m_Field60;
        uint8_t m_Field70;
    };

    MARATHON_ASSERT_SIZEOF(ActiveArea, 0x80);
}
