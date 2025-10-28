#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/MyGraphicsDevice.h>
#include <Sonicteam/SoX/Input/Manager.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <api/stdx/vector.h>
#include <Sonicteam/RenderTargetContainer.h>

namespace Sonicteam
{
    class DocMarathonImp : public SoX::Engine::Doc
    {
    public:
        MARATHON_INSERT_PADDING(0x4);
        xpointer<MyGraphicsDevice> m_pMyGraphicDevice;
        MARATHON_INSERT_PADDING(0x38);
        stdx::vector<boost::shared_ptr<SoX::Input::Manager>> m_vspInputManager;
        MARATHON_INSERT_PADDING(0x24);
        bool m_VFrame;
        MARATHON_INSERT_PADDING(0x4);
        xpointer<RenderTargetContainer> m_pRenderTargetContainer;
        MARATHON_INSERT_PADDING(0x55B50);
        be<uint32_t> m_aPadIDs[4];
    };

    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_vspInputManager, 0x9C);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_VFrame, 0xD0);
    MARATHON_ASSERT_OFFSETOF(DocMarathonImp, m_aPadIDs, 0x55C2C);
    MARATHON_ASSERT_SIZEOF(DocMarathonImp, 0x55C68);
}
