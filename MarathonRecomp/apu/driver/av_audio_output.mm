#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>

#include "av_audio_output.h"
#include <apu/audio.h>

// How far ahead of the playback clock audio is kept.
static constexpr double LEAD_SECONDS = 0.2;
static constexpr int64_t LEAD_FRAMES = int64_t(LEAD_SECONDS * XAUDIO_SAMPLES_HZ);
static constexpr int BYTES_PER_FRAME = XAUDIO_NUM_CHANNELS * sizeof(float);

struct AVAudioOutput::Impl
{
    AVSampleBufferAudioRenderer* Renderer{};
    AVSampleBufferRenderSynchronizer* Synchronizer{};
    CMAudioFormatDescriptionRef Format{};
    int64_t NextFrame{};
    bool Started{};
};

AVAudioOutput::AVAudioOutput() : m_pImpl(std::make_unique<Impl>())
{
    AudioStreamBasicDescription asbd{};
    asbd.mSampleRate = XAUDIO_SAMPLES_HZ;
    asbd.mFormatID = kAudioFormatLinearPCM;
    asbd.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
    asbd.mChannelsPerFrame = XAUDIO_NUM_CHANNELS;
    asbd.mBitsPerChannel = XAUDIO_SAMPLE_BITS;
    asbd.mFramesPerPacket = 1;
    asbd.mBytesPerFrame = asbd.mBytesPerPacket = BYTES_PER_FRAME;

    // XAudio's channel order
    AudioChannelLayout layout{};
    layout.mChannelLayoutTag = kAudioChannelLayoutTag_WAVE_5_1_A;

    if (CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &asbd, sizeof(layout), &layout, 0, nullptr, nullptr, &m_pImpl->Format) != noErr)
        return;

    m_pImpl->Renderer = [[AVSampleBufferAudioRenderer alloc] init];

    if (@available(macOS 12.0, *))
        m_pImpl->Renderer.allowedAudioSpatializationFormats = AVAudioSpatializationFormatMonoStereoAndMultichannel;

    m_pImpl->Synchronizer = [[AVSampleBufferRenderSynchronizer alloc] init];
    [m_pImpl->Synchronizer addRenderer:m_pImpl->Renderer];
}

AVAudioOutput::~AVAudioOutput()
{
    if (m_pImpl->Synchronizer)
    {
        m_pImpl->Synchronizer.rate = 0.0f;
        [m_pImpl->Synchronizer removeRenderer:m_pImpl->Renderer atTime:kCMTimeInvalid completionHandler:nil];
        [m_pImpl->Synchronizer release];
    }

    [m_pImpl->Renderer flush];
    [m_pImpl->Renderer release];

    if (m_pImpl->Format)
        CFRelease(m_pImpl->Format);
}

bool AVAudioOutput::IsValid() const
{
    return m_pImpl->Format && m_pImpl->Renderer && m_pImpl->Synchronizer;
}

int AVAudioOutput::GetQueuedBytes()
{
    auto currentFrame = int64_t(CMTimeGetSeconds(m_pImpl->Synchronizer.currentTime) * XAUDIO_SAMPLES_HZ);

    // Fell behind the clock, skip ahead.
    if (m_pImpl->Started && m_pImpl->NextFrame < currentFrame)
        m_pImpl->NextFrame = currentFrame + LEAD_FRAMES;

    return int(std::max<int64_t>(0, m_pImpl->NextFrame - currentFrame - LEAD_FRAMES) * BYTES_PER_FRAME);
}

void AVAudioOutput::Put(const float* pSamples, uint32_t frameCount)
{
    size_t size = frameCount * BYTES_PER_FRAME;
    CMBlockBufferRef block{};

    if (CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault, nullptr, size, kCFAllocatorDefault, nullptr, 0, size, kCMBlockBufferAssureMemoryNowFlag, &block) != kCMBlockBufferNoErr)
        return;

    CMBlockBufferReplaceDataBytes(pSamples, block, 0, size);

    CMSampleBufferRef sampleBuffer{};
    auto status = CMAudioSampleBufferCreateReadyWithPacketDescriptions(kCFAllocatorDefault, block, m_pImpl->Format,
        frameCount, CMTimeMake(m_pImpl->NextFrame, XAUDIO_SAMPLES_HZ), nullptr, &sampleBuffer);

    CFRelease(block);

    if (status != noErr)
        return;

    [m_pImpl->Renderer enqueueSampleBuffer:sampleBuffer];
    CFRelease(sampleBuffer);

    m_pImpl->NextFrame += frameCount;

    // Start the clock once the lead is buffered, so playback doesn't begin starved.
    if (!m_pImpl->Started && m_pImpl->NextFrame >= LEAD_FRAMES)
    {
        [m_pImpl->Synchronizer setRate:1.0f time:kCMTimeZero];
        m_pImpl->Started = true;
    }
}
