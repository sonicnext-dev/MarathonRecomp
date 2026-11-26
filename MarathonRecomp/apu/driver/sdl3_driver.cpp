#include "sdl3_driver.h"

#include <apu/audio.h>
#include <os/logger.h>
#include <ui/game_window.h>

static std::once_flag g_initializedOnce;
static bool g_initialized = false;

static void InitializeSDLAudio() {
#ifdef _WIN32
    // Force wasapi on Windows.
    SDL_SetHint(SDL_HINT_AUDIO_DRIVER, "wasapi");
#endif

    SDL_SetHint(SDL_HINT_AUDIO_CATEGORY, "playback");
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, "Marathon Recompiled");

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
        LOGFN_ERROR("Failed to init audio subsystem: {}", SDL_GetError());

    g_initialized = true;
}

SDL3AudioDriver::SDL3AudioDriver(bool surround)
{
    std::call_once(g_initializedOnce, InitializeSDLAudio);
    if (!g_initialized)
        return;

    SDL_AudioSpec deviceSpec{};
    if (surround && SDL_GetAudioDeviceFormat(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &deviceSpec, nullptr))
        surround = deviceSpec.channels >= XAUDIO_NUM_CHANNELS;

    m_channels = surround ? XAUDIO_NUM_CHANNELS : 2;

    SDL_AudioSpec desired;
    desired.freq = XAUDIO_SAMPLES_HZ;
    desired.format = SDL_AUDIO_F32;
    desired.channels = m_channels;
    m_pAudioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired, nullptr, nullptr);

    if (!m_pAudioStream)
    {
        LOGFN_ERROR("Failed to open audio device: {}", SDL_GetError());
        return;
    }

    if (!SDL_ResumeAudioStreamDevice(m_pAudioStream))
        LOGFN_ERROR("Failed to resume audio device: {}", SDL_GetError());
}

SDL3AudioDriver::~SDL3AudioDriver()
{
    if (m_pAudioStream)
        SDL_DestroyAudioStream(m_pAudioStream);
}

int SDL3AudioDriver::GetQueuedBytes() {
    if (!m_pAudioStream)
        return -1;

    return SDL_GetAudioStreamQueued(m_pAudioStream);
}

void SDL3AudioDriver::Put(const float *pSamples, uint32_t frameCount) {
    if (!m_pAudioStream)
        return;

    SDL_PutAudioStreamData(m_pAudioStream, pSamples, static_cast<int>(frameCount * m_channels * sizeof(float)));
}
