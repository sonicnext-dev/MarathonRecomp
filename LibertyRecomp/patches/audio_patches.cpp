// GTA IV Audio Patches
// Handles audio volume, attenuation, and RAGE audio engine integration

#include <stdafx.h>
#include <api/Liberty.h>
#include <kernel/function.h>
#include <kernel/memory.h>
#include <os/media.h>
#include <os/version.h>
#include <os/logger.h>
#include <patches/audio_patches.h>
#include <user/config.h>
#include <app.h>

int AudioPatches::m_isAttenuationSupported = -1;

// GTA IV Audio Engine State
// These pointers will be resolved at runtime when we find the audio engine in memory
namespace GTA4Audio
{
    // Volume levels (0.0 - 1.0)
    static float g_musicVolume = 1.0f;
    static float g_effectsVolume = 1.0f;
    static float g_masterVolume = 1.0f;
    
    // Audio engine state
    static bool g_audioInitialized = false;
    static uint32_t g_audioEnginePtr = 0;  // Guest pointer to RAGE audEngine
    
    float GetMusicVolume() { return g_musicVolume; }
    void SetMusicVolume(float volume) { g_musicVolume = std::clamp(volume, 0.0f, 1.0f); }
    
    float GetEffectsVolume() { return g_effectsVolume; }
    void SetEffectsVolume(float volume) { g_effectsVolume = std::clamp(volume, 0.0f, 1.0f); }
    
    float GetMasterVolume() { return g_masterVolume; }
    void SetMasterVolume(float volume) { g_masterVolume = std::clamp(volume, 0.0f, 1.0f); }
    
    bool IsInitialized() { return g_audioInitialized; }
    void SetInitialized(bool init) { g_audioInitialized = init; }
}

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
#elif __APPLE__
    // macOS supports audio attenuation
    return true;
#else
    return false;
#endif
}

void AudioPatches::Update(float deltaTime)
{
    // Update GTA IV audio volumes from config
    const float musicVolume = Config::MusicVolume * Config::MasterVolume;
    const float effectsVolume = Config::EffectsVolume * Config::MasterVolume;

    if (Config::MusicAttenuation && CanAttenuate())
    {
        auto time = 1.0f - expf(2.5f * -deltaTime);

        if (os::media::IsExternalMediaPlaying())
        {
            // Fade out music when external media is playing
            GTA4Audio::SetMusicVolume(std::lerp(GTA4Audio::GetMusicVolume(), 0.0f, time));
        }
        else
        {
            // Fade music back in
            GTA4Audio::SetMusicVolume(std::lerp(GTA4Audio::GetMusicVolume(), musicVolume, time));
        }
    }
    else
    {
        GTA4Audio::SetMusicVolume(musicVolume);
    }

    GTA4Audio::SetEffectsVolume(effectsVolume);
    GTA4Audio::SetMasterVolume(Config::MasterVolume);
}

// =============================================================================
// GTA IV Audio System Stubs and Hooks
// =============================================================================

// GTA IV uses RAGE's audEngine for audio management.
// These hooks intercept audio-related calls to provide proper volume control.

// Hook for audio engine initialization
// Address TBD - needs to be found in GTA IV code
void GTA4_AudioEngineInit(PPCRegister& result)
{
    GTA4Audio::SetInitialized(true);
    LOGN("GTA4 Audio: Engine initialized");
    result.u32 = 1;  // Success
}

// Hook for setting music volume
// This would be called by the game's options menu
void GTA4_SetMusicVolume(PPCRegister& volume)
{
    float vol = volume.f32;
    GTA4Audio::SetMusicVolume(vol);
    Config::MusicVolume = vol;
}

// Hook for setting effects volume
void GTA4_SetEffectsVolume(PPCRegister& volume)
{
    float vol = volume.f32;
    GTA4Audio::SetEffectsVolume(vol);
    Config::EffectsVolume = vol;
}

// Hook for getting current music volume (for UI display)
void GTA4_GetMusicVolume(PPCRegister& result)
{
    result.f32 = GTA4Audio::GetMusicVolume();
}

// Hook for getting current effects volume (for UI display)
void GTA4_GetEffectsVolume(PPCRegister& result)
{
    result.f32 = GTA4Audio::GetEffectsVolume();
}

// =============================================================================
// XMA Audio Playback Hooks
// =============================================================================
// GTA IV uses XMA compressed audio for music and sound effects.
// The XMA decoder is already implemented in apu/xma_decoder.cpp

// Stub for audio stream creation - returns success
// TODO: Find actual GTA IV address for audio stream creation
uint32_t GTA4_CreateAudioStream(PPCContext& ctx, uint8_t* base)
{
    // Return success - actual implementation in xma_decoder
    ctx.r3.u32 = 0;  // S_OK
    return ctx.r3.u32;
}

// =============================================================================
// Radio System (GTA IV specific)
// =============================================================================
// GTA IV has an in-game radio system with multiple stations.
// Volume should respect both radio volume and music volume settings.

namespace GTA4Radio
{
    static int g_currentStation = 0;
    static float g_radioVolume = 1.0f;
    static bool g_radioEnabled = true;
    
    int GetCurrentStation() { return g_currentStation; }
    void SetCurrentStation(int station) { g_currentStation = station; }
    
    float GetRadioVolume() { return g_radioVolume * GTA4Audio::GetMusicVolume(); }
    void SetRadioVolume(float volume) { g_radioVolume = std::clamp(volume, 0.0f, 1.0f); }
    
    bool IsRadioEnabled() { return g_radioEnabled; }
    void SetRadioEnabled(bool enabled) { g_radioEnabled = enabled; }
}

// Hook for radio station change
void GTA4_SetRadioStation(PPCRegister& station)
{
    int stationId = station.s32;
    GTA4Radio::SetCurrentStation(stationId);
    // Station change logged - LOGN doesn't support format args
}

// Hook for radio volume
void GTA4_SetRadioVolume(PPCRegister& volume)
{
    float vol = volume.f32;
    GTA4Radio::SetRadioVolume(vol);
}

// =============================================================================
// Audio Configuration Callback
// =============================================================================
// Called when audio-related config values change

void AudioConfigChanged()
{
    // Sync config values to audio system
    GTA4Audio::SetMasterVolume(Config::MasterVolume);
    GTA4Audio::SetMusicVolume(Config::MusicVolume);
    GTA4Audio::SetEffectsVolume(Config::EffectsVolume);
}
