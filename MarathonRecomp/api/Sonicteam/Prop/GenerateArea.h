#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::Prop
{
    class GenerateArea : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0x08);
        SoX::Math::Vector m_Field10;
        SoX::Math::Vector m_Field20;
        SoX::Math::Vector m_Field30;
        SoX::Math::Vector m_HalfExtents;
        MARATHON_INSERT_PADDING(0x0C);
        SoX::Math::Vector m_Field60;
        SoX::Math::Vector m_Field70;
        MARATHON_INSERT_PADDING(0x08);
    };

    MARATHON_ASSERT_OFFSETOF(GenerateArea, m_Field10, 0x10);
    MARATHON_ASSERT_OFFSETOF(GenerateArea, m_Field20, 0x20);
    MARATHON_ASSERT_OFFSETOF(GenerateArea, m_Field30, 0x30);
    MARATHON_ASSERT_OFFSETOF(GenerateArea, m_HalfExtents, 0x40);
    MARATHON_ASSERT_OFFSETOF(GenerateArea, m_Field60, 0x60);
    MARATHON_ASSERT_OFFSETOF(GenerateArea, m_Field70, 0x70);
    MARATHON_ASSERT_SIZEOF(GenerateArea, 0x90);
}
