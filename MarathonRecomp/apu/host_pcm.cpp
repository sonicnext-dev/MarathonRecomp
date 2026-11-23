#include "host_pcm.h"
#include <os/logger.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/channel_layout.h>
#include <libswresample/swresample.h>
}

// The guest engine mixes at most this many channels, so only layouts above it are reduced.
static constexpr int MAX_CHANNELS = 6;

static void LogAvError(const char* action, int result)
{
    char message[AV_ERROR_MAX_STRING_SIZE]{};
    av_strerror(result, message, sizeof(message));

    LOGFN_ERROR("Failed to {}: {}", action, message);
}

static std::optional<uint32_t> GetTagValue(const AVDictionary* pDict, std::initializer_list<const char*> keys)
{
    for (auto key : keys)
    {
        if (auto* pEntry = av_dict_get(pDict, key, nullptr, 0))
            return static_cast<uint32_t>(strtoul(pEntry->value, nullptr, 10));
    }

    return std::nullopt;
}

HostPcm::~HostPcm()
{
    av_frame_free(&m_pFrame);
    av_packet_free(&m_pPacket);
    swr_free(&m_pSwr);
    avcodec_free_context(&m_pCodec);
    avformat_close_input(&m_pFormat);
}

std::shared_ptr<HostPcm> HostPcm::Open(const std::filesystem::path& path, std::optional<HostPcmXmaLayout> layout)
{
    std::error_code ec;

    if (!std::filesystem::is_regular_file(path, ec))
        return nullptr;

    std::shared_ptr<HostPcm> pcm(new HostPcm());

    pcm->m_path = path;
    pcm->m_layout = std::move(layout);

    if (!pcm->OpenInput(path))
        return nullptr;

    return pcm;
}

bool HostPcm::OpenInput(const std::filesystem::path& path)
{
    auto utf8Path = path.u8string();
    auto* pPath = reinterpret_cast<const char*>(utf8Path.c_str());

    int result = avformat_open_input(&m_pFormat, pPath, nullptr, nullptr);

    if (result < 0)
    {
        LogAvError("open input", result);
        return false;
    }

    result = avformat_find_stream_info(m_pFormat, nullptr);

    if (result < 0)
    {
        LogAvError("find stream info", result);
        return false;
    }

    const AVCodec* pDecoder = nullptr;
    m_streamIndex = av_find_best_stream(m_pFormat, AVMEDIA_TYPE_AUDIO, -1, -1, &pDecoder, 0);

    if (m_streamIndex < 0)
    {
        LogAvError("find audio stream", m_streamIndex);
        return false;
    }

    m_pStream = m_pFormat->streams[m_streamIndex];

    m_pCodec = avcodec_alloc_context3(pDecoder);
    avcodec_parameters_to_context(m_pCodec, m_pStream->codecpar);
    m_pCodec->pkt_timebase = m_pStream->time_base;

    result = avcodec_open2(m_pCodec, pDecoder, nullptr);

    if (result < 0)
    {
        LogAvError("open decoder", result);
        return false;
    }

    const int sourceChannels = m_pCodec->ch_layout.nb_channels;

    m_channels = std::min(sourceChannels, MAX_CHANNELS);
    m_sampleRate = m_pCodec->sample_rate;

    AVChannelLayout outLayout{};

    if (sourceChannels <= MAX_CHANNELS)
        av_channel_layout_copy(&outLayout, &m_pCodec->ch_layout);
    else
        av_channel_layout_default(&outLayout, MAX_CHANNELS);

    result = swr_alloc_set_opts2(&m_pSwr, &outLayout, AV_SAMPLE_FMT_S16, m_sampleRate,
        &m_pCodec->ch_layout, m_pCodec->sample_fmt, m_pCodec->sample_rate, 0, nullptr);

    av_channel_layout_uninit(&outLayout);

    if (result >= 0)
        result = swr_init(m_pSwr);

    if (result < 0)
    {
        LogAvError("create resampler", result);
        return false;
    }

    // The length is only an estimate, Read corrects it.
    const AVRational frameBase{ 1, static_cast<int>(m_sampleRate) };
    int64_t frames = 0;

    if (m_layout)
    {
        // XMA files do not have a duration that ffmpeg can read.
        // The last seek table entry is the exact length.
        frames = m_layout->m_blockEnds.empty() ? 0 : m_layout->m_blockEnds.back();
    }
    else if (m_pStream->duration != AV_NOPTS_VALUE)
    {
        frames = av_rescale_q(m_pStream->duration, m_pStream->time_base, frameBase);
    }
    else if (m_pFormat->duration != AV_NOPTS_VALUE)
    {
        frames = av_rescale_q(m_pFormat->duration, AV_TIME_BASE_Q, frameBase);
    }

    if (frames <= 0)
    {
        LOGFN_ERROR("Cannot determine the length of {}", pPath);
        return false;
    }

    m_frameCount = static_cast<uint32_t>(frames);
    m_pPacket = av_packet_alloc();
    m_pFrame = av_frame_alloc();

    ReadLoopPoints();

    return true;
}

// Loop points are read from LOOPSTART/LOOPEND or LOOPSTART/LOOPLENGTH tags, in samples.
// Without usable tags the whole track loops.
void HostPcm::ReadLoopPoints()
{
    const AVDictionary* dictionaries[] = { m_pStream->metadata, m_pFormat->metadata };

    std::optional<uint32_t> begin, end, length;

    for (auto* pDict : dictionaries)
    {
        if (!pDict)
            continue;

        if (!begin)
            begin = GetTagValue(pDict, { "LOOPSTART", "LOOP_START" });

        if (!end)
            end = GetTagValue(pDict, { "LOOPEND", "LOOP_END" });

        if (!length)
            length = GetTagValue(pDict, { "LOOPLENGTH", "LOOP_LENGTH" });
    }

    m_loopBegin = begin.value_or(0);
    m_loopEnd = end ? *end : length ? m_loopBegin + *length : m_frameCount;
    m_loopEnd = std::min(m_loopEnd, m_frameCount);

    if (m_loopEnd <= m_loopBegin)
    {
        m_loopBegin = 0;
        m_loopEnd = m_frameCount;
    }
}

uint32_t HostPcm::GetFrameCount()
{
    std::lock_guard lock(m_mutex);

    return m_frameCount;
}

void HostPcm::SetLengthChangedCallback(std::function<void(uint32_t)> callback)
{
    std::lock_guard lock(m_mutex);

    m_lengthChanged = std::move(callback);
}

void HostPcm::SetFrameCount(uint32_t frames)
{
    if (frames == m_frameCount)
        return;

    m_frameCount = frames;

    if (m_lengthChanged)
        m_lengthChanged(frames);
}

void HostPcm::ResetDecoder(int64_t target)
{
    m_pending.clear();
    m_pendingOffset = 0;
    m_pendingStart = target;
    m_nextIndex = 0;
    m_targetIndex = target;
    m_needSync = false;
    m_draining = false;
    m_eof = false;

    avcodec_flush_buffers(m_pCodec);

    swr_close(m_pSwr);
    swr_init(m_pSwr);
}

bool HostPcm::Seek(int64_t target)
{
    ResetDecoder(target);

    if (m_layout)
    {
        const auto& ends = m_layout->m_blockEnds;
        const size_t block = std::min<size_t>(
            std::upper_bound(ends.begin(), ends.end(), target) - ends.begin(), ends.size() - 1);

        const int64_t offset = m_layout->m_dataOffset + static_cast<int64_t>(block) * m_layout->m_blockSize;

        if (avformat_seek_file(m_pFormat, -1, offset, offset, offset, AVSEEK_FLAG_BYTE) < 0)
            return RestartFromStart(target);

        m_nextIndex = block != 0 ? ends[block - 1] : 0;

        return true;
    }

    const AVRational frameBase{ 1, static_cast<int>(m_sampleRate) };
    const int64_t start = m_pStream->start_time != AV_NOPTS_VALUE ? m_pStream->start_time : 0;
    const int64_t preroll = std::min<int64_t>(target, m_sampleRate / 2);
    const int64_t timestamp = start + av_rescale_q(target - preroll, frameBase, m_pStream->time_base);

    if (avformat_seek_file(m_pFormat, m_streamIndex, INT64_MIN, timestamp, timestamp, 0) < 0)
        return RestartFromStart(target);

    m_needSync = true;

    return true;
}

bool HostPcm::RestartFromStart(int64_t target)
{
    ResetDecoder(target);

    if (!Reopen())
    {
        LOGN_ERROR("Failed to rewind audio stream");

        m_eof = true;
        return false;
    }

    return true;
}

// Some containers cannot seek at all, so open the file again instead.
bool HostPcm::Reopen()
{
    avformat_close_input(&m_pFormat);

    auto utf8Path = m_path.u8string();

    if (avformat_open_input(&m_pFormat, reinterpret_cast<const char*>(utf8Path.c_str()), nullptr, nullptr) < 0)
    {
        m_pStream = nullptr;
        return false;
    }

    if (m_streamIndex >= static_cast<int>(m_pFormat->nb_streams))
        return false;

    m_pStream = m_pFormat->streams[m_streamIndex];

    return true;
}

void HostPcm::AppendFrame()
{
    if (m_needSync)
    {
        const AVRational frameBase{ 1, static_cast<int>(m_sampleRate) };
        const int64_t start = m_pStream->start_time != AV_NOPTS_VALUE ? m_pStream->start_time : 0;
        const int64_t timestamp = m_pFrame->best_effort_timestamp;

        int64_t index = INT64_MIN;

        if (timestamp != AV_NOPTS_VALUE)
            index = av_rescale_q(timestamp - start, m_pStream->time_base, frameBase);

        // Without a timestamp, or after landing past the target, the position cannot be trusted.
        if (index == INT64_MIN || index > m_targetIndex)
        {
            RestartFromStart(m_targetIndex);
            return;
        }

        m_nextIndex = index;
        m_needSync = false;
    }

    const int count = m_pFrame->nb_samples;
    const int capacity = swr_get_out_samples(m_pSwr, count);

    if (capacity <= 0)
        return;

    m_scratch.resize(static_cast<size_t>(capacity) * m_channels);

    auto* pOutput = reinterpret_cast<uint8_t*>(m_scratch.data());
    const int frames = swr_convert(m_pSwr, &pOutput, capacity,
        m_pFrame->extended_data, count);

    if (frames <= 0)
        return;

    const int64_t skip = std::clamp<int64_t>(m_targetIndex - m_nextIndex, 0, frames);

    m_pending.insert(m_pending.end(),
        m_scratch.begin() + skip * m_channels,
        m_scratch.begin() + frames * m_channels);

    m_nextIndex += frames;
}

// Decodes one more frame into m_pending. Returns false once the stream is exhausted.
bool HostPcm::DecodeMore()
{
    while (!m_eof)
    {
        int result = avcodec_receive_frame(m_pCodec, m_pFrame);

        if (result >= 0)
        {
            AppendFrame();
            av_frame_unref(m_pFrame);

            return true;
        }

        if (result == AVERROR(EAGAIN) && !m_draining)
        {
            if (av_read_frame(m_pFormat, m_pPacket) < 0)
            {
                // A null packet drains the decoder.
                m_draining = true;
                avcodec_send_packet(m_pCodec, nullptr);
            }
            else
            {
                if (m_pPacket->stream_index == m_streamIndex)
                    avcodec_send_packet(m_pCodec, m_pPacket);

                av_packet_unref(m_pPacket);
            }

            continue;
        }

        if (result != AVERROR_EOF && result != AVERROR(EAGAIN))
            LogAvError("decode audio", result);

        m_eof = true;
    }

    return false;
}

uint32_t HostPcm::Read(uint32_t startFrame, uint32_t frameCount, int16_t* dst)
{
    std::lock_guard lock(m_mutex);

    if (startFrame != m_pendingStart && !Seek(startFrame))
        return 0;

    uint32_t written = 0;

    while (written < frameCount)
    {
        const size_t available = (m_pending.size() - m_pendingOffset) / m_channels;

        if (available == 0)
        {
            if (!DecodeMore())
                break;

            continue;
        }

        const size_t count = std::min<size_t>(frameCount - written, available);

        memcpy(dst + static_cast<size_t>(written) * m_channels,
            m_pending.data() + m_pendingOffset, count * m_channels * sizeof(int16_t));

        m_pendingOffset += count * m_channels;
        m_pendingStart += count;
        written += static_cast<uint32_t>(count);
    }

    // Drop what was handed out, the leftover is less than one decoded frame.
    m_pending.erase(m_pending.begin(), m_pending.begin() + static_cast<ptrdiff_t>(m_pendingOffset));
    m_pendingOffset = 0;

    const uint32_t end = startFrame + written;

    if (written < frameCount)
    {
        // Ran into the end of the stream before the estimate said it would.
        SetFrameCount(end);
    }
    else if (end >= m_frameCount)
    {
        // Served the last block of the estimate, look ahead: there may be more audio.
        while (m_pending.empty() && DecodeMore()) {}

        SetFrameCount(end + static_cast<uint32_t>(m_pending.size() / m_channels));
    }

    return written;
}
