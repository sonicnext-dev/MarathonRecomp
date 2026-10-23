#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <Sonicteam/Game.h>

namespace Sonicteam::Camera
{
    class CameraMode : public SoX::MessageReceiver
    {
    public:
        boost::shared_ptr<Game> m_spGame;
        xpointer<void> m_pInputListener;
        MARATHON_INSERT_PADDING(0x14);
        xpointer<SoX::Engine::Doc> m_pDoc;
    };

    MARATHON_ASSERT_OFFSETOF(CameraMode, m_spGame, 0x04);
    MARATHON_ASSERT_OFFSETOF(CameraMode, m_pInputListener, 0x0C);
    MARATHON_ASSERT_OFFSETOF(CameraMode, m_pDoc, 0x24);
    MARATHON_ASSERT_SIZEOF(CameraMode, 0x28);
}
