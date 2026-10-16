#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/FrameGP.h>
#include <Sonicteam/SoX/Scenery/Camera.h>

namespace Sonicteam
{
    class MainMode : public SoX::Engine::DocMode
    {
    public:
        enum MainModeState : uint32_t
        {
            MainModeState_SEGALogo = 1,
            MainModeState_SonicTeamLogo,
            MainModeState_CriWareLogo,
            MainModeState_Title,
            MainModeState_LoadSavedData,
            MainModeState_MainMenu,
            MainModeState_LoadArea,
            MainModeState_AdvertiseMovie,
            MainModeState_AdvertiseMovieNext,
            MainModeState_GameShowCharacterSelect,
            MainModeState_ComingSoon,
            MainModeState_SaveSavedData
        };

        be<MainModeState> m_State;
        MARATHON_INSERT_PADDING(0x20);
        boost::shared_ptr<SoX::Scenery::Camera> m_spSelectCamera;
        xpointer<SoX::Graphics::FrameGP> m_pFrameGP;
        MARATHON_INSERT_PADDING(4);
        xpointer<Task> m_pMainTask;
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(MainMode, m_State, 0x50);
    MARATHON_ASSERT_OFFSETOF(MainMode, m_spSelectCamera, 0x74);
    MARATHON_ASSERT_OFFSETOF(MainMode, m_pFrameGP, 0x7C);
    MARATHON_ASSERT_OFFSETOF(MainMode, m_pMainTask, 0x84);
    MARATHON_ASSERT_SIZEOF(MainMode, 0x8C);
}
