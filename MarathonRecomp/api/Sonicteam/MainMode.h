#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/FrameGP.h>
#include <Sonicteam/SoX/Scenery/Camera.h>

namespace Sonicteam
{
    class MainMode : public SoX::Engine::DocMode
    {
    public:
        MARATHON_INSERT_PADDING(0x24);
        boost::shared_ptr<SoX::Scenery::Camera> m_spSelectCamera;
        xpointer<SoX::Graphics::FrameGP> m_pFrameGPStandard;
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_OFFSETOF(MainMode, m_spSelectCamera, 0x74);
    MARATHON_ASSERT_OFFSETOF(MainMode, m_pFrameGPStandard, 0x7C);
    MARATHON_ASSERT_SIZEOF(MainMode, 0x8C);
}
