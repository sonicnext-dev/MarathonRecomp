#include <apu/audio.h>
#include <apu/embedded_player.h>
#include <user/config.h>

#include <res/music/installer.ogg.h>
#include <res/sounds/window_open.ogg.h>
#include <res/sounds/window_close.ogg.h>
#include <res/sounds/cursor.ogg.h>
#include <res/sounds/deside.ogg.h>
#include <res/sounds/move.ogg.h>
#include <res/sounds/main_deside.ogg.h>
#include <res/sounds/cannot_deside.ogg.h>

enum class EmbeddedSound
{
    WindowOpen,
    WindowClose,
    Cursor,
    Deside,
    Move,
    MainDeside,
    CannotDeside,
    Count
};

struct EmbeddedSoundData
{
    MIX_Audio* audio{};
};

static constexpr size_t EMBEDDED_TRACK_COUNT = 8;

static MIX_Mixer* g_mixer;
static std::array<MIX_Track*, EMBEDDED_TRACK_COUNT> g_tracks = {};

static std::array<EmbeddedSoundData, size_t(EmbeddedSound::Count)> g_embeddedSoundData = {};
static const std::unordered_map<std::string_view, EmbeddedSound> g_embeddedSoundMap =
{
    { "window_open", EmbeddedSound::WindowOpen },
    { "window_close", EmbeddedSound::WindowClose },
    { "cursor", EmbeddedSound::Cursor },
    { "deside", EmbeddedSound::Deside },
    { "move", EmbeddedSound::Move },
    { "main_deside", EmbeddedSound::MainDeside },
    { "cannot_deside", EmbeddedSound::CannotDeside },
};

static size_t g_trackIndex;

static void PlayEmbeddedSound(EmbeddedSound s)
{
    EmbeddedSoundData &data = g_embeddedSoundData[size_t(s)];
    if (data.audio == nullptr)
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
        case EmbeddedSound::Cursor:
            soundData = g_cursor;
            soundDataSize = sizeof(g_cursor);
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
        case EmbeddedSound::CannotDeside:
            soundData = g_cannot_deside;
            soundDataSize = sizeof(g_cannot_deside);
            break;
        default:
            assert(false && "Unknown embedded sound.");
            return;
        }

        data.audio = MIX_LoadAudio_IO(g_mixer, SDL_IOFromConstMem(soundData, soundDataSize), true, true);
    }

    MIX_Track *track = g_tracks[g_trackIndex % EMBEDDED_TRACK_COUNT];
    ++g_trackIndex;

    MIX_SetTrackGain(track, Config::MasterVolume * Config::EffectsVolume * EmbeddedPlayer::EFFECTS_VOLUME);
    MIX_SetTrackAudio(track, data.audio);
    MIX_PlayTrack(track, 0);
}

static MIX_Audio* g_installerMusic;
static MIX_Track* g_musicTrack;

void EmbeddedPlayer::Init()
{
    MIX_Init();

    SDL_AudioSpec spec{};
    spec.freq = XAUDIO_SAMPLES_HZ;
    spec.format = SDL_AUDIO_F32;
    spec.channels = 2;
    g_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);

    for (MIX_Track *&track : g_tracks)
        track = MIX_CreateTrack(g_mixer);

    g_installerMusic = MIX_LoadAudio_IO(g_mixer, SDL_IOFromConstMem(g_installer_music, sizeof(g_installer_music)), false, true);
    g_musicTrack = MIX_CreateTrack(g_mixer);
    MIX_SetTrackAudio(g_musicTrack, g_installerMusic);

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
    if (!MIX_TrackPlaying(g_musicTrack))
    {
        SDL_PropertiesID options = SDL_CreateProperties();
        SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);

        MIX_SetTrackGain(g_musicTrack, Config::MasterVolume * Config::MusicVolume * MUSIC_VOLUME);
        MIX_PlayTrack(g_musicTrack, options);

        SDL_DestroyProperties(options);
    }
}

void EmbeddedPlayer::FadeOutMusic()
{
    if (MIX_TrackPlaying(g_musicTrack))
        MIX_StopTrack(g_musicTrack, MIX_TrackMSToFrames(g_musicTrack, 1000));
}

void EmbeddedPlayer::Shutdown()
{
    MIX_DestroyMixer(g_mixer);

    for (EmbeddedSoundData &data : g_embeddedSoundData)
    {
        if (data.audio != nullptr)
            MIX_DestroyAudio(data.audio);
    }

    MIX_DestroyAudio(g_installerMusic);

    MIX_Quit();

    s_isActive = false;
}
