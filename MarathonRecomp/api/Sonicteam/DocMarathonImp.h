#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Input/Manager.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Thread.h>
#include <Sonicteam/MyGraphicsDevice.h>
#include <Sonicteam/RaderMapManager.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    class RenderTargetContainer;
    class SFXAgent;

    class DocMarathonImp : public SoX::Engine::Doc
    {
    public:
        MARATHON_INSERT_PADDING(4);
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        MARATHON_INSERT_PADDING(0x38);
        stdx::vector<boost::shared_ptr<SoX::Input::Manager>> m_vspInputManagers;
        MARATHON_INSERT_PADDING(0x24);
        bool m_VFrame;
        MARATHON_INSERT_PADDING(4);
        xpointer<RenderTargetContainer> m_pRenderTargetContainer;
        xpointer<SFXAgent> m_pSFXAgent;
        MARATHON_INSERT_PADDING(0x8);
        be<float> m_DocSpeed;
        be<uint32_t> m_PauseFlags;
        MARATHON_INSERT_PADDING(0x8);
        xpointer<SoX::LinkedList<SoX::Thread>> m_lnThread;
        MARATHON_INSERT_PADDING(0x288);
        xpointer<void> m_pParticleManager;
        MARATHON_INSERT_PADDING(0x15D0);
        xpointer<RaderMapManager> m_pRaderMapManager;
        MARATHON_INSERT_PADDING(0x542D0);
        be<uint32_t> m_aPadIDs[4];
        MARATHON_INSERT_PADDING(0x2C);
    };

    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_DocSpeed, 0xE8);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_PauseFlags, 0xEC);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_vspInputManagers, 0x9C);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_VFrame, 0xD0);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_aPadIDs, 0x55C2C);
    MARATHON_ASSERT_SIZEOF(DocMarathonImp, 0x55C68);
}
