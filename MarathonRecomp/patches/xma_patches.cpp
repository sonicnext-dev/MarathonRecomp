#include <api/Marathon.h>
#include <apu/host_pcm.h>
#include <kernel/heap.h>
#include <kernel/io/file_system.h>
#include <kernel/memory.h>

// 8192 frames per block, 170 ms at 48 kHz.
static constexpr uint32_t PCM_FRAMES_PER_BLOCK = 8192;
static constexpr uint8_t PCM_MAX_PACKET_COUNT = 3;
static constexpr uint8_t XAUDIO_SAMPLETYPE_16BITPCM = 1;

struct HostReader
{
    std::shared_ptr<HostPcm> m_pcm;
    bool m_synthetic{}; // no OS file handle behind the reader
};

static std::mutex g_hostReadersMutex;
static std::unordered_map<uint32_t, HostReader> g_hostReaders; // keyed by guest XmaStreamReader address

struct XmaStreamChunk
{
    be<uint32_t> m_FourCC;
    xpointer<XmaStreamChunk> m_pParent;
    be<uint32_t> m_FileHandle;
    be<uint32_t> m_DataOffset;
    be<uint32_t> m_Size;
    MARATHON_INSERT_PADDING(0x04);
};

MARATHON_ASSERT_SIZEOF(XmaStreamChunk, 0x18);

struct XmaStreamFormat // header at reader + 0x98, partially known; do not assert a size
{
    MARATHON_INSERT_PADDING(0x03);
    uint8_t m_LoopCount;
    be<uint32_t> m_LoopBegin;
    be<uint32_t> m_LoopEnd;
    MARATHON_INSERT_PADDING(0x04);
    be<uint32_t> m_BlockSize;
    MARATHON_INSERT_PADDING(0x08);
    be<uint32_t> m_BlockCount;
};

struct XmaStreamReader
{
    be<uint32_t> m_FileHandle;
    XmaStreamChunk m_Riff;
    XmaStreamChunk m_Fmt;
    XmaStreamChunk m_Data;
    XmaStreamChunk m_Wave;
    XmaStreamChunk m_Seek;
    XmaStreamChunk m_Xma2;
    xpointer<void> m_pFmt;
    xpointer<XmaStreamFormat> m_pXma2;
    xpointer<be<uint32_t>> m_pSeek;
};

MARATHON_ASSERT_SIZEOF(XmaStreamReader, 0xA0);

static std::shared_ptr<HostPcm> FindHostPcm(uint32_t reader)
{
    std::lock_guard lock(g_hostReadersMutex);

    auto it = g_hostReaders.find(reader);

    return it != g_hostReaders.end() ? it->second.m_pcm : nullptr;
}

static std::optional<HostReader> TakeHostReader(uint32_t reader)
{
    std::lock_guard lock(g_hostReadersMutex);

    auto it = g_hostReaders.find(reader);

    if (it == g_hostReaders.end())
        return std::nullopt;

    auto hostReader = std::move(it->second);
    g_hostReaders.erase(it);

    return hostReader;
}

static void RegisterHostReader(uint32_t reader, std::shared_ptr<HostPcm> pcm, bool synthetic)
{
    std::lock_guard lock(g_hostReadersMutex);

    g_hostReaders[reader] = { std::move(pcm), synthetic };
}

static void* AllocGuest(size_t size)
{
    void* pMemory = g_userHeap.Alloc(size);
    memset(pMemory, 0, size);

    return pMemory;
}

static void ApplyPcmGeometry(XmaStreamReader* pReader, XmaStreamFormat* pXma2, const std::shared_ptr<HostPcm>& pcm)
{
    const uint32_t frameSize = pcm->GetChannels() * sizeof(int16_t);
    const uint32_t blockSize = PCM_FRAMES_PER_BLOCK * frameSize;
    const uint32_t frames = pcm->GetFrameCount();
    const uint32_t blockCount = std::max(1u, (frames + PCM_FRAMES_PER_BLOCK - 1) / PCM_FRAMES_PER_BLOCK);

    pXma2->m_BlockSize = blockSize;
    pXma2->m_BlockCount = blockCount;

    // Cumulative frames per block. The last entry is one past the end so that a loop end
    // equal to the file length still matches a block in sub_8238BA40, which compares with
    // a strict less-than.
    auto* pSeek = static_cast<be<uint32_t>*>(AllocGuest(blockCount * sizeof(be<uint32_t>)));

    for (uint32_t i = 0; i < blockCount; i++)
        pSeek[i] = std::min((i + 1) * PCM_FRAMES_PER_BLOCK, frames);

    pSeek[blockCount - 1] = frames + 1;

    // The original table came from the same heap, and the reader destroy only frees what is set then.
    if (auto* pOldSeek = pReader->m_pSeek.get())
        g_userHeap.Free(pOldSeek);

    pReader->m_pSeek = pSeek;

    // sub_8238B820 clamps block reads against the data chunk size.
    pReader->m_Data.m_Size = frames * frameSize;

    // The game re-reads both of these on every block, so a corrected length takes effect immediately.
    pcm->SetLengthChangedCallback([pReader, pXma2, frameSize](uint32_t newFrames)
    {
        pReader->m_Data.m_Size = newFrames * frameSize;
        pXma2->m_BlockCount = (newFrames + PCM_FRAMES_PER_BLOCK - 1) / PCM_FRAMES_PER_BLOCK;
    });
}

static bool InstallSyntheticReader(uint32_t readerAddress, std::shared_ptr<HostPcm> pcm)
{
    if (!pcm)
        return false;

    auto* pReader = static_cast<XmaStreamReader*>(g_memory.Translate(readerAddress));

    const uint32_t channels = pcm->GetChannels();
    const uint32_t streams = channels <= 2 ? 1 : (channels + 1) / 2;
    auto* pFmt = static_cast<uint8_t*>(AllocGuest(0x80));

    *reinterpret_cast<be<uint16_t>*>(pFmt + 0x08) = static_cast<uint16_t>(streams);
    *reinterpret_cast<be<uint32_t>*>(pFmt + 0x10) = pcm->GetSampleRate();
    pFmt[0x1D] = static_cast<uint8_t>(std::min(channels, 2u));

    // XMA2 chunk copy, only the fields the game reads.
    auto* pXma2 = static_cast<XmaStreamFormat*>(AllocGuest(0x40));

    pXma2->m_LoopCount = 0; // looping comes from the cue flag
    pXma2->m_LoopBegin = pcm->GetLoopBegin();
    pXma2->m_LoopEnd = pcm->GetLoopEnd();

    ApplyPcmGeometry(pReader, pXma2, pcm);

    // There is no OS file handle, the reader destroy hook resets this before the original runs.
    pReader->m_FileHandle = 0;
    pReader->m_pFmt = pFmt;
    pReader->m_pXma2 = pXma2;

    RegisterHostReader(readerAddress, std::move(pcm), true);

    return true;
}

static bool InstallStockReader(uint32_t readerAddress, const std::filesystem::path& path)
{
    auto* pReader = static_cast<XmaStreamReader*>(g_memory.Translate(readerAddress));

    auto* pXma2 = pReader->m_pXma2.get();
    auto* pSeek = pReader->m_pSeek.get();

    if (!pXma2 || !pSeek)
        return false;

    const uint32_t blockCount = pXma2->m_BlockCount;
    const uint32_t blockSize = pXma2->m_BlockSize;

    if (blockCount == 0 || blockSize == 0)
        return false;

    HostPcmXmaLayout layout;
    layout.m_dataOffset = pReader->m_Data.m_DataOffset;
    layout.m_blockSize = blockSize;

    for (uint32_t i = 0; i < blockCount; i++)
        layout.m_blockEnds.push_back(pSeek[i]);

    auto pcm = HostPcm::Open(path, std::move(layout));

    if (!pcm)
        return false;

    ApplyPcmGeometry(pReader, pXma2, pcm);
    RegisterHostReader(readerAddress, std::move(pcm), false);

    return true;
}

// Reader destroy and reset
PPC_FUNC_IMPL(__imp__sub_8238B8F0);
PPC_FUNC(sub_8238B8F0)
{
    if (auto hostReader = TakeHostReader(ctx.r3.u32))
    {
        hostReader->m_pcm->SetLengthChangedCallback(nullptr);

        if (hostReader->m_synthetic)
            static_cast<XmaStreamReader*>(g_memory.Translate(ctx.r3.u32))->m_FileHandle = 0xFFFFFFFF;
    }

    __imp__sub_8238B8F0(ctx, base);
}

// RIFF Parser
PPC_FUNC_IMPL(__imp__sub_8238BCB8);
PPC_FUNC(sub_8238BCB8)
{
    const uint32_t reader = ctx.r3.u32;
    const auto hostPath = FileSystem::ResolvePath(std::string_view(static_cast<const char*>(g_memory.Translate(ctx.r4.u32))), true);

    __imp__sub_8238BCB8(ctx, base);

    if (ctx.r3.s32 >= 0)
    {
        InstallStockReader(reader, hostPath);
        return;
    }

    const uint32_t result = ctx.r3.u32;

    ctx.r3.u32 = reader;
    // Reset what the parser left behind
    __imp__sub_8238B8F0(ctx, base);

    if (InstallSyntheticReader(reader, HostPcm::Open(hostPath)))
    {
        ctx.r3.u32 = 0;
        return;
    }

    ctx.r3.u32 = result;
}

// ReadChunk
PPC_FUNC_IMPL(__imp__sub_8238B650);
PPC_FUNC(sub_8238B650)
{
    auto pcm = FindHostPcm(ctx.r3.u32 - 0x34);

    if (!pcm)
    {
        __imp__sub_8238B650(ctx, base);
        return;
    }

    const uint32_t channels = pcm->GetChannels();
    const uint32_t frameSize = channels * sizeof(int16_t);
    const uint32_t size = ctx.r6.u32;

    auto* pDestination = static_cast<int16_t*>(g_memory.Translate(ctx.r5.u32));

    const uint32_t frames = pcm->Read(ctx.r4.u32 / frameSize, size / frameSize, pDestination);
    const size_t samples = static_cast<size_t>(frames) * channels;

    // The guest engine reads its PCM as big-endian 16-bit samples.
    for (size_t i = 0; i < samples; i++)
        ByteSwapInplace(pDestination[i]);

    // Fill anything extra with silence.
    memset(pDestination + samples, 0, size - samples * sizeof(int16_t));

    ctx.r3.u32 = 0;
}

// FillVoiceInit
PPC_FUNC_IMPL(__imp__sub_8238B960);
PPC_FUNC(sub_8238B960)
{
    auto pcm = FindHostPcm(ctx.r3.u32);
    const uint32_t init = ctx.r4.u32;

    // The original picks the voice routing from the fmt chunk.
    __imp__sub_8238B960(ctx, base);

    if (!pcm)
        return;

    auto* pInit = static_cast<uint8_t*>(g_memory.Translate(init));

    pInit[0x00] = XAUDIO_SAMPLETYPE_16BITPCM;
    pInit[0x04] = static_cast<uint8_t>(pcm->GetChannels());
    *reinterpret_cast<be<uint32_t>*>(pInit + 0x08) = pcm->GetSampleRate();
    memset(pInit + 0x0C, 0, 0x14);
    pInit[0x3B] = PCM_MAX_PACKET_COUNT;
}
