#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Input/Manager.h>
#include <Sonicteam/MyGraphicsDevice.h>
#include <Sonicteam/SFXAgent.h>
#include <Sonicteam/RaderMapManager.h>
#include <Sonicteam/RenderTargetContainer.h>
#include <Sonicteam/Particles/ParticleManager.h>
#include <stdx/vector.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam
{
    class DocMarathonImp : public SoX::Engine::Doc
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        MARATHON_INSERT_PADDING(0x38);
        stdx::vector<boost::shared_ptr<SoX::Input::Manager>> m_vspInputManager;
        MARATHON_INSERT_PADDING(0x24);
        bool m_VFrame;
        MARATHON_INSERT_PADDING(4);
        xpointer<RenderTargetContainer> m_pRenderTargetContainer;
        xpointer<SFXAgent> m_pSFXAgent;
        MARATHON_INSERT_PADDING(0x2A4);
        xpointer<Particles::ParticleManager> m_pParticleManager;
        MARATHON_INSERT_PADDING(0x15D0);
        xpointer<RaderMapManager> m_pRaderMapManager;
        MARATHON_INSERT_PADDING(0x542D0);
        be<uint32_t> m_aPadIDs[4];
        MARATHON_INSERT_PADDING(0x2C);
    };

    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_vspInputManager, 0x9C);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_VFrame, 0xD0);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_aPadIDs, 0x55C2C);
    MARATHON_ASSERT_SIZEOF(DocMarathonImp, 0x55C68);
}
