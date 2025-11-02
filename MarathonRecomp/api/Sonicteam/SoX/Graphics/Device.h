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
        MARATHON_INSERT_PADDING(0x14);
        be<uint32_t> m_DBRFlag;
        MARATHON_INSERT_PADDING(0x24);
        Math::Matrix4x4 m_Field40;
        be<float> m_Field80;
        be<float> m_Field84; //viewport?
        MARATHON_INSERT_PADDING(0x4);
        be<uint32_t> m_Width1;
        be<uint32_t> m_Height1;
        be<uint32_t> m_Field94;
        be<uint32_t> m_Field98;
        be<uint32_t> m_Width2;
        be<uint32_t> m_Height2;
        MARATHON_INSERT_PADDING(0x1C);
        SoX::RefSharedPointer<SoX::RefCountObject> m_FrameBufferObject;
        MARATHON_INSERT_PADDING(0xC);
    };

    MARATHON_ASSERT_OFFSETOF(Device, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(Device, m_Field40, 0x40);
    MARATHON_ASSERT_OFFSETOF(Device, m_Field80, 0x80);
    MARATHON_ASSERT_SIZEOF(Device, 0xD0);
}
