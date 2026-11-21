#include <kernel/function.h>
#include <kernel/heap.h>
#include <kernel/io/file_system.h>
#include <kernel/memory.h>
#include <app.h>

extern "C"
{
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

static constexpr uint32_t XMV_S_OK = 0;
static constexpr uint32_t XMV_S_FALSE = 1;
static constexpr uint32_t XMV_E_FAIL = 0x80004005;

// CAL's audio renderer always creates a 16-bit stereo 44.1 kHz PCM voice.
static constexpr uint32_t AUDIO_SAMPLE_RATE = 44100;
static constexpr uint32_t AUDIO_CHANNELS = 2;
static constexpr uint32_t AUDIO_PACKET_FRAMES = 1024;
static constexpr uint32_t AUDIO_PACKET_COUNT = 16;
static constexpr size_t MAX_QUEUED_AUDIO_SAMPLES = AUDIO_SAMPLE_RATE * AUDIO_CHANNELS;

struct XmvPlayer
{
    xpointer<void> Vftable;
};

struct XmvCreateParameters
{
    MARATHON_INSERT_PADDING(0x04);
    be<uint32_t> Flags;
    MARATHON_INSERT_PADDING(0x10);
    be<int32_t> AudioStreamId;
    MARATHON_INSERT_PADDING(0x04);
    xpointer<const char> FileName;
};

MARATHON_ASSERT_OFFSETOF(XmvCreateParameters, FileName, 0x20);

struct XmvVideoDescriptor
{
    be<uint32_t> Width;
    be<uint32_t> Height;
    be<float> FrameRate;
};

struct XmvVideoFrame
{
    MARATHON_INSERT_PADDING(0x0C);
    xpointer<uint8_t> Planes[3];
    be<uint32_t> PlaneSizes[3];
    be<uint32_t> Pitches[3];
};

MARATHON_ASSERT_SIZEOF(XmvVideoFrame, 0x30);

struct XAudioPacket
{
    xpointer<void> pBuffer;
    be<uint32_t> BufferSize;
    MARATHON_INSERT_PADDING(0x4C);
    xpointer<void> pContext;
};

MARATHON_ASSERT_SIZEOF(XAudioPacket, 0x58);

struct CalAudioRendererParameters
{
    be<uint32_t> Unknown00;
    be<uint32_t> Category;
    be<uint32_t> QueueLength;
    xpointer<void> pEffectChain;
};

enum class XmvPlayerState : uint32_t
{
    Buffering = 2,
    Playing = 3,
    Stopped = 7
};

static void AddBusyPackets(be<uint32_t>& value, int32_t delta)
{
    const std::atomic_ref ref(*reinterpret_cast<uint32_t*>(&value));
    auto expected = ref.load();

    while (!ref.compare_exchange_weak(expected, __builtin_bswap32(__builtin_bswap32(expected) + delta)))
        ;
}

static AVCodecContext* OpenCodec(const AVStream* pStream)
{
    auto pDecoder = avcodec_find_decoder(pStream->codecpar->codec_id);

    if (!pDecoder)
        return nullptr;

    auto pCodec = avcodec_alloc_context3(pDecoder);

    if (avcodec_parameters_to_context(pCodec, pStream->codecpar) < 0 || avcodec_open2(pCodec, pDecoder, nullptr) < 0)
    {
        avcodec_free_context(&pCodec);
        return nullptr;
    }

    return pCodec;
}

static void CopyPlane(uint8_t* pDst, uint32_t dstPitch, uint32_t dstSize, const uint8_t* pSrc, uint32_t srcPitch, uint32_t width, uint32_t height)
{
    if (!pDst || !dstPitch)
        return;

    width = std::min(width, dstPitch);
    height = std::min(height, dstSize / dstPitch);

    for (uint32_t y = 0; y < height; y++)
        memcpy(pDst + y * dstPitch, pSrc + y * srcPitch, width);
}

class MoviePlayer
{
public:
    uint32_t Width{};
    uint32_t Height{};
    float FrameRate{};

    ~MoviePlayer();

    bool Open(const std::filesystem::path& path, int32_t audioStreamId, bool loop, bool isMusic);
    void Stop();
    XmvPlayerState GetState();
    bool CopyNextFrame(const XmvVideoFrame * pFrame);
    void SetVolume(float volume);

private:
    static constexpr size_t MAX_QUEUED_FRAMES = 16;

    struct VideoFrame
    {
        double Time;
        std::vector<uint8_t> Planes[3];
    };

    AVFormatContext* m_pFormat{};
    AVCodecContext* m_pVideoCodec{};
    AVCodecContext* m_pAudioCodec{};
    SwsContext* m_pScaler{};
    SwrContext* m_pResampler{};

    int m_videoStreamIndex{ -1 };
    int m_audioStreamIndex{ -1 };
    uint32_t m_chromaPitch{};
    bool m_loop{};

    double m_timeOffset{};
    double m_lastFrameTime{};
    uint64_t m_decodedAudioFrames{};
    std::vector<int16_t> m_resampleBuffer;

    CCalAudioRenderer* m_pAudioRenderer{};
    XAudioPacket* m_pAudioPackets{};
    be<int16_t>* m_pAudioBuffers{};
    uint32_t m_packetIndex{};
    std::deque<uint32_t> m_packetFrames;
    std::deque<int16_t> m_audioSamples;
    uint64_t m_playedFrames{};
    bool m_audioStarted{};

    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::deque<VideoFrame> m_frames;
    std::atomic<bool> m_stopping{};
    std::atomic<bool> m_decodeFinished{};
    bool m_started{};
    double m_clockTime{};
    std::chrono::steady_clock::time_point m_clockUpdated;

    void OpenAudio(int32_t audioStreamId, bool isMusic);
    void CloseAudio();
    void PumpAudio();
    double GetTime();
    void DecodeThread();
    void Decode(AVCodecContext* pCodec, const AVPacket* pPacket, AVFrame* pFrame);
    void PushVideoFrame(const AVFrame* pFrame);
    void PushAudioFrame(const AVFrame* pFrame);
};

MoviePlayer::~MoviePlayer()
{
    Stop();

    swr_free(&m_pResampler);
    sws_freeContext(m_pScaler);
    avcodec_free_context(&m_pAudioCodec);
    avcodec_free_context(&m_pVideoCodec);
    avformat_close_input(&m_pFormat);
}

bool MoviePlayer::Open(const std::filesystem::path& path, int32_t audioStreamId, bool loop, bool isMusic)
{
    av_log_set_level(AV_LOG_ERROR);
    m_loop = loop;

    auto pathString = path.u8string();

    if (avformat_open_input(&m_pFormat, (const char*)pathString.c_str(), nullptr, nullptr) < 0)
        return false;

    if (avformat_find_stream_info(m_pFormat, nullptr) < 0)
        return false;

    m_videoStreamIndex = av_find_best_stream(m_pFormat, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);

    if (m_videoStreamIndex < 0)
        return false;

    const auto pVideoStream = m_pFormat->streams[m_videoStreamIndex];
    m_pVideoCodec = OpenCodec(pVideoStream);

    if (!m_pVideoCodec)
        return false;

    Width = m_pVideoCodec->width;
    Height = m_pVideoCodec->height;

    // Falls back to r_frame_rate for files that don't report an average rate
    FrameRate = static_cast<float>(av_q2d(av_guess_frame_rate(m_pFormat, pVideoStream, nullptr)));
    m_chromaPitch = (Width + 1) / 2;

    if (audioStreamId != -1)
        OpenAudio(audioStreamId, isMusic);

    m_thread = std::thread(&MoviePlayer::DecodeThread, this);

    return true;
}

void MoviePlayer::OpenAudio(const int32_t audioStreamId, const bool isMusic)
{
    // Count audio streams in file order. The requested one is used if it exists, the first otherwise.
    auto audioStreamCount = 0;

    for (unsigned int i = 0; i < m_pFormat->nb_streams; i++)
    {
        if (m_pFormat->streams[i]->codecpar->codec_type != AVMEDIA_TYPE_AUDIO)
            continue;

        audioStreamCount++;

        if (m_audioStreamIndex == -1 || audioStreamCount == audioStreamId)
            m_audioStreamIndex = i;
    }

    if (m_audioStreamIndex == -1)
        return;

    m_pAudioCodec = OpenCodec(m_pFormat->streams[m_audioStreamIndex]);

    if (!m_pAudioCodec)
    {
        m_audioStreamIndex = -1;
        return;
    }

    AVChannelLayout outputLayout;
    av_channel_layout_default(&outputLayout, AUDIO_CHANNELS);

    swr_alloc_set_opts2(&m_pResampler, &outputLayout, AV_SAMPLE_FMT_S16, AUDIO_SAMPLE_RATE,
        &m_pAudioCodec->ch_layout, m_pAudioCodec->sample_fmt, m_pAudioCodec->sample_rate, 0, nullptr);

    // Prevent ffmpeg's resampler audio clipping protection from making audio quieter
    av_opt_set_double(m_pResampler, "rematrix_maxval", 1000, 0);

    if (!m_pResampler || swr_init(m_pResampler) < 0)
    {
        m_audioStreamIndex = -1;
        return;
    }

    // Create the renderer as XmvpCreateXmvPlayer does.
    // CCalAudioRenderer::operator new
    m_pAudioRenderer = GuestToHostFunction<CCalAudioRenderer*>(sub_82734F50, 0x5D0);
    // CCalAudioRenderer::CCalAudioRenderer
    GuestToHostFunction<void>(sub_827351A0, m_pAudioRenderer);

    guest_stack_var<CalAudioRendererParameters> parameters;
    parameters->Unknown00 = 0;
    parameters->Category = isMusic;
    parameters->QueueLength = AUDIO_PACKET_COUNT;
    parameters->pEffectChain = nullptr;

    // CCalAudioRenderer::Create
    if (GuestToHostFunction<int32_t>(sub_82735210, m_pAudioRenderer, parameters.get()) < 0)
    {
        CloseAudio();
        m_audioStreamIndex = -1;
        return;
    }

    m_pAudioPackets = static_cast<XAudioPacket*>(g_userHeap.Alloc(sizeof(XAudioPacket) * AUDIO_PACKET_COUNT));
    memset(m_pAudioPackets, 0, sizeof(XAudioPacket) * AUDIO_PACKET_COUNT);

    m_pAudioBuffers = static_cast<be<int16_t>*>(g_userHeap.Alloc(AUDIO_PACKET_FRAMES * AUDIO_CHANNELS *
                         sizeof(int16_t) * AUDIO_PACKET_COUNT));
}

void MoviePlayer::CloseAudio()
{
    if (!m_pAudioRenderer)
        return;

    GuestToHostFunction<void>(sub_82631338, m_pAudioRenderer);
    m_pAudioRenderer = nullptr;

    if (m_pAudioPackets)
        g_userHeap.Free(m_pAudioPackets);

    if (m_pAudioBuffers)
        g_userHeap.Free(m_pAudioBuffers);

    m_pAudioPackets = nullptr;
    m_pAudioBuffers = nullptr;
}

void MoviePlayer::PumpAudio()
{
    if (!m_pAudioRenderer)
        return;

    while (m_packetFrames.size() > m_pAudioRenderer->m_ulBusyPackets)
    {
        m_playedFrames += m_packetFrames.front();
        m_packetFrames.pop_front();
    }

    auto pVoice = m_pAudioRenderer->m_pSourceVoice.get();
    auto consumed = false;

    while (m_packetFrames.size() < AUDIO_PACKET_COUNT)
    {
        auto frameCount = static_cast<uint32_t>(std::min<size_t>(m_audioSamples.size() / AUDIO_CHANNELS, AUDIO_PACKET_FRAMES));

        // Only submit a partial packet for the tail of the stream.
        if (frameCount == 0 || (frameCount < AUDIO_PACKET_FRAMES && !m_decodeFinished))
            break;

        const auto sampleCount = frameCount * AUDIO_CHANNELS;
        const auto pBuffer = m_pAudioBuffers + m_packetIndex * AUDIO_PACKET_FRAMES * AUDIO_CHANNELS;

        for (uint32_t i = 0; i < sampleCount; i++)
            pBuffer[i] = m_audioSamples[i];

        auto& packet = m_pAudioPackets[m_packetIndex];
        packet.pBuffer = pBuffer;
        packet.BufferSize = sampleCount * sizeof(int16_t);
        packet.pContext = m_pAudioRenderer;

        AddBusyPackets(m_pAudioRenderer->m_ulBusyPackets, 1);

        // IXAudioSourceVoice::SubmitPacket
        if (GuestToHostFunction<int32_t>(sub_8255A068, pVoice, &packet, 0) < 0)
        {
            AddBusyPackets(m_pAudioRenderer->m_ulBusyPackets, -1);
            break;
        }

        m_audioSamples.erase(m_audioSamples.begin(), m_audioSamples.begin() + sampleCount);
        m_packetFrames.push_back(frameCount);
        m_packetIndex = (m_packetIndex + 1) % AUDIO_PACKET_COUNT;
        consumed = true;
    }

    if (consumed)
        m_condition.notify_all();

    if (m_started && !m_audioStarted && !m_packetFrames.empty())
    {
        // IXAudioSourceVoice::Start
        GuestToHostFunction<int32_t>(sub_8255A180, pVoice, 0);
        m_audioStarted = true;
    }
}

double MoviePlayer::GetTime()
{
    const auto now = std::chrono::steady_clock::now();

    if (m_audioStarted && !m_packetFrames.empty())
    {
        m_clockTime = static_cast<double>(m_playedFrames) / AUDIO_SAMPLE_RATE;
        m_clockUpdated = now;

        return m_clockTime;
    }

    return m_clockTime + std::chrono::duration<double>(now - m_clockUpdated).count();
}

void MoviePlayer::Stop()
{
    {
        std::lock_guard lock(m_mutex);
        m_stopping = true;
    }

    m_condition.notify_all();

    if (m_thread.joinable())
        m_thread.join();

    std::lock_guard lock(m_mutex);
    CloseAudio();
}

XmvPlayerState MoviePlayer::GetState()
{
    std::lock_guard lock(m_mutex);

    PumpAudio();

    const auto audioDrained = m_audioSamples.empty() && m_packetFrames.empty();

    if (m_stopping || (m_decodeFinished && m_frames.empty() && (!m_started || audioDrained)))
        return XmvPlayerState::Stopped;

    return m_started ? XmvPlayerState::Playing : XmvPlayerState::Buffering;
}

bool MoviePlayer::CopyNextFrame(const XmvVideoFrame* pFrame)
{
    std::unique_lock lock(m_mutex);

    if (!m_started && !m_frames.empty())
    {
        m_started = true;
        m_clockUpdated = std::chrono::steady_clock::now();
    }

    PumpAudio();

    if (m_frames.empty())
        return false;

    const auto time = GetTime();

    if (m_frames.front().Time > time)
        return false;

    const auto frame = std::move(m_frames.front());
    m_frames.pop_front();

    lock.unlock();
    m_condition.notify_all();

    CopyPlane(pFrame->Planes[0], pFrame->Pitches[0], pFrame->PlaneSizes[0], frame.Planes[0].data(), Width, Width, Height);

    for (int i = 1; i < 3; i++)
        CopyPlane(pFrame->Planes[i], pFrame->Pitches[i], pFrame->PlaneSizes[i], frame.Planes[i].data(), m_chromaPitch, Width / 2, Height / 2);

    return true;
}

void MoviePlayer::SetVolume(const float volume)
{
    std::lock_guard lock(m_mutex);

    if (m_pAudioRenderer)
        m_pAudioRenderer->m_pSourceVoice->m_pVoice->SetVolume(volume);
}

void MoviePlayer::DecodeThread()
{
    auto pPacket = av_packet_alloc();
    auto pFrame = av_frame_alloc();

    while (!m_stopping)
    {
        if (av_read_frame(m_pFormat, pPacket) < 0)
        {
            // Drain frames the decoders are still holding (WMV3 can have B-frames).
            Decode(m_pVideoCodec, nullptr, pFrame);

            if (m_pAudioCodec)
                Decode(m_pAudioCodec, nullptr, pFrame);

            if (!m_loop || av_seek_frame(m_pFormat, -1, 0, AVSEEK_FLAG_BACKWARD) < 0)
                break;

            avcodec_flush_buffers(m_pVideoCodec);

            if (m_pAudioCodec)
                avcodec_flush_buffers(m_pAudioCodec);

            m_timeOffset = m_audioStreamIndex >= 0 ? static_cast<double>(m_decodedAudioFrames) / AUDIO_SAMPLE_RATE : m_lastFrameTime + 1.0 / FrameRate;
            continue;
        }

        if (pPacket->stream_index == m_videoStreamIndex)
            Decode(m_pVideoCodec, pPacket, pFrame);
        else if (pPacket->stream_index == m_audioStreamIndex)
            Decode(m_pAudioCodec, pPacket, pFrame);

        av_packet_unref(pPacket);
    }

    av_frame_free(&pFrame);
    av_packet_free(&pPacket);

    m_decodeFinished = true;
}

void MoviePlayer::Decode(AVCodecContext* pCodec, const AVPacket* pPacket, AVFrame* pFrame)
{
    if (avcodec_send_packet(pCodec, pPacket) < 0)
        return;

    while (!m_stopping && avcodec_receive_frame(pCodec, pFrame) == 0)
    {
        if (pCodec == m_pVideoCodec)
            PushVideoFrame(pFrame);
        else
            PushAudioFrame(pFrame);

        av_frame_unref(pFrame);
    }
}

void MoviePlayer::PushVideoFrame(const AVFrame* pFrame)
{
    const auto pStream = m_pFormat->streams[m_videoStreamIndex];
    auto time = m_lastFrameTime + 1.0 / FrameRate;

    if (pFrame->best_effort_timestamp != AV_NOPTS_VALUE)
    {
        auto pts = pFrame->best_effort_timestamp;

        if (pStream->start_time != AV_NOPTS_VALUE)
            pts -= pStream->start_time;

        time = m_timeOffset + pts * av_q2d(pStream->time_base);
    }

    m_lastFrameTime = time;

    // Also converts sources that aren't 4:2:0
    m_pScaler = sws_getCachedContext(m_pScaler, pFrame->width, pFrame->height, static_cast<AVPixelFormat>(pFrame->format),
        Width, Height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, nullptr, nullptr, nullptr);

    if (!m_pScaler)
        return;

    VideoFrame frame { time };
    frame.Planes[0].resize(Width * Height);
    frame.Planes[1].resize(m_chromaPitch * ((Height + 1) / 2));
    frame.Planes[2].resize(m_chromaPitch * ((Height + 1) / 2));

    uint8_t* planes[] = { frame.Planes[0].data(), frame.Planes[1].data(), frame.Planes[2].data() };
    const int pitches[] = { static_cast<int>(Width), static_cast<int>(m_chromaPitch), static_cast<int>(m_chromaPitch) };

    sws_scale(m_pScaler, pFrame->data, pFrame->linesize, 0, pFrame->height, planes, pitches);

    std::unique_lock lock(m_mutex);
    m_condition.wait(lock, [this] { return m_stopping || m_frames.size() < MAX_QUEUED_FRAMES; });

    if (!m_stopping)
        m_frames.push_back(std::move(frame));
}

void MoviePlayer::PushAudioFrame(const AVFrame* pFrame)
{
    if (!m_pAudioRenderer)
        return;

    const auto sampleCount = swr_get_out_samples(m_pResampler, pFrame->nb_samples);
    m_resampleBuffer.resize(sampleCount * AUDIO_CHANNELS);

    const auto pOutput = reinterpret_cast<uint8_t*>(m_resampleBuffer.data());
    const auto convertedCount = swr_convert(m_pResampler, &pOutput, sampleCount, pFrame->extended_data, pFrame->nb_samples);

    std::unique_lock lock(m_mutex);

    if (convertedCount > 0)
    {
        m_audioSamples.insert(m_audioSamples.end(), m_resampleBuffer.begin(), m_resampleBuffer.begin() + convertedCount * AUDIO_CHANNELS);
        m_decodedAudioFrames += convertedCount;
    }

    m_condition.wait(lock, [this] { return m_stopping || m_audioSamples.size() < MAX_QUEUED_AUDIO_SAMPLES; });
}

static std::mutex g_moviePlayersMutex;
static std::unordered_map<uint32_t, std::unique_ptr<MoviePlayer>> g_moviePlayers;

static MoviePlayer* FindMoviePlayer(const uint32_t handle)
{
    std::lock_guard lock(g_moviePlayersMutex);

    const auto findResult = g_moviePlayers.find(handle);
    return findResult != g_moviePlayers.end() ? findResult->second.get() : nullptr;
}

void SetMovieVolume(const Sonicteam::MovieObjectWmv* pMovieObjectWmv, const float volume)
{
    const auto pHandle = pMovieObjectWmv->m_pXmvPlayer.get();

    if (!pHandle)
        return;

    if (const auto pPlayer = FindMoviePlayer(g_memory.MapVirtual(pHandle)))
        pPlayer->SetVolume(volume);
}

// XMediaCreateXmvPlayer
PPC_FUNC(sub_82733B78)
{
    const auto pParameters = static_cast<XmvCreateParameters*>(g_memory.Translate(ctx.r4.u32));
    const auto ppPlayer = static_cast<xpointer<XmvPlayer>*>(g_memory.Translate(ctx.r5.u32));
    const auto path = FileSystem::ResolvePath(pParameters->FileName.get(), true);
    auto pPlayer = std::make_unique<MoviePlayer>();

    if (!pPlayer->Open(path, pParameters->AudioStreamId, (pParameters->Flags.get() & 8) != 0, (pParameters->Flags.get() & 1) != 0))
    {
        *ppPlayer = nullptr;
        ctx.r3.u64 = XMV_E_FAIL;
        return;
    }

    const auto pHandle = static_cast<XmvPlayer*>(g_userHeap.Alloc(sizeof(XmvPlayer)));
    pHandle->Vftable = nullptr;

    {
        const auto handle = g_memory.MapVirtual(pHandle);
        std::lock_guard lock(g_moviePlayersMutex);
        g_moviePlayers.emplace(handle, std::move(pPlayer));
    }

    *ppPlayer = pHandle;
    ctx.r3.u64 = XMV_S_OK;
}

// IXMediaXmvPlayer::GetNextFrame
PPC_FUNC(sub_827334D8)
{
    const auto pPlayer = FindMoviePlayer(ctx.r3.u32);
    assert(pPlayer);

    const auto pFrame = static_cast<XmvVideoFrame*>(g_memory.Translate(ctx.r4.u32));

    ctx.r3.u64 = pPlayer->CopyNextFrame(pFrame) ? XMV_S_OK : XMV_S_FALSE;
}

// IXMediaXmvPlayer::GetVideoDescriptor
PPC_FUNC(sub_827334E8)
{
    const auto pPlayer = FindMoviePlayer(ctx.r3.u32);
    assert(pPlayer);

    const auto pDescriptor = static_cast<XmvVideoDescriptor*>(g_memory.Translate(ctx.r4.u32));
    pDescriptor->Width = pPlayer->Width;
    pDescriptor->Height = pPlayer->Height;
    pDescriptor->FrameRate = pPlayer->FrameRate;

    ctx.r3.u64 = XMV_S_OK;
}

// IXMediaXmvPlayer::Stop
PPC_FUNC(sub_82733630)
{
    const auto pPlayer = FindMoviePlayer(ctx.r3.u32);
    assert(pPlayer);

    pPlayer->Stop();
    ctx.r3.u64 = XMV_S_OK;
}

// IXMediaXmvPlayer::GetStatus
PPC_FUNC(sub_82733640)
{
    const auto pPlayer = FindMoviePlayer(ctx.r3.u32);
    assert(pPlayer);

    *static_cast<be<uint32_t> *>(g_memory.Translate(ctx.r4.u32)) = static_cast<uint32_t>(pPlayer->GetState());
    ctx.r3.u64 = XMV_S_OK;
}

PPC_FUNC_IMPL(__imp__sub_82631338);
PPC_FUNC(sub_82631338)
{
    const auto handle = ctx.r3.u32;
    std::unique_ptr<MoviePlayer> pPlayer;

    {
        std::lock_guard lock(g_moviePlayersMutex);

        if (const auto findResult = g_moviePlayers.find(handle); findResult != g_moviePlayers.end())
        {
            pPlayer = std::move(findResult->second);
            g_moviePlayers.erase(findResult);
        }
    }

    if (!pPlayer)
    {
        __imp__sub_82631338(ctx, base);
        return;
    }

    pPlayer.reset();
    g_userHeap.Free(g_memory.Translate(handle));

    ctx.r3.u64 = 0;
}
