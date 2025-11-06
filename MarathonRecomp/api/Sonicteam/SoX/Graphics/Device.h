#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/FrameBufferObject.h>
#include <Sonicteam/SoX/Graphics/Xenon/FrameBufferObjectXenon.h>
#include <Sonicteam/SoX/Graphics/Xenon/RenderStatesXenon.h>
#include <Sonicteam/SoX/Graphics/Xenon/TextureStagesXenon.h>
#include <Sonicteam/SoX/Math/Matrix.h>

namespace Sonicteam::SoX::Graphics
{
    class Device
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0x20);
        xpointer<Xenon::RenderStatesXenon> m_pRenderStates;
        xpointer<Xenon::TextureStagesXenon> m_pTextureStages;
        MARATHON_INSERT_PADDING(0x14);
        Math::Matrix4x4 m_Field40;
        be<float> m_Field80;
        MARATHON_INSERT_PADDING(0x38);
        xpointer<FrameBufferObject> m_pFrameBufferObject;
        xpointer<Xenon::FrameBufferObjectXenon> m_pFrameBufferObjectXenon;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(Device, 0xC8);
    MARATHON_ASSERT_OFFSETOF(Device, m_pRenderStates, 0x24);
    MARATHON_ASSERT_OFFSETOF(Device, m_pTextureStages, 0x28);
    MARATHON_ASSERT_OFFSETOF(Device, m_pFrameBufferObject, 0xBC);
    MARATHON_ASSERT_OFFSETOF(Device, m_pFrameBufferObjectXenon, 0xC0);
}
