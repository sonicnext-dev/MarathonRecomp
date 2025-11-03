#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IExportExternalFlag.h>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/SoX/Graphics/Frame.h>
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player
{
    class RootFrame : public SoX::Graphics::Frame, public IPlugIn, public IExportExternalFlag
    {
    public:
        SoX::Math::Matrix4x4 m_Field70;
        SoX::Math::Matrix4x4 m_FieldB0;
        SoX::Math::Vector m_PositionF0;
        SoX::Math::Vector m_Field100;
        SoX::Math::Vector m_Impulse;
        MARATHON_INSERT_PADDING(0x30);
        be<uint64_t> m_ExternalFlag;
    };

    MARATHON_ASSERT_OFFSETOF(RootFrame, m_Field70, 0x70);
    MARATHON_ASSERT_OFFSETOF(RootFrame, m_FieldB0, 0xB0);
    MARATHON_ASSERT_OFFSETOF(RootFrame, m_PositionF0, 0xF0);
    MARATHON_ASSERT_OFFSETOF(RootFrame, m_Field100, 0x100);
    MARATHON_ASSERT_OFFSETOF(RootFrame, m_Impulse, 0x110);
    MARATHON_ASSERT_OFFSETOF(RootFrame, m_ExternalFlag, 0x150);
    MARATHON_ASSERT_SIZEOF(RootFrame, 0x160);
}
