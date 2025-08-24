#include <api/Marathon.h>
#include <kernel/function.h>
#include <os/media.h>
#include <os/version.h>
#include <patches/audio_patches.h>
#include <user/config.h>
#include <app.h>

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

// Update function for CRI cues.
// This hook fixes jingles fading the BGM back in prematurely.
PPC_FUNC_IMPL(__imp__sub_8260F168);
PPC_FUNC(sub_8260F168)
{
    struct CueParams
    {
        be<float> Duration;
        be<float> FrameTime;
        be<float> Field08;
        be<float> Field0C;
        be<float> MusicVolume;
        bool Field14;
    };

    auto pParams = (CueParams*)(base + ctx.r3.u32);

    pParams->FrameTime = App::s_deltaTime;

    __imp__sub_8260F168(ctx, base);
}

void CriCueUpdateDeltaTimeFix(PPCRegister& deltaTime)
{
    deltaTime.f64 = App::s_deltaTime;
}

void PowerUpJingleDurationFix(PPCRegister& duration)
{
    if (!Config::FixPowerUpJingleDuration)
        return;

    duration.f64 = 20.0;
}

void XmvPlayerLang(PPCRegister& r11)
{
    r11.u32 = 1;

    if (Config::VoiceLanguage == EVoiceLanguage::Japanese)
        r11.u32++;
}

void CsbSbkLang(PPCRegister& r8)
{
    r8.u32 = Config::VoiceLanguage == EVoiceLanguage::Japanese ? 0 : 1;
}

void MovieVoiceLang(PPCRegister& r19)
{
    r19.u32 = Config::VoiceLanguage == EVoiceLanguage::Japanese ? 0x80000000 : 0x40000000;
}
