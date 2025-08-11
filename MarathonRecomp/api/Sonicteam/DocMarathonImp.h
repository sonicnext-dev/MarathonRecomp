#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>

namespace Sonicteam
{
	
    struct DocSaveFile
    {
    public:
        enum EPISODE
        {
            EPISODE_SONIC,
            EPISODE_SHADOW,
            EPISODE_SILVER
            //...
        };
        static const uint32_t EPISODE_COUNT = 32;
        static const uint32_t TRIAL_COUNT = 512;
        static const uint32_t GLOBALFLAG_COUNT = 0x2800;
        static const uint32_t STRING_SIZE = 0x100;
        static const uint32_t STRING_SIZE_SHORT = 0x42;

        MARATHON_INSERT_PADDING(4);
        be<uint32_t> m_Episode_Lives[EPISODE_COUNT];
        be<uint32_t> m_Episode_Rings[EPISODE_COUNT];
        be<uint32_t> m_GlobalFlags[GLOBALFLAG_COUNT];
        char m_Episode_Lua[EPISODE_COUNT * STRING_SIZE];
        MARATHON_INSERT_PADDING(EPISODE_COUNT * 0x80);
        char m_Episode_Objective[EPISODE_COUNT * STRING_SIZE];
        char m_Episode_Area[EPISODE_COUNT * STRING_SIZE];
        char m_Episode_Terrain[EPISODE_COUNT * STRING_SIZE];
        char m_Episode_SET[EPISODE_COUNT * STRING_SIZE];
        char m_Episode_PATH[EPISODE_COUNT * STRING_SIZE];
        MARATHON_INSERT_PADDING(EPISODE_COUNT * 0x500);
        char m_Episode_MST[EPISODE_COUNT * STRING_SIZE];
        MARATHON_INSERT_PADDING(EPISODE_COUNT * 0x24C);
        be<uint32_t> m_Episode_Progress[EPISODE_COUNT];
        be<uint16_t> m_Episode_Year[EPISODE_COUNT];
        uint8_t m_Episode_Month[EPISODE_COUNT];
        uint8_t m_Episode_Day[EPISODE_COUNT];
        uint8_t m_Episode_Hour[EPISODE_COUNT];
        uint8_t m_Episode_Minute[EPISODE_COUNT];
        char m_Episode_Location[EPISODE_COUNT * STRING_SIZE_SHORT];
        be<uint32_t> m_Trial_ID[TRIAL_COUNT];
        be<uint32_t> m_Trial_Rank[TRIAL_COUNT];
        be<uint32_t> m_Trial_Time[TRIAL_COUNT];
        be<uint32_t> m_Trial_Score[TRIAL_COUNT];
        be<uint32_t> m_Trial_Rings[TRIAL_COUNT];
        bool m_Option_Subtitles;
        be<float> m_Option_Music;
        be<float> m_Option_Effects;
    };

    class DocMarathonImp : public SoX::Engine::Doc
    {
    public:
        MARATHON_INSERT_PADDING(0x74);
        bool m_VFrame;
        MARATHON_INSERT_PADDING(0X2E8);
        DocSaveFile m_SaveIn;
        DocSaveFile m_SaveOut;
    };
}   
