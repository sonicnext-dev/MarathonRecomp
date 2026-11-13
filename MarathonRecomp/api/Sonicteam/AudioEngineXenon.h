#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Audio/IAudioEngine.h>
#include <Sonicteam/SoX/Audio/SoundBank.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/SoundBankData.h>

namespace Sonicteam
{
    class AudioEngineXenon : public SoX::Audio::IAudioEngine, public System::Singleton<AudioEngineXenon, 0x82D37AC8, System::CreateStatic<AudioEngineXenon, 0x824A5C78>>
    {
    public:
        class MySoundBank : public SoX::Audio::SoundBank
        {
        public:
            xpointer<SoundBankData> m_pSoundBankData;
            MARATHON_INSERT_PADDING(0x04);
        };

        MARATHON_INSERT_PADDING(0x08);
        be<float> m_MusicVolume;
        be<float> m_EffectsVolume;
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_OFFSETOF(AudioEngineXenon::MySoundBank, m_pSoundBankData, 0x78);
    MARATHON_ASSERT_SIZEOF(AudioEngineXenon::MySoundBank, 0x80);

    MARATHON_ASSERT_OFFSETOF(AudioEngineXenon, m_MusicVolume, 0x0C);
    MARATHON_ASSERT_OFFSETOF(AudioEngineXenon, m_EffectsVolume, 0x10);
    MARATHON_ASSERT_SIZEOF(AudioEngineXenon, 0x2C);
}
