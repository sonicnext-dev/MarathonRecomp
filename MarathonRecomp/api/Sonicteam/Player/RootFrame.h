#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/SoX/Graphics/Frame.h>
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player
{
    class RootFrame : public SoX::Graphics::Frame, public IPlugIn
    {
    public:
        SoX::Math::Matrix4x4 m_Transform70;
        SoX::Math::Matrix4x4 m_TransformB0;
        SoX::Math::Vector m_PositionF0;
        SoX::Math::Vector m_Position100;
        SoX::Math::Vector m_Impulse;
        MARATHON_INSERT_PADDING(0x30);
        be<uint64_t> m_ExternalFlag;
    };
}
