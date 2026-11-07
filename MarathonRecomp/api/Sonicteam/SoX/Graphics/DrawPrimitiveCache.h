#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <Sonicteam/SoX/Graphics/DrawPrimitive.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Graphics
{
    class DrawPrimitiveCache : public RefCountObject
    {
    public:
        xpointer<Device> m_pDevice;
        xpointer<DrawPrimitive> m_pPrimitivesA; // 1000 elements
        MARATHON_INSERT_PADDING(0x04);
        xpointer<DrawPrimitive> m_pPrimitivesB; // 1000 elements
        MARATHON_INSERT_PADDING(0x04);
        xpointer<DrawPrimitive> m_pPrimitivesC; // 400 elements
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_OFFSETOF(DrawPrimitiveCache, m_pDevice, 0x08);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitiveCache, m_pPrimitivesA, 0x0C);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitiveCache, m_pPrimitivesB, 0x14);
    MARATHON_ASSERT_OFFSETOF(DrawPrimitiveCache, m_pPrimitivesC, 0x1C);
    MARATHON_ASSERT_SIZEOF(DrawPrimitiveCache, 0x44);
}
