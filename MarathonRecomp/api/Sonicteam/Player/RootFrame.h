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
        SoX::Math::Matrix4x4 m_TransformMatrix0x70; //set/get
        SoX::Math::Matrix4x4 m_TransformMatrix0xB0; //get
        SoX::Math::Vector m_Position0xF0;
        SoX::Math::Vector m_Position0x100;
        SoX::Math::Vector m_Impulse;
        MARATHON_INSERT_PADDING(0x30);
        be<uint64_t> m_ExternalFlag;
    };
}
