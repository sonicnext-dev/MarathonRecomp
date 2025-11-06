#pragma once

#include <Marathon.inl>
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
        MARATHON_INSERT_PADDING(0x14);
        be<uint32_t> m_DBRFlag;
        MARATHON_INSERT_PADDING(0x8);
        xpointer<Xenon::RenderStatesXenon> m_pRenderStates;
        xpointer<Xenon::TextureStagesXenon> m_pTextureStages;
        MARATHON_INSERT_PADDING(0x14);
        Math::Matrix4x4 m_Field40;
        be<float> m_Field80;
        be<float> m_Field84;
        MARATHON_INSERT_PADDING(0x4);
        be<uint32_t> m_Width1;
        be<uint32_t> m_Height1;
        be<uint32_t> m_Field94;
        be<uint32_t> m_Field98;
        be<uint32_t> m_Width2;
        be<uint32_t> m_Height2;
        MARATHON_INSERT_PADDING(0x18);
        RefSharedPointer<> m_FrameBufferObject;
        xpointer<Xenon::FrameBufferObjectXenon> m_pFrameBufferObjectXenon;
        MARATHON_INSERT_PADDING(0x4);
    };

    MARATHON_ASSERT_SIZEOF(Device, 0xC8);
    MARATHON_ASSERT_OFFSETOF(Device, m_pRenderStates, 0x24);
    MARATHON_ASSERT_OFFSETOF(Device, m_pTextureStages, 0x28);
    MARATHON_ASSERT_OFFSETOF(Device, m_FrameBufferObject, 0xBC);
}
