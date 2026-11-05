/**
******************************************************************************
* Xenia : Xbox 360 Emulator Research Project                                 *
******************************************************************************
* Copyright 2024 Xenia Canary. All rights reserved.                          *
* Released under the BSD license - see LICENSE in the root for more details. *
******************************************************************************
*/

// Almost all decoding code is from Xenia Canary, so leave the copyright here

#include <condition_variable>
#include <kernel/function.h>
#include <kernel/heap.h>
#include <utils/bit_stream.h>
#include <utils/host_thread.h>
#include <utils/ring_buffer.h>

extern "C" {
    #include <libavcodec/avcodec.h>
}

// #define ENABLE_DEBUG_XMA_DECODER

#ifdef ENABLE_DEBUG_XMA_DECODER
#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_printf(...)
#endif

constexpr uint32_t kBytesPerPacket = 2048;
constexpr uint32_t kBytesPerPacketHeader = 4;
constexpr uint32_t kBytesPerPacketData = kBytesPerPacket - kBytesPerPacketHeader;
constexpr uint32_t kBytesPerSample = 2;
constexpr uint32_t kSamplesPerFrame = 512;
constexpr uint32_t kBytesPerFrameChannel = kSamplesPerFrame * kBytesPerSample;

constexpr uint32_t kOutputBytesPerBlock = 256;
constexpr uint32_t kBitsPerPacketHeader = 32;
constexpr uint32_t kBitsPerPacket = kBytesPerPacket * 8;
constexpr uint32_t kMaxFrameLength = 0x7FFF;
constexpr uint32_t kBitsPerFrameHeader = 15;
constexpr uint32_t kMaxFrameSizeinBits = 0x4000 - kBitsPerPacketHeader;

struct XMAPLAYBACKINIT {
    be<uint32_t> sampleRate;
    be<uint32_t> outputBufferSize;
    uint8_t channelCount;
    uint8_t subframes;
};

void DecoderThreadFunc(void* playback);

struct XmaPlayback {
    uint32_t sampleRate;
    uint32_t outputBufferSize;
    uint32_t channelCount;
    uint32_t subframes;
    uint8_t* outputBuffer;
    uint32_t outputBufferVirtual;
    uint32_t currentDataSize;

    uint32_t partialBytesRead = 0;
    uint32_t streamPosition = 0;

    // ffmpeg
    AVCodecContext *codec_ctx = nullptr;
    const AVCodec *codec = nullptr;
    AVPacket *av_packet_ = nullptr;
    AVFrame *av_frame_ = nullptr;

    // xenia
    std::array<uint8_t, kBytesPerPacketData * 2> inputBuffer;
    std::array<uint8_t, 1 + 4096> xmaFrame;
    std::array<uint8_t, kBytesPerFrameChannel * 2> rawFrame;
    uint32_t outputBufferBlockCount = 0;
    uint32_t outputBufferReadOffset = 0;
    uint32_t outputBufferWriteOffset = 0;
    int32_t remainingSubframeBlocksInOutputBuffer = 0;
    uint8_t currentFrameRemainingSubframes = 0;
    uint32_t inputBufferReadOffset = 32;

    uint32_t numSubframesToSkip = 0;

    RingBuffer outputRb;
    uint32_t inputBuffer1 = 0;
    uint32_t inputBuffer2 = 0;
    size_t inputBuffer1Size = 0;
    size_t inputBuffer2Size = 0;
    uint32_t validInputBuffer = 0;
    std::atomic<uint32_t> inputBuffer1Valid = 0;
    std::atomic<uint32_t> inputBuffer2Valid = 0;
    uint32_t currentBuffer = 0;

    uint32_t outputBufferValid = 1;

    uint8_t numLoops = 0;
    uint8_t loopSubframeEnd = 0;
    uint8_t loopSubframeSkip = 0;
    uint32_t loopStartOffset = 0;
    uint32_t loopEndOffset = 0;

    std::unique_ptr<HostThread> decoderThread;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> isLocked { false };
    std::atomic<bool> isRunning { true };

    XmaPlayback(uint32_t sampleRate, uint32_t outputBufferSize,
                uint32_t channelCount, uint32_t subframes)
                : sampleRate(sampleRate), outputBufferSize(outputBufferSize),
                  channelCount(channelCount), subframes(subframes),
                  outputRb(nullptr, 0) {
        outputBufferBlockCount =
            (((channelCount * outputBufferSize) << 15) & 0x7C00000) >> 22;
        outputBuffer =
            static_cast<uint8_t*>(g_userHeap.AllocPhysical(0x2000, 0));
        outputBufferVirtual = g_memory.MapVirtual(outputBuffer);

        codec = avcodec_find_decoder(AV_CODEC_ID_XMAFRAMES);
        if (!codec) {
            throw std::runtime_error("Decoder not found");
        }

        codec_ctx = avcodec_alloc_context3(codec);
        if (!codec_ctx) {
            throw std::runtime_error("Failed to allocate codec context");
        }

        codec_ctx->sample_rate = sampleRate;
        codec_ctx->ch_layout.nb_channels = channelCount;

        av_frame_ = av_frame_alloc();
        if (!av_frame_) {
            throw std::runtime_error("Coulnd't allocate frame");
        }

        if (int err = avcodec_open2(codec_ctx, codec, nullptr); err < 0) {
            throw std::runtime_error("Failed to open codec");
        }
        av_packet_ = av_packet_alloc();

        decoderThread = std::make_unique<HostThread>("XMA Decode Thread", DecoderThreadFunc, this);
    }

    ~XmaPlayback() {
        {
            std::lock_guard lock(mutex);
            isRunning = false;
            cv.notify_one();
        }

        decoderThread->Join();

        av_packet_free(&av_packet_);
        av_frame_free(&av_frame_);
        avcodec_free_context(&codec_ctx);

        g_userHeap.Free(outputBuffer);
    }

    const uint32_t GetInputBufferAddress(uint8_t bufferIndex) const {
        return bufferIndex == 0 ? inputBuffer1 : inputBuffer2;
    }

    const uint32_t GetCurrentInputBufferAddress() const {
        return GetInputBufferAddress(currentBuffer);
    }

    const uint32_t GetInputBufferPacketCount(uint8_t bufferIndex) const {
        return bufferIndex == 0 ? inputBuffer1Size : inputBuffer2Size;
    }
    const uint32_t GetCurrentInputBufferPacketCount() const {
        return GetInputBufferPacketCount(currentBuffer);
    }

    uint8_t *GetCurrentInputBuffer() {
        return (uint8_t *)g_memory.Translate(GetCurrentInputBufferAddress());
    }

    bool IsInputBufferValid(uint8_t bufferIndex) const {
        return bufferIndex == 0 ? inputBuffer1Valid : inputBuffer2Valid;
    }

    bool IsCurrentInputBufferValid() const {
        return IsInputBufferValid(currentBuffer);
    }

    bool IsAnyInputBufferValid() const {
        return inputBuffer1Valid || inputBuffer2Valid;
    }
};


uint32_t XMAPlaybackGetFrameOffsetFromPacketHeader(uint32_t header) {
    uint32_t result = 0;

    if (header != 0x7FFF) {
        return ((header >> 11) & 0x7FFF) + 32;
    }

    return result;
}

int16_t GetPacketNumber(size_t size, size_t bitOffset) {
    if (bitOffset < kBitsPerPacketHeader) {
        return -1;
    }

    if (bitOffset >= (size << 3)) {
        return -1;
    }

    size_t byteOffset = bitOffset >> 3;
    size_t packetNumber = byteOffset / kBytesPerPacket;

    return (int16_t)packetNumber;
}

static uint32_t GetPacketFrameOffset(const uint8_t *packet) {
    uint32_t val = (uint16_t)(((packet[0] & 0x3) << 13) | (packet[1] << 5) | (packet[2] >> 3));
    return val + 32;
}

struct kPacketInfo {
    uint8_t frameCount;
    uint8_t currentFrame;
    uint32_t currentFrameSize;

    const bool IsLastFrameInPacket() const {
        return currentFrame == frameCount - 1;
    }
};

kPacketInfo GetPacketInfo(uint8_t *packet, uint32_t frameOffset) {
    kPacketInfo packetInfo = {};

    const uint32_t firstFrameOffset = GetPacketFrameOffset(packet);
    BitStream stream(packet, kBitsPerPacket);
    stream.SetOffset(firstFrameOffset);

    // Handling of split frame
    if (frameOffset < firstFrameOffset) {
        packetInfo.currentFrame = 0;
        packetInfo.currentFrameSize = firstFrameOffset - frameOffset;
    }

    while (true) {
        if (stream.BitsRemaining() < kBitsPerFrameHeader) {
            break;
        }

        const uint64_t frameSize = stream.Peek(kBitsPerFrameHeader);
        if (frameSize == kMaxFrameLength) {
            break;
        }

        if (stream.offset_bits() == frameOffset) {
            packetInfo.currentFrame = packetInfo.frameCount;
            packetInfo.currentFrameSize = (uint32_t)frameSize;
        }

        packetInfo.frameCount++;

        if (frameSize > stream.BitsRemaining()) {
            // Last frame.
            break;
        }

        stream.Advance(frameSize - 1);

        // Read the trailing bit to see if frames follow
        if (stream.Read(1) == 0) {
            break;
        }
    }

    return packetInfo;
}

uint8_t *GetNextPacket(XmaPlayback *playback, uint32_t nextPacketIndex, uint32_t currentInputPacketCount) {
    if (nextPacketIndex < currentInputPacketCount) {
        uint8_t *currentInputBuffer = (uint8_t *)g_memory.Translate(playback->GetCurrentInputBufferAddress());
        return currentInputBuffer + nextPacketIndex * kBytesPerPacket;
    }

    const uint8_t nextBufferIndex = playback->currentBuffer ^ 1;

    if (!playback->IsInputBufferValid(nextBufferIndex)) {
        return nullptr;
    }

    const uint32_t nextBufferAddress = playback->GetInputBufferAddress(nextBufferIndex);

    if (!nextBufferAddress) {
        // This should never occur, but there is always a chance
        debug_printf("XmaContext: Buffer is marked as valid, but doesn't have valid pointer!\n");
        return nullptr;
    }

    return (uint8_t *)g_memory.Translate(nextBufferAddress);
}

uint8_t GetPacketSkipCount(const uint8_t *packet) { return packet[3]; }

uint32_t GetAmountOfBitsToRead(const uint32_t remainingStreamBits, const uint32_t frameSize) {
    return std::min(remainingStreamBits, frameSize);
}

template <typename T> T clamp_float(T value, T minValue, T maxValue) {
    float clampedToMin = std::isgreater(value, minValue) ? value : minValue;
    return std::isless(clampedToMin, maxValue) ? clampedToMin : maxValue;
}

const uint32_t GetNextPacketReadOffset(uint8_t *buffer, uint32_t nextPacketIndex,
                                       uint32_t currentInputPacketCount) {
    if (nextPacketIndex >= currentInputPacketCount) {
        return kBitsPerPacketHeader;
    }

    uint8_t *nextPacket = buffer + (nextPacketIndex * kBytesPerPacket);
    const uint32_t packetFrameOffset = GetPacketFrameOffset(nextPacket);

    if (packetFrameOffset > kMaxFrameSizeinBits) {
        const uint32_t offset = GetNextPacketReadOffset(buffer, nextPacketIndex + 1, currentInputPacketCount);
        return offset;
    }

    const uint32_t newInputBufferOffset = (nextPacketIndex * kBitsPerPacket) + packetFrameOffset;

    return newInputBufferOffset;
}

void SwapInputBuffer(XmaPlayback *playback) {
    // No more frames.
    if (playback->currentBuffer == 0) {
        playback->inputBuffer1Valid = 0;
    } else {
        playback->inputBuffer2Valid = 0;
    }

    playback->currentBuffer ^= 1;
    playback->inputBufferReadOffset = kBitsPerPacketHeader;
}

void UpdateLoopStatus(XmaPlayback *playback) {
    if (playback->numLoops == 0) {
        return;
    }

    const uint32_t loop_start = std::max(kBitsPerPacketHeader, playback->loopStartOffset);
    const uint32_t loop_end = std::max(kBitsPerPacketHeader, playback->loopEndOffset);

    if (playback->inputBufferReadOffset != loop_end) {
        return;
    }

    playback->inputBufferReadOffset = loop_start;
    playback->numSubframesToSkip = playback->loopSubframeSkip;

    if (playback->numLoops != 255) {
        playback->numLoops--;
    }
}

void Decode(XmaPlayback *playback) {
    if (!playback->IsAnyInputBufferValid()) {
        return;
    }

    if (playback->currentFrameRemainingSubframes > 0) {
        return;
    }

    uint8_t *currentInputBuffer = playback->GetCurrentInputBuffer();

    playback->inputBuffer.fill(0);

    UpdateLoopStatus(playback); // TODO

    const uint32_t currentInputSize = playback->GetCurrentInputBufferPacketCount() * kBytesPerPacket;
    uint32_t currentInputPacketCount = currentInputSize / kBytesPerPacket;
    int16_t packetIndex = GetPacketNumber(currentInputSize, playback->inputBufferReadOffset);

    if (packetIndex == -1) {
        return;
    }

    uint8_t *packet = currentInputBuffer + (packetIndex * kBytesPerPacket);

    const uint32_t frameOffset = GetPacketFrameOffset(packet);
    if (playback->inputBufferReadOffset < frameOffset) {
        playback->inputBufferReadOffset = frameOffset;
    }

    uint32_t relativeOffset = playback->inputBufferReadOffset % kBitsPerPacket;
    const kPacketInfo packetInfo = GetPacketInfo(packet, relativeOffset);
    const uint32_t packetToSkip = GetPacketSkipCount(packet) + 1;
    const uint32_t nextPacketIndex = packetIndex + packetToSkip;

    BitStream stream = BitStream(currentInputBuffer, (packetIndex + 1) * kBitsPerPacket);
    stream.SetOffset(playback->inputBufferReadOffset);

    const uint64_t bitsToCopy = GetAmountOfBitsToRead((uint32_t)stream.BitsRemaining(),
                                                      packetInfo.currentFrameSize);

    if (bitsToCopy == 0) {
        SwapInputBuffer(playback);
        return;
    }

    if (packetInfo.IsLastFrameInPacket()) {
        // Frame is a split frame
        if (stream.BitsRemaining() < packetInfo.currentFrameSize) {
            const uint8_t *nextPacket = GetNextPacket(playback, nextPacketIndex, currentInputPacketCount);

            if (!nextPacket) {
                // Error path
                // Decoder probably should return error here
                // Not sure what error code should be returned
                // data->error_status = 4;
                __builtin_debugtrap();
                return;
            }

            // Copy next packet to buffer
            std::memcpy(playback->inputBuffer.data() + kBytesPerPacketData,
                        nextPacket + kBytesPerPacketHeader, kBytesPerPacketData);
        }
    }

    std::memcpy(playback->inputBuffer.data(), packet + kBytesPerPacketHeader, kBytesPerPacketData);

    stream = BitStream(playback->inputBuffer.data(), (kBitsPerPacket - kBitsPerPacketHeader) * 2);
    stream.SetOffset(relativeOffset - kBitsPerPacketHeader);

    playback->xmaFrame.fill(0);

    const uint32_t paddingStart = static_cast<uint8_t>(stream.Copy(playback->xmaFrame.data() + 1,
                                                                   packetInfo.currentFrameSize));

    playback->rawFrame.fill(0);
    playback->av_packet_->data = playback->xmaFrame.data();
    playback->av_packet_->size = static_cast<int>(1 + ((paddingStart + packetInfo.currentFrameSize) / 8) +
                                                  (((paddingStart + packetInfo.currentFrameSize) % 8) ? 1 : 0));

    auto paddingEnd = playback->av_packet_->size * 8 - (8 + paddingStart + packetInfo.currentFrameSize);
    playback->xmaFrame[0] = ((paddingStart & 7) << 5) | ((paddingEnd & 7) << 2);

    auto ret = avcodec_send_packet(playback->codec_ctx, playback->av_packet_);
    if (ret < 0) {
        debug_printf("Error sending packet for decoding: %s\n", av_err2str(ret));
    }

    ret = avcodec_receive_frame(playback->codec_ctx, playback->av_frame_);
    if (ret < 0) {
        debug_printf("Error receiving frame from decoder: %s\n", av_err2str(ret));
    }

    constexpr float scale = (1 << 15) - 1;
    auto out = reinterpret_cast<int16_t *>(playback->rawFrame.data());
    auto samples = reinterpret_cast<const uint8_t **>(&playback->av_frame_->data);

    uint32_t o = 0;
    if (playback->av_frame_->nb_samples != 0) {
        for (uint32_t i = 0; i < kSamplesPerFrame; i++) {
            for (uint32_t j = 0; j < playback->av_frame_->ch_layout.nb_channels; j++) {
                // Select the appropriate array based on the current channel.
                auto in = reinterpret_cast<const float *>(samples[j]);

                // Raw samples sometimes aren't within [-1, 1]
                float scaledSample = clamp_float(in[i], -1.0f, 1.0f) * scale;

                // Convert the sample and output it in big endian.
                auto sample = static_cast<int16_t>(scaledSample);
                out[o++] = ByteSwap(sample);
            }
        }
    }

    playback->currentFrameRemainingSubframes = (4 - std::min<uint32_t>(playback->numSubframesToSkip, 4))
                                           * playback->channelCount;
    playback->numSubframesToSkip = 0;

    if (!packetInfo.IsLastFrameInPacket()) {
        const uint32_t nextFrameOffset = (playback->inputBufferReadOffset + bitsToCopy) % kBitsPerPacket;

        playback->inputBufferReadOffset = (packetIndex * kBitsPerPacket) + nextFrameOffset;
        return;
    }

    uint32_t nextInputOffset = GetNextPacketReadOffset(currentInputBuffer, nextPacketIndex,
                                                       currentInputPacketCount);

    if (nextInputOffset == kBitsPerPacketHeader) {
        SwapInputBuffer(playback);

        // We're at start of next buffer
        // Any frames in this packet decoder should go to the first frame in the packet.
        // If it doesn't have any frames, then it should immediately go to the next packet.
        if (playback->IsAnyInputBufferValid()) {
            nextInputOffset = GetPacketFrameOffset((uint8_t *)g_memory.Translate(
                    playback->GetCurrentInputBufferAddress()));

            if (nextInputOffset > kMaxFrameSizeinBits) {
                SwapInputBuffer(playback);
                return;
            }
        }
    }

    playback->inputBufferReadOffset = nextInputOffset;
}

void Consume(XmaPlayback *playback) {
    if (!playback->currentFrameRemainingSubframes) {
        return;
    }

    const int8_t subframesToWrite = std::min((int8_t)playback->currentFrameRemainingSubframes,
                                             (int8_t)playback->subframes);

    const int8_t rawFrameReadOffset = ((kBytesPerFrameChannel / kOutputBytesPerBlock) * playback->channelCount)
                                      - playback->currentFrameRemainingSubframes;

    playback->outputRb.Write(playback->rawFrame.data() +
                             (kOutputBytesPerBlock * rawFrameReadOffset),
                             subframesToWrite * kOutputBytesPerBlock);
    playback->remainingSubframeBlocksInOutputBuffer -= subframesToWrite;
    playback->currentFrameRemainingSubframes -= subframesToWrite;
}

void DecoderThreadFunc(void* data) {
    auto* playback = static_cast<XmaPlayback*>(data);
    std::unique_lock lock(playback->mutex);

    while (playback->isRunning) {
        playback->cv.wait(lock, [&] {
            return !playback->isRunning || (playback->outputBufferValid == 1 &&
                                            playback->IsAnyInputBufferValid());
        });

        if (!playback->isRunning)
            break;

        const int32_t minimumSubframeDecodeCount = (playback->subframes * playback->channelCount) - 1;

        size_t outputCapacity = playback->outputBufferBlockCount * kOutputBytesPerBlock;

        const uint32_t outputReadOffset = playback->outputBufferReadOffset * kOutputBytesPerBlock;
        const uint32_t outputWriteOffset = playback->outputBufferWriteOffset * kOutputBytesPerBlock;

        playback->outputRb = RingBuffer(playback->outputBuffer, outputCapacity);
        playback->outputRb.set_read_offset(outputReadOffset);
        playback->outputRb.set_write_offset(outputWriteOffset);
        playback->remainingSubframeBlocksInOutputBuffer = (int32_t)playback->outputRb.write_count()
                                                          / kOutputBytesPerBlock;

        if (minimumSubframeDecodeCount > playback->remainingSubframeBlocksInOutputBuffer) {
            playback->outputBufferValid = 0;
            continue;
        }

        Decode(playback);
        Consume(playback);

        playback->outputBufferWriteOffset = playback->outputRb.write_offset() / kOutputBytesPerBlock;

        if (playback->outputRb.write_offset() != outputWriteOffset && playback->outputRb.empty()) {
            playback->outputBufferValid = 0;
        }

        lock.unlock();
        std::this_thread::yield();
        lock.lock();
    }
}

uint32_t XMAPlaybackCreate(uint32_t streams, XMAPLAYBACKINIT *init, uint32_t flags, be<uint32_t> *outPlayback) {
    const auto xmaPlayback = g_userHeap.AllocPhysical<XmaPlayback>(
            init->sampleRate.get(), init->outputBufferSize.get(), init->channelCount,
            init->subframes);
    *outPlayback = g_memory.MapVirtual(xmaPlayback);

    return 0;
}

uint32_t XMAPlaybackRequestModifyLock(XmaPlayback *playback) {
    std::lock_guard lock(playback->mutex);
    playback->isLocked = true;

    return 0;
}

uint32_t XMAPlaybackWaitUntilModifyLockObtained(XmaPlayback *playback) {
    std::unique_lock lock(playback->mutex);
    playback->cv.wait(lock, [&playback] { return playback->isLocked.load(); });

    return 0;
}

uint32_t XMAPlaybackQueryReadyForMoreData(XmaPlayback *playback, uint32_t stream) {
    return playback->inputBuffer1Valid == 0 || playback->inputBuffer2Valid == 0;
}

uint32_t XMAPlaybackIsIdle(XmaPlayback *playback, uint32_t stream) {
    return playback->inputBuffer1Valid == 0 && playback->inputBuffer2Valid == 0;
}

uint32_t XMAPlaybackQueryContextsAllocated(XmaPlayback *playback) {
    if (!playback) {
        return 0;
    }

    return 1;
}

uint32_t XMAPlaybackResumePlayback(XmaPlayback *playback) {
    std::lock_guard lock(playback->mutex);
    playback->isLocked = false;
    playback->cv.notify_all();

    return 0;
}

uint32_t XMAPlaybackQueryInputDataPending(XmaPlayback *playback, uint32_t stream, uint32_t data) {
    if (playback->inputBuffer1Valid && playback->inputBuffer1 == data) {
        return 1;
    }

    if (playback->inputBuffer2Valid && playback->inputBuffer2 == data) {
        return 1;
    }

    return 0;
}

uint32_t XMAPlaybackGetErrorBits(XmaPlayback *playback, uint32_t stream) {
    return 0;
}

uint32_t XMAPlaybackSubmitData(XmaPlayback *playback, uint32_t stream, uint32_t data, uint32_t dataSize) {
    if (!playback->isLocked) {
        return 1;
    }

    std::lock_guard lock(playback->mutex);
    uint32_t packetCount = dataSize >> 11;

    uint32_t validBuffer = playback->inputBuffer1Valid | (playback->inputBuffer2Valid << 1);

    if (playback->inputBuffer1Valid == 1) {
        if (playback->inputBuffer2Valid == 1) {
            return 0x80070005;
        }
        playback->inputBuffer2 = data;
        playback->inputBuffer2Size = packetCount & 0xFFF;

        playback->inputBuffer2Valid = 1;
    } else {
        playback->inputBuffer1 = data;
        playback->inputBuffer1Size = packetCount & 0xFFF;

        playback->inputBuffer1Valid = 1;
    }

    if (!validBuffer) {
        playback->currentBuffer = 0;

        uint8_t *currentInputBuffer = (uint8_t *)g_memory.Translate(data);
        uint32_t frameOffset = XMAPlaybackGetFrameOffsetFromPacketHeader(*currentInputBuffer);

        if (frameOffset) {
            playback->inputBufferReadOffset = frameOffset & 0x3FFFFFF;
        }
    }

    playback->cv.notify_all();
    return 0;
}

uint32_t XMAPlaybackQueryAvailableData(XmaPlayback *playback, uint32_t stream) {
    if (!playback->isLocked) {
        return 0;
    }

    std::lock_guard lock(playback->mutex);

    uint32_t partialBytesRead = playback->partialBytesRead;
    uint32_t writeBufferOffsetRead = playback->outputBufferReadOffset & 0x1F;
    uint32_t offsetWrite = playback->outputBufferWriteOffset;
    uint32_t sizeWrite = playback->outputBufferBlockCount & 0x1F;

    uint32_t availableBytes = 0;
    uint32_t isValidWrite = playback->outputBufferValid;

    if (partialBytesRead) {
        availableBytes = 256 - partialBytesRead;
        writeBufferOffsetRead++;
        isValidWrite = 1;
    }

    uint32_t availableBlocks = 0;
    if (offsetWrite <= writeBufferOffsetRead) {
        if (offsetWrite < writeBufferOffsetRead || !isValidWrite) {
            availableBlocks = sizeWrite - writeBufferOffsetRead;
        }
    } else {
        availableBlocks = offsetWrite - writeBufferOffsetRead;
    }

    uint32_t totalBytes = (availableBlocks << 8) + availableBytes;
    uint32_t bytesPerSample = playback->channelCount;

    return totalBytes >> bytesPerSample;
}


uint32_t XMAPlaybackAccessDecodedData(XmaPlayback *playback, uint32_t stream, be<uint32_t>* data) {
    if (!playback->isLocked)
        return 0;

    std::lock_guard lock(playback->mutex);

    uint32_t partialBytesRead = playback->partialBytesRead;
    uint32_t addr = playback->outputBufferVirtual +
                    ((playback->outputBufferReadOffset << 8) & 0x1F00) +
                    partialBytesRead;

    *data = addr;

    uint32_t writeBufferOffsetRead = playback->outputBufferReadOffset & 0x1F;
    uint32_t offsetWrite = playback->outputBufferWriteOffset;
    uint32_t sizeWrite = playback->outputBufferBlockCount & 0x1F;

    uint32_t availableBytes = 0;
    uint32_t isValidWrite = playback->outputBufferValid;

    if (partialBytesRead) {
        availableBytes = 256 - partialBytesRead;
        writeBufferOffsetRead++;
        isValidWrite = 1;
    }

    uint32_t availableBlocks = 0;
    if (offsetWrite <= writeBufferOffsetRead) {
        if (offsetWrite < writeBufferOffsetRead || !isValidWrite) {
            availableBlocks = sizeWrite - writeBufferOffsetRead;
        }
    } else {
        availableBlocks = offsetWrite - writeBufferOffsetRead;
    }

    uint32_t totalBytes = (availableBlocks << 8) + availableBytes;
    uint32_t bytesPerSample = playback->channelCount;

    return totalBytes >> bytesPerSample;
}

uint32_t XMAPlaybackConsumeDecodedData(XmaPlayback *playback, uint32_t stream, uint32_t maxSamples, be<uint32_t>* data) {
    if (!playback->isLocked) {
        return 0;
    }

    std::lock_guard lock(playback->mutex);

    uint32_t totalBytes = 0;
    uint32_t partialBytesRead = playback->partialBytesRead;
    uint32_t addr = playback->outputBufferVirtual +
                    ((playback->outputBufferReadOffset << 8) & 0x1F00) +
                    partialBytesRead;

    *data = addr;
    uint32_t bytesPerSample = playback->channelCount;
    uint32_t bytesDesired = maxSamples << bytesPerSample;
    if (partialBytesRead) {
        if (bytesDesired < 256 - partialBytesRead) {
            totalBytes = bytesDesired;
            playback->partialBytesRead += bytesDesired;
            bytesDesired = 0;
        } else {
            totalBytes = 256 - partialBytesRead;
            bytesDesired -= 256 - partialBytesRead;
            playback->partialBytesRead = 0;

            uint32_t writeIndex = playback->outputBufferReadOffset + 1 & 0x1F;

            if (writeIndex >= (playback->outputBufferBlockCount & 0x1F)) {
                writeIndex = 0;
            }

            playback->outputBufferReadOffset = writeIndex;
            playback->outputBufferValid = 1;
        }
    }

    uint32_t writeIndex = playback->outputBufferReadOffset;
    uint32_t blocksToProcess = bytesDesired >> 8;
    uint32_t availableBlocks = 0;

    uint32_t writeSize = playback->outputBufferBlockCount;
    if (writeSize <= writeIndex) {
        if (writeSize < writeIndex || !playback->outputBufferValid) {
            availableBlocks = (playback->outputBufferBlockCount & 0x1F) - writeIndex;
        }
    } else {
        availableBlocks = writeSize - writeIndex;
    }

    if (blocksToProcess) {
        if (blocksToProcess > availableBlocks) {
            blocksToProcess = availableBlocks;
        }

        totalBytes += blocksToProcess << 8;
        availableBlocks -= blocksToProcess;
        writeIndex = (writeIndex + blocksToProcess) & 0x1F;

        if (writeIndex >= (playback->outputBufferBlockCount & 0x1F)) {
            writeIndex = 0;
        }

        playback->outputBufferReadOffset = writeIndex;
        playback->outputBufferValid = 1;
    }

    uint32_t remainingBytes = bytesDesired & 0xFF;
    if (remainingBytes && availableBlocks) {
        totalBytes += remainingBytes;
        playback->partialBytesRead = remainingBytes;
    }

    playback->cv.notify_all();

    uint32_t samplesConsumed = totalBytes >> bytesPerSample;
    playback->streamPosition += samplesConsumed;

    return samplesConsumed;
}

uint32_t XMAPlaybackQueryModifyLockObtained(XmaPlayback *playback) {
    debug_printf("XMAPlaybackQueryModifyLockObtained %x\n", playback);
    return playback->isLocked.load();
}

uint32_t XMAPlaybackDestroy(XmaPlayback *playback) {
    debug_printf("XMAPlaybackDestroy %x\n", playback);

    playback->~XmaPlayback();
    g_userHeap.Free(playback);

    return 0;
}

uint32_t XMAPlaybackFlushData(XmaPlayback *playback) {
    debug_printf("XMAPlaybackFlushData %x\n", playback);
    // __builtin_debugtrap();
    return 0;
}

struct XMAPLAYBACKLOOP {
    be<uint32_t> loopStartOffset;
    be<uint32_t> loopEndOffset;
    uint8_t loopSubframeEnd;
    uint8_t loopSubframeSkip;
    uint8_t numLoops;
    uint8_t reserved;
};

uint32_t XmaPlaybackSetLoop(XmaPlayback *playback, uint32_t streamIndex, XMAPLAYBACKLOOP *loop) {
    std::lock_guard lock(playback->mutex);

    playback->numLoops = loop->numLoops;
    playback->loopSubframeEnd = loop->loopSubframeEnd;
    playback->loopSubframeSkip = loop->loopSubframeSkip;
    playback->loopStartOffset = loop->loopStartOffset.get() & 0x3FFFFFF;
    playback->loopEndOffset = loop->loopEndOffset.get() & 0x3FFFFFF;

    return 0;
}

uint32_t XMAPlaybackGetRemainingLoopCount(XmaPlayback *playback) {
    debug_printf("XMAPlaybackGetRemainingLoopCount %x\n", playback);
    __builtin_debugtrap();
    return 0;
}

uint32_t XMAPlaybackGetStreamPosition(XmaPlayback *playback) {
    return playback->streamPosition;
}

uint32_t XMAPlaybackSetDecodePosition(XmaPlayback *playback, uint32_t streamIndex, uint32_t bitOffset,
                                      uint32_t subframe) {
    std::lock_guard lock(playback->mutex);

    playback->inputBufferReadOffset = bitOffset & 0x3FFFFFF;
    playback->numSubframesToSkip = subframe & 0x7;
    return 0;
}

uint32_t XMAPlaybackRewindDecodePosition(XmaPlayback *playback, uint32_t streamIndex, uint32_t numSamples) {
    std::lock_guard lock(playback->mutex);

    uint32_t shift = 7 - (1 != 0);
    uint32_t adjustedSamples = numSamples >> shift;
    uint32_t writeSize = playback->outputBufferBlockCount & 0x1F;

    uint32_t newOffset;
    if (adjustedSamples >= writeSize) {
        newOffset = playback->inputBufferReadOffset & 0x3FFFFFF;
        playback->outputBufferValid = 1;
        playback->outputBufferWriteOffset = newOffset >> 27;
        return 0;
    }

    newOffset = playback->outputBufferWriteOffset - adjustedSamples + writeSize & 0x1F;

    if (newOffset >= writeSize) {
        newOffset -= writeSize;
    }

    playback->outputBufferValid = 1;
    playback->outputBufferWriteOffset = newOffset;

    return 1;
}

uint32_t XMAPlaybackQueryCurrentPosition(XmaPlayback *playback) {
    debug_printf("XMAPlaybackQueryCurrentPosition %x\n", playback);
    __builtin_debugtrap();
    return 0;
}

GUEST_FUNCTION_HOOK(sub_8255C090, XMAPlaybackCreate);
GUEST_FUNCTION_HOOK(sub_8255CC48, XMAPlaybackRequestModifyLock);
GUEST_FUNCTION_HOOK(sub_8255CCC8, XMAPlaybackWaitUntilModifyLockObtained);
GUEST_FUNCTION_HOOK(sub_8255C4D0, XMAPlaybackQueryReadyForMoreData);
GUEST_FUNCTION_HOOK(sub_8255C520, XMAPlaybackIsIdle);
GUEST_FUNCTION_HOOK(sub_8255C388, XMAPlaybackQueryContextsAllocated);
GUEST_FUNCTION_HOOK(sub_8255CF10, XMAPlaybackResumePlayback);
GUEST_FUNCTION_HOOK(sub_8255C470, XMAPlaybackQueryInputDataPending);
GUEST_FUNCTION_HOOK(sub_8255C9A0, XMAPlaybackGetErrorBits);
GUEST_FUNCTION_HOOK(sub_8255C398, XMAPlaybackSubmitData);
GUEST_FUNCTION_HOOK(sub_8255C578, XMAPlaybackQueryAvailableData);
GUEST_FUNCTION_HOOK(sub_8255C7A8, XMAPlaybackAccessDecodedData);
GUEST_FUNCTION_HOOK(sub_8255C5F0, XMAPlaybackConsumeDecodedData);
GUEST_FUNCTION_HOOK(sub_8255CD90, XMAPlaybackQueryModifyLockObtained);
GUEST_FUNCTION_HOOK(sub_8255C8D8, XMAPlaybackFlushData);
GUEST_FUNCTION_HOOK(sub_8255C9D8, XmaPlaybackSetLoop);
GUEST_FUNCTION_HOOK(sub_8255CA50, XMAPlaybackGetRemainingLoopCount);
GUEST_FUNCTION_HOOK(sub_8255CA90, XMAPlaybackGetStreamPosition);
GUEST_FUNCTION_HOOK(sub_8255CB20, XMAPlaybackSetDecodePosition);
GUEST_FUNCTION_HOOK(sub_8255C850, XMAPlaybackRewindDecodePosition);
GUEST_FUNCTION_HOOK(sub_8255CAB0, XMAPlaybackQueryCurrentPosition);
GUEST_FUNCTION_HOOK(sub_8255C2C0, XMAPlaybackDestroy);
