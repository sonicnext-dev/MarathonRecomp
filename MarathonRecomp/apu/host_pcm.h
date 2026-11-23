#pragma once

struct AVFormatContext;
struct AVCodecContext;
struct SwrContext;
struct AVPacket;
struct AVFrame;
struct AVStream;

struct HostPcmXmaLayout
{
    uint32_t m_dataOffset{};
    uint32_t m_blockSize{};
    std::vector<uint32_t> m_blockEnds;
};

// Streams an audio file as interleaved 16-bit PCM.
class HostPcm
{
public:
    static std::shared_ptr<HostPcm> Open(const std::filesystem::path& path,
        std::optional<HostPcmXmaLayout> layout = std::nullopt);

    ~HostPcm();

    uint32_t GetChannels() const { return m_channels; }
    uint32_t GetSampleRate() const { return m_sampleRate; }
    uint32_t GetLoopBegin() const { return m_loopBegin; }
    uint32_t GetLoopEnd() const { return m_loopEnd; }

    uint32_t GetFrameCount();
    void SetLengthChangedCallback(std::function<void(uint32_t)> callback);
    uint32_t Read(uint32_t startFrame, uint32_t frameCount, int16_t* dst);

private:
    HostPcm() = default;

    std::mutex m_mutex;

    AVFormatContext* m_pFormat{};
    AVCodecContext* m_pCodec{};
    SwrContext* m_pSwr{};
    AVPacket* m_pPacket{};
    AVFrame* m_pFrame{};
    AVStream* m_pStream{};
    int m_streamIndex{};

    uint32_t m_channels{};
    uint32_t m_sampleRate{};
    uint32_t m_loopBegin{};
    uint32_t m_loopEnd{};
    uint32_t m_frameCount{};
    std::function<void(uint32_t)> m_lengthChanged;

    // Decoded frames not yet handed out. m_pendingStart is the stream index of the first one.
    std::vector<int16_t> m_pending;
    std::vector<int16_t> m_scratch;
    size_t m_pendingOffset{}; // In samples
    int64_t m_pendingStart{};
    int64_t m_nextIndex{}; // Stream index of the next converted frame
    int64_t m_targetIndex{}; // Converted frames before this are dropped
    bool m_needSync{}; // The next decoded frame's timestamp gives the position
    bool m_draining{}; // End of input reached
    bool m_eof{}; // Decoder fully drained
    std::filesystem::path m_path;
    std::optional<HostPcmXmaLayout> m_layout;

    bool Reopen();
    bool OpenInput(const std::filesystem::path& path);
    void ReadLoopPoints();
    void ResetDecoder(int64_t target);
    bool Seek(int64_t target);
    bool RestartFromStart(int64_t target);
    bool DecodeMore();
    void AppendFrame();
    void SetFrameCount(uint32_t frames);
};
