#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Audio/IAudioEngine.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam
{
    class AudioEngineXenon : public SoX::Audio::IAudioEngine, public System::Singleton<AudioEngineXenon, 0x82D37AC8, System::CreateStatic<AudioEngineXenon, 0x824A5C78>>
    {
    public:
        MARATHON_INSERT_PADDING(8);
        be<float> m_MusicVolume;
        be<float> m_EffectsVolume;
        MARATHON_INSERT_PADDING(0x14);
    };
}   
