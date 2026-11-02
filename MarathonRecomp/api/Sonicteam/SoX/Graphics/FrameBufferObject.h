#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/FrameBufferType.h>
#include <Sonicteam/SoX/Graphics/Surface.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/Color.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

namespace Sonicteam::SoX::Graphics
{
    class FrameBufferObject : public RefCountObject
    {
    public:
        Array<RefSharedPointer<Surface>, 4> m_FrameBufferSurfaces;
        RefSharedPointer<Surface> m_spDepthStencilSurface;
        be<FrameBufferType> m_Type;
        ARGB8888 m_ClearColor;
        be<float> m_ClearZ;
        MARATHON_INSERT_PADDING(0x04);
        uint8_t m_Field2C;
        be<uint32_t> m_Field30;
        be<uint32_t> m_MSAALevel;
    };

    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_FrameBufferSurfaces, 0x08);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_spDepthStencilSurface, 0x18);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_Type, 0x1C);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_ClearColor, 0x20);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_ClearZ, 0x24);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_Field2C, 0x2C);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_Field30, 0x30);
    MARATHON_ASSERT_OFFSETOF(FrameBufferObject, m_MSAALevel, 0x34);
    MARATHON_ASSERT_SIZEOF(FrameBufferObject, 0x38);
}
