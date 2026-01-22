#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/Player/Object.h>
#include <Sonicteam/ButtonWindowTask.h>
#include <Sonicteam/MainMenuExpositionTask.h>

namespace Sonicteam
{
    class MainMenuTask : public SoX::Engine::Task
    {
    public:
        enum MainMenuState : uint32_t
        {
            MainMenuState_MainMenuBack = 1,
            MainMenuState_MainMenu = 2,
            MainMenuState_ExitPrompt = 4,
            MainMenuState_SinglePlayer = 6,
            MainMenuState_EpisodeSelect = 9,
            MainMenuState_TrialSelect = 0x0D,
            MainMenuState_SelectCharacter = 0x0F,
            MainMenuState_ActTrial = 0x11,
            MainMenuState_TownTrial = 0x15,
            MainMenuState_Multiplayer = 0x17,
            MainMenuState_Extras = 0x1C,
            MainMenuState_Tag = 0x1E,
            MainMenuState_Tag1PSelect = 0x1F,
            MainMenuState_Battle = 0x22,
            MainMenuState_GoldMedalResultsOpen = 0x26,
            MainMenuState_GoldMedalResults = 0x27,
            MainMenuState_AudioRoom = 0x2F,
            MainMenuState_TheaterRoom = 0x31,
            MainMenuState_Options = 0x33,
            MainMenuState_ExitToStage = 0x3B,
            MainMenuState_ExitToTitle = 0x3C
        };

        be<uint32_t> m_State;
        MARATHON_INSERT_PADDING(0x24);
        xpointer<HUDMainMenu> m_pHUDMainMenu;
        MARATHON_INSERT_PADDING(8);
        xpointer<HUDGoldMedal> m_pHUDGoldMedal;
        MARATHON_INSERT_PADDING(0x14);
        xpointer<ButtonWindowTask> m_pButtonWindowTask;
        SoX::RefSharedPointer<MainMenuExpositionTask> m_spMainMenuExpositionTask;
        be<uint32_t> m_MainMenuSelectedIndex;
        be<uint32_t> m_SinglePlayerSelectedIndex;
        MARATHON_INSERT_PADDING(0x14);
        be<uint32_t> m_FieldBC;
        MARATHON_INSERT_PADDING(0x60);
        be<uint32_t> m_GoldMedalEpisodeIndex;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_GoldMedalScrollIndex;
        MARATHON_INSERT_PADDING(0x144);
        be<uint32_t> m_IsChangingState;
        MARATHON_INSERT_PADDING(8);
        be<uint32_t> m_PressedButtons;
        MARATHON_INSERT_PADDING(0x18);
        xpointer<Actor> m_Field298;
        xpointer<Player::Object> m_apSelectCharacters[9];
        MARATHON_INSERT_PADDING(0x38);
    };

    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_State, 0x4C);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_pHUDMainMenu, 0x74);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_pHUDGoldMedal, 0x80);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_pButtonWindowTask, 0x98);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_spMainMenuExpositionTask, 0x9C);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_MainMenuSelectedIndex, 0xA0);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_SinglePlayerSelectedIndex, 0xA4);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_FieldBC, 0xBC);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_GoldMedalEpisodeIndex, 0x120);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_GoldMedalScrollIndex, 0x128);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_IsChangingState, 0x270);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_PressedButtons, 0x27C);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_Field298, 0x298);
    MARATHON_ASSERT_OFFSETOF(MainMenuTask, m_apSelectCharacters, 0x29C);
    MARATHON_ASSERT_SIZEOF(MainMenuTask, 0x2F8);
}
