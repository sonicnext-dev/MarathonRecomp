#pragma once

#include <Marathon.inl>

namespace Sonicteam
{
    class GameImp : public SoX::MessageReceiver
    {
    public:
        enum GameState : uint32_t
        {
            eGameState_MainMenu,
            eGameState_Stage,
            eGameState_Event,
            eGameState_Movie,
            eGameState_Result,
            eGameState_Message,
            eGameState_6,
            eGameState_Save,
            eGameState_ReturnToMainMenu
        };

        struct PlayerData
        {
            be<uint32_t> ActorID;
            be<uint32_t> RingCount;
            MARATHON_INSERT_PADDING(4);
            be<uint32_t> LifeCount;
            be<uint32_t> ScoreCount;
            be<float> AliveTime;
            be<float> Time;
            MARATHON_INSERT_PADDING(4);
            be<float> SectionSaveTime;
            be<float> GaugeValue;
            be<uint32_t> MaturityLevel;
            be<float> MaturityValue;
            MARATHON_INSERT_PADDING(4);
            be<uint32_t> ExtendRingCount;
            be<uint32_t> GemIndex;
            MARATHON_INSERT_PADDING(0x10);
        };

        MARATHON_INSERT_PADDING(4);
        be<GameState> m_GameState;
        xpointer<DocMarathonState> m_pDoc;
        be<uint32_t> m_Flags;
        MARATHON_INSERT_PADDING(0xE2C);
        PlayerData m_PlayerData[4];
    };
}
