#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/Color.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

#undef DrawText

namespace Sonicteam::SoX::Graphics
{
    class Device
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0xB8);
            be<uint32_t> fpDrawText;
            MARATHON_INSERT_PADDING(0x08);
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0x14);
        be<uint32_t> m_DBRFlag;
        MARATHON_INSERT_PADDING(0x24);
        Math::Matrix4x4 m_Field40;
        be<float> m_Field80;
        be<float> m_Field84; // Viewport?
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_WidthA;
        be<uint32_t> m_HeightA;
        be<uint32_t> m_Field94;
        be<uint32_t> m_Field98;
        be<uint32_t> m_WidthB;
        be<uint32_t> m_HeightB;
        MARATHON_INSERT_PADDING(0x1C);
        RefSharedPointer<RefCountObject> m_spFrameBufferObject;
        MARATHON_INSERT_PADDING(0x0C);

        void DrawText(const char* in_pText, const uint32_t in_x, const uint32_t in_y, const ARGB8888* in_pColor = nullptr)
        {
            if (!in_pText)
                return;

            const auto text = guest_stack_var<stdx::string>(in_pText);
            const auto color = guest_stack_var<ARGB8888>(in_pColor ? *in_pColor : ARGB8888{ { 255, 255, 255, 255 } });

            GuestToHostFunction<void>(m_pVftable->fpDrawText.get(), this, text->c_str(), in_x, in_y, color.get());
        }
    };

    MARATHON_ASSERT_OFFSETOF(Device::Vftable, fpDrawText, 0xB8);
    MARATHON_ASSERT_SIZEOF(Device::Vftable, 0xC4);

    MARATHON_ASSERT_OFFSETOF(Device, m_DBRFlag, 0x18);
    MARATHON_ASSERT_OFFSETOF(Device, m_Field40, 0x40);
    MARATHON_ASSERT_OFFSETOF(Device, m_Field80, 0x80);
    MARATHON_ASSERT_OFFSETOF(Device, m_Field84, 0x84);
    MARATHON_ASSERT_OFFSETOF(Device, m_Field84, 0x84);
    MARATHON_ASSERT_OFFSETOF(Device, m_WidthA, 0x8C);
    MARATHON_ASSERT_OFFSETOF(Device, m_HeightA, 0x90);
    MARATHON_ASSERT_OFFSETOF(Device, m_Field94, 0x94);
    MARATHON_ASSERT_OFFSETOF(Device, m_Field98, 0x98);
    MARATHON_ASSERT_OFFSETOF(Device, m_WidthB, 0x9C);
    MARATHON_ASSERT_OFFSETOF(Device, m_HeightB, 0xA0);
    MARATHON_ASSERT_OFFSETOF(Device, m_spFrameBufferObject, 0xC0);
    MARATHON_ASSERT_SIZEOF(Device, 0xD0);
}
