#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/MessageReceiver.h>
#include <Sonicteam/GameImp.h>

namespace Sonicteam::Camera
{
    class CameraMode : public SoX::MessageReceiver
    {
    public:
        boost::shared_ptr<Game> m_spGame;
        xpointer<void> m_pCameraInputListener;
        MARATHON_INSERT_PADDING(0x18);

        GameImp* GetGame() const
        {
            return (GameImp*)m_spGame.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(CameraMode, m_spGame, 0x04);
    MARATHON_ASSERT_OFFSETOF(CameraMode, m_pCameraInputListener, 0x0C);
    MARATHON_ASSERT_SIZEOF(CameraMode, 0x28);
}
