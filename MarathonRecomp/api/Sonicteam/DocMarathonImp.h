#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Input/Manager.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <api/stdx/vector.h>


namespace Sonicteam
{
    class DocMarathonImp : public SoX::Engine::Doc
    {
    public:
        MARATHON_INSERT_PADDING(0x40);
        stdx::vector<boost::shared_ptr<Sonicteam::SoX::Input::Manager>> m_vspInputManager;
        MARATHON_INSERT_PADDING(0x24);
        bool m_VFrame;
        MARATHON_INSERT_PADDING(0x55B58);
        be<uint32_t> m_PlayerControllerID[4];
    };
}
