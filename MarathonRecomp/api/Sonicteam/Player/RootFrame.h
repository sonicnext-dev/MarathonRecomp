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
        SoX::Math::Vector m_FieldF0;
        SoX::Math::Vector m_Field100;
        SoX::Math::Vector m_Impulse;
        MARATHON_INSERT_PADDING(0x30);
        be<uint64_t> m_ExternalFlag;
    };
}
