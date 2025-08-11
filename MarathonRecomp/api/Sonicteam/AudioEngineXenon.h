#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Audio/IAudioEngine.h>
#include <kernel/memory.h>

namespace Sonicteam
{
	
    class AudioEngineXenon : public SoX::Audio::IAudioEngine
    {
    public:
        MARATHON_INSERT_PADDING(8);
        be<float> m_Audio;
        be<float> m_SoundEffects;
        MARATHON_INSERT_PADDING(0x14);

        //Singleton<T,C<T>>
        static AudioEngineXenon* GetAudioEngine()
        {
            auto ppAudio = (xpointer<AudioEngineXenon>*)g_memory.Translate(0x82D37AC8);
            if (!ppAudio->ptr.get())
            {
                auto pAudio = GuestToHostFunction<AudioEngineXenon*>(sub_824A5C78);
                *ppAudio = pAudio;
            }
            return *ppAudio;
        };
    };
}   
