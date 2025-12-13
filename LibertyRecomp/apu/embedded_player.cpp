#include <apu/audio.h>
#include <apu/embedded_player.h>
#include <user/config.h>

#include <res/music/installer.ogg.h>
#include <res/sounds/window_open.ogg.h>
#include <res/sounds/window_close.ogg.h>
#include <res/sounds/cursor2.ogg.h>
#include <res/sounds/deside.ogg.h>
#include <res/sounds/move.ogg.h>
#include <res/sounds/main_deside.ogg.h>

enum class EmbeddedSound
{
    WindowOpen,
    WindowClose,
    Cursor2,
    Deside,
    Move,
    MainDeside,
    Count,
};

struct EmbeddedSoundData
{
    Mix_Chunk* chunk{};
};

static std::array<EmbeddedSoundData, size_t(EmbeddedSound::Count)> g_embeddedSoundData = {};
static const std::unordered_map<std::string_view, EmbeddedSound> g_embeddedSoundMap =
{
    { "window_open", EmbeddedSound::WindowOpen },
    { "window_close", EmbeddedSound::WindowClose },
    { "cursor2", EmbeddedSound::Cursor2 },
    { "deside", EmbeddedSound::Deside },
    { "move", EmbeddedSound::Move },
    { "main_deside", EmbeddedSound::MainDeside },
};

static size_t g_channelIndex;

static void PlayEmbeddedSound(EmbeddedSound s)
{
    EmbeddedSoundData &data = g_embeddedSoundData[size_t(s)];
    if (data.chunk == nullptr)
    {
        // The sound hasn't been created yet, create it and pick it.
        const void *soundData = nullptr;
        size_t soundDataSize = 0;
        switch (s)
        {
        case EmbeddedSound::WindowOpen:
            soundData = g_window_open;
            soundDataSize = sizeof(g_window_open);
            break;
        case EmbeddedSound::WindowClose:
            soundData = g_window_close;
            soundDataSize = sizeof(g_window_close);
            break;
        case EmbeddedSound::Cursor2:
            soundData = g_cursor2;
            soundDataSize = sizeof(g_cursor2);
            break;
        case EmbeddedSound::Deside:
            soundData = g_deside;
            soundDataSize = sizeof(g_deside);
            break;
        case EmbeddedSound::Move:
            soundData = g_move;
            soundDataSize = sizeof(g_move);
            break;
        case EmbeddedSound::MainDeside:
            soundData = g_main_deside;
            soundDataSize = sizeof(g_main_deside);
            break;
        default:
            assert(false && "Unknown embedded sound.");
            return;
        }

        data.chunk = Mix_LoadWAV_RW(SDL_RWFromConstMem(soundData, soundDataSize), 1);
    }
    
    Mix_VolumeChunk(data.chunk, (Config::MasterVolume * Config::EffectsVolume * EmbeddedPlayer::EFFECTS_VOLUME) * MIX_MAX_VOLUME);
    Mix_PlayChannel(g_channelIndex % MIX_CHANNELS, data.chunk, 0);
    ++g_channelIndex;
}

static Mix_Music* g_installerMusic;

void EmbeddedPlayer::Init() 
{
    Mix_OpenAudio(XAUDIO_SAMPLES_HZ, AUDIO_F32SYS, 2, 4096);
    g_installerMusic = Mix_LoadMUS_RW(SDL_RWFromConstMem(g_installer_music, sizeof(g_installer_music)), 1);

    s_isActive = true;
}

void EmbeddedPlayer::Play(const char *name) 
{
    assert(s_isActive && "Playback shouldn't be requested if the Embedded Player isn't active.");

    auto it = g_embeddedSoundMap.find(name);
    if (it == g_embeddedSoundMap.end())
    {
        return;
    }

    PlayEmbeddedSound(it->second);
}

void EmbeddedPlayer::PlayMusic()
{
    if (!Mix_PlayingMusic())
    {
        Mix_PlayMusic(g_installerMusic, INT_MAX);
        Mix_VolumeMusic(Config::MasterVolume * Config::MusicVolume * MUSIC_VOLUME * MIX_MAX_VOLUME);
    }
}

void EmbeddedPlayer::FadeOutMusic()
{
    if (Mix_PlayingMusic())
        Mix_FadeOutMusic(1000);
}

void EmbeddedPlayer::Shutdown() 
{
    for (EmbeddedSoundData &data : g_embeddedSoundData)
    {
        if (data.chunk != nullptr)
            Mix_FreeChunk(data.chunk);
    }

    Mix_HaltMusic();
    Mix_FreeMusic(g_installerMusic);

    Mix_CloseAudio();
    Mix_Quit();

    s_isActive = false;
}
