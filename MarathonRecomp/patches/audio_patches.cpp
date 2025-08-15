#include <api/Marathon.h>
#include <kernel/function.h>
#include <os/media.h>
#include <os/version.h>
#include <patches/audio_patches.h>
#include <user/config.h>

int AudioPatches::m_isAttenuationSupported = -1;

bool AudioPatches::CanAttenuate()
{
#if _WIN32
    if (m_isAttenuationSupported >= 0)
        return m_isAttenuationSupported;

    auto version = os::version::GetOSVersion();

    m_isAttenuationSupported = version.Major >= 10 && version.Build >= 17763;

    return m_isAttenuationSupported;
#elif __linux__
    return true;
#else
    return false;
#endif
}

void AudioPatches::Update(float deltaTime)
{
    auto pAudioEngine = Sonicteam::AudioEngineXenon::GetInstance();

    if (!pAudioEngine)
        return;

    const float musicVolume = Config::MusicVolume * Config::MasterVolume;
    if (Config::MusicAttenuation && CanAttenuate())
    {
        auto time = 1.0f - expf(2.5f * -deltaTime);

        if (os::media::IsExternalMediaPlaying())
        {
            pAudioEngine->m_MusicVolume = std::lerp(pAudioEngine->m_MusicVolume, 0.0f, time);
        }
        else
        {
            pAudioEngine->m_MusicVolume = std::lerp(pAudioEngine->m_MusicVolume, musicVolume, time);
        }
    }
    else
    {
        pAudioEngine->m_MusicVolume = musicVolume;
    }
}
