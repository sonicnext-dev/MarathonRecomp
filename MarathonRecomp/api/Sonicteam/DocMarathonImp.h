#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Input/Manager.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Thread.h>
#include <Sonicteam/DocMarathon.h>
#include <Sonicteam/DocState.h>
#include <Sonicteam/MyGraphicsDevice.h>
#include <Sonicteam/RaderMapManager.h>
#include <Sonicteam/RenderTargetContainer.h>
#include <Sonicteam/SFXAgent.h>
#include <Sonicteam/TextBook.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    class SceneParamManager;

    class DocMarathonImp : public DocMarathon
    {
    public:
        xpointer<DocState> m_pDocState;
        xpointer<MyGraphicsDevice> m_pMyGraphicsDevice;
        xpointer<SceneParamManager> m_pSceneParamManager;
        MARATHON_INSERT_PADDING(0x34);
        stdx::vector<boost::shared_ptr<SoX::Input::Manager>> m_InputManagers;
        MARATHON_INSERT_PADDING(0x24);
        bool m_VFrame;
        MARATHON_INSERT_PADDING(0x04);
        xpointer<RenderTargetContainer> m_pRenderTargetContainer;
        xpointer<SFXAgent> m_pSFXAgent;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_PauseFlags;
        MARATHON_INSERT_PADDING(0x08);
        xpointer<SoX::LinkedList<SoX::Thread>> m_pThreads;
        MARATHON_INSERT_PADDING(0x288);
        xpointer<void> m_pParticleManager;
        MARATHON_INSERT_PADDING(0x04);
        xpointer<TextBook::TextBookContext> m_pTextBookContext;
        bool m_IsLoading;
        xpointer<SoX::Thread> m_pMainThread;
        MARATHON_INSERT_PADDING(0x15C0);
        xpointer<RaderMapManager> m_pRaderMapManager;
        MARATHON_INSERT_PADDING(0x542D0);
        be<uint32_t> m_PadIDs[4];
        MARATHON_INSERT_PADDING(0x2C);
    };

    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pDocState, 0x5C);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pMyGraphicsDevice, 0x60);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pSceneParamManager, 0x64);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_InputManagers, 0x9C);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_VFrame, 0xD0);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pRenderTargetContainer, 0xD8);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pSFXAgent, 0xDC);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_PauseFlags, 0xEC);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pThreads, 0xF8);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pParticleManager, 0x384);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pTextBookContext, 0x38C);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_IsLoading, 0x390);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pMainThread, 0x394);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_pRaderMapManager, 0x1958);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_PadIDs, 0x55C2C);
    MARATHON_ASSERT_SIZEOF(DocMarathonImp, 0x55C68);
}
