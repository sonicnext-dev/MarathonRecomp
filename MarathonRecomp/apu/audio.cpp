#include "audio.h"
#include "driver/sdl3_driver.h"

#include <cpu/guest_thread.h>
#include <kernel/heap.h>
#include <kernel/memory.h>
#include <ui/game_window.h>
#include <user/config.h>
#include <utils/host_thread.h>

#ifdef __APPLE__
#include "driver/avfoundation_driver.h"
#endif

#define AUDIO_DRIVER_KEY (uint32_t)('DAUD')

// Use to dump raw audio captures to the game folder.
//#define AUDIO_DUMP_SAMPLES_PATH "audio.pcm"

#ifdef AUDIO_DUMP_SAMPLES_PATH
std::ofstream g_audioDumpStream;
#endif

static std::unique_ptr<AudioDriver> g_driver;

static std::unique_ptr<HostThread> g_audioThread;
static volatile bool g_audioThreadShouldExit;

static PPCFunc* g_clientCallback{};
static uint32_t g_clientCallbackParam{}; // pointer in guest memory

static void AudioThread()
{
    using namespace std::chrono_literals;

    GuestThreadContext ctx(0);

    while (!g_audioThreadShouldExit)
    {
        int queuedAudioSize = g_driver->GetQueuedBytes();

        constexpr size_t MAX_LATENCY = 10;
        const size_t callbackAudioSize = g_driver->m_channels * XAUDIO_NUM_SAMPLES * sizeof(float);

        if (queuedAudioSize >= 0)
        {
            for (size_t i = queuedAudioSize / callbackAudioSize; i <= MAX_LATENCY; i++)
            {
                ctx.ppcContext.r3.u32 = g_clientCallbackParam;
                g_clientCallback(ctx.ppcContext, g_memory.base);
            }
        }

        auto now = std::chrono::steady_clock::now();
        constexpr auto INTERVAL = 1000000000ns * XAUDIO_NUM_SAMPLES / XAUDIO_SAMPLES_HZ;
        auto next = now + (INTERVAL - now.time_since_epoch() % INTERVAL);

        std::this_thread::sleep_for(std::chrono::floor<std::chrono::milliseconds>(next - now));

        while (std::chrono::steady_clock::now() < next)
            std::this_thread::yield();
    }
}

static void CreateAudioThread()
{
    g_audioThreadShouldExit = false;
    g_audioThread = std::make_unique<HostThread>("Audio Thread", AudioThread);
}

static void CreateAudioDriver()
{
    bool surround = Config::ChannelConfiguration == EChannelConfiguration::Surround;

#ifdef __APPLE__
    if (surround)
    {
        // Using AVFoundation allows us to support Spatial Audio
        g_driver = std::make_unique<AVFoundationDriver>(surround);
        return;
    }
#endif

    g_driver = std::make_unique<SDL3AudioDriver>(surround);
}

void XAudioInitializeSystem()
{
    CreateAudioDriver();
}

void XAudioConfigValueChangedCallback(IConfigDef* configDef)
{
    if (!g_audioThread)
        return;

    if (configDef == &Config::ChannelConfiguration)
    {
        g_audioThreadShouldExit = true;
        g_audioThread->Join();

        CreateAudioDriver();
        CreateAudioThread();
    }
}

uint32_t XAudioRegisterRenderDriverClient(be<uint32_t>* callback, be<uint32_t>* driver)
{
#ifdef AUDIO_DUMP_SAMPLES_PATH
    g_audioDumpStream.open(AUDIO_DUMP_SAMPLES_PATH, std::ios::binary);
#endif

    auto* pClientParam = static_cast<be<uint32_t>*>(g_userHeap.Alloc(sizeof(be<uint32_t>)));
    *pClientParam = callback[1];
    g_clientCallbackParam = g_memory.MapVirtual(pClientParam);
    g_clientCallback = g_memory.FindFunction(*callback);

    CreateAudioThread();

    *driver = AUDIO_DRIVER_KEY;
    return 0;
}

uint32_t XAudioUnregisterRenderDriverClient(uint32_t driver)
{
    if (g_audioThread)
    {
        g_audioThreadShouldExit = true;
        g_audioThread->Join();
        g_audioThread.reset();
    }

    if (g_clientCallbackParam)
    {
        g_userHeap.Free(g_memory.Translate(g_clientCallbackParam));
        g_clientCallbackParam = 0;
    }

    g_clientCallback = nullptr;
    return 0;
}

uint32_t XAudioSubmitRenderDriverFrame(uint32_t driver, void* samples)
{
#ifdef AUDIO_DUMP_SAMPLES_PATH
    static uint32_t xaudioSamplesBuffer[XAUDIO_NUM_SAMPLES * XAUDIO_NUM_CHANNELS];
    for (size_t i = 0; i < XAUDIO_NUM_SAMPLES; i++)
    {
        for (size_t j = 0; j < XAUDIO_NUM_CHANNELS; j++)
        {
            xaudioSamplesBuffer[i * XAUDIO_NUM_CHANNELS + j] = ByteSwap(((uint32_t *)samples)[j * XAUDIO_NUM_SAMPLES + i]);
        }
    }

    g_audioDumpStream.write((const char *)(xaudioSamplesBuffer), sizeof(xaudioSamplesBuffer));
#endif

    auto floatSamples = reinterpret_cast<be<float>*>(samples);
    auto volume = Config::MasterVolume.Value;

    if (Config::MuteOnFocusLost && !GameWindow::s_isFocused)
        volume = 0.0f;

    if (g_driver->m_channels == 2)
    {
        // 0: left 1.0f, right 0.0f
        // 1: left 0.0f, right 1.0f
        // 2: left 0.75f, right 0.75f
        // 3: left 0.0f, right 0.0f
        // 4: left 1.0f, right 0.0f
        // 5: left 0.0f, right 1.0f

        std::array<float, 2 * XAUDIO_NUM_SAMPLES> audioFrames;

        for (size_t i = 0; i < XAUDIO_NUM_SAMPLES; i++)
        {
            float ch0 = floatSamples[0 * XAUDIO_NUM_SAMPLES + i];
            float ch1 = floatSamples[1 * XAUDIO_NUM_SAMPLES + i];
            float ch2 = floatSamples[2 * XAUDIO_NUM_SAMPLES + i];
            float ch3 = floatSamples[3 * XAUDIO_NUM_SAMPLES + i];
            float ch4 = floatSamples[4 * XAUDIO_NUM_SAMPLES + i];
            float ch5 = floatSamples[5 * XAUDIO_NUM_SAMPLES + i];

            float samp0 = (ch0 + ch2 * 0.75f + ch4) * volume;
            float samp1 = (ch1 + ch2 * 0.75f + ch5) * volume;

            audioFrames[i * 2 + 0] = isnan(samp0) ? 0.0f : samp0;
            audioFrames[i * 2 + 1] = isnan(samp1) ? 0.0f : samp1;
        }

        g_driver->Put(audioFrames.data(), XAUDIO_NUM_SAMPLES);
    }
    else
    {
        std::array<float, XAUDIO_NUM_CHANNELS * XAUDIO_NUM_SAMPLES> audioFrames;

        for (size_t i = 0; i < XAUDIO_NUM_SAMPLES; i++)
        {
            for (size_t j = 0; j < XAUDIO_NUM_CHANNELS; j++)
            {
                float samp = floatSamples[j * XAUDIO_NUM_SAMPLES + i] * volume;
                audioFrames[i * XAUDIO_NUM_CHANNELS + j] = isnan(samp) ? 0.0f : samp;
            }
        }

        g_driver->Put(audioFrames.data(), XAUDIO_NUM_SAMPLES);
    }

    return 0;
}
