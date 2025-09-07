#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/Object.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam
{
    class MainMenuTask : public SoX::Engine::Task
    {
    public:
        enum MainMenuState : uint32_t
        {
            MainMenuState_MainMenuBack = 1,
            MainMenuState_MainMenu = 2,
            MainMenuState_MainMenuExitPrompt = 4,
            MainMenuState_SinglePlayer = 6,
            MainMenuState_EpisodeSelect = 9,
            MainMenuState_TrialSelect = 0x0D,
            MainMenuState_SelectCharacter = 0x0F,
            MainMenuState_ActTrial = 0x11,
            MainMenuState_TownTrial = 0x15,
            MainMenuState_Multiplayer = 0x17,
            MainMenuState_Extras = 0x1C,
            MainMenuState_Tag = 0x1E,
            MainMenuState_Battle = 0x22,
            MainMenuState_GoldMedalResults = 0x26,
            MainMenuState_AudioRoom = 0x2F,
            MainMenuState_TheaterRoom = 0x31,
            MainMenuState_Options = 0x33,
            MainMenuState_MainMenuExit = 0x3C
        };

        be<uint32_t> m_State;
        MARATHON_INSERT_PADDING(0x248);
        xpointer<Actor> m_Field298;
        xpointer<Player::Object> m_SelectCharacters[9];
    };
}
