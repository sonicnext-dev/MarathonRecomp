#include "avfoundation_driver.h"

#include <apu/audio.h>
#include <os/logger.h>

#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>

// How far ahead of the playback clock audio is kept.
static constexpr double LEAD_SECONDS = 0.2;
static constexpr int64_t LEAD_FRAMES = int64_t(LEAD_SECONDS * XAUDIO_SAMPLES_HZ);

struct AVFoundationDriver::Impl
{
    AVSampleBufferAudioRenderer* Renderer{};
    AVSampleBufferRenderSynchronizer* Synchronizer{};
    CMAudioFormatDescriptionRef Format{};
    int64_t NextFrame{};
    bool Started{};
};

AVFoundationDriver::AVFoundationDriver(bool surround)
    : m_pImpl(std::make_unique<Impl>())
{
    m_channels = surround ? XAUDIO_NUM_CHANNELS : 2;

    AudioStreamBasicDescription asbd{};
    asbd.mSampleRate = XAUDIO_SAMPLES_HZ;
    asbd.mFormatID = kAudioFormatLinearPCM;
    asbd.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
    asbd.mChannelsPerFrame = m_channels;
    asbd.mBitsPerChannel = XAUDIO_SAMPLE_BITS;
    asbd.mFramesPerPacket = 1;
    asbd.mBytesPerFrame = asbd.mBytesPerPacket = m_channels * sizeof(float);

    // XAudio's channel order
    AudioChannelLayout layout{};
    layout.mChannelLayoutTag = surround ? kAudioChannelLayoutTag_WAVE_5_1_A : kAudioChannelLayoutTag_Stereo;

    const auto err = CMAudioFormatDescriptionCreate(kCFAllocatorDefault, &asbd, sizeof(layout), &layout, 0, nullptr, nullptr, &m_pImpl->Format);
    if (err != noErr)
    {
        LOGFN_ERROR("Failed to create audio format description: {}", err);
        return;
    }

    m_pImpl->Renderer = [AVSampleBufferAudioRenderer new];
    if (!m_pImpl->Renderer)
    {
        LOGN_ERROR("Failed to create audio renderer");
        return;
    }

    m_pImpl->Renderer.allowedAudioSpatializationFormats = AVAudioSpatializationFormatMultichannel;

    m_pImpl->Synchronizer = [AVSampleBufferRenderSynchronizer new];
    if (!m_pImpl->Renderer)
    {
        LOGN_ERROR("Failed to create audio synchronizer");
        return;
    }

    [m_pImpl->Synchronizer addRenderer:m_pImpl->Renderer];
}

AVFoundationDriver::~AVFoundationDriver() {
    if (m_pImpl->Synchronizer)
    {
        m_pImpl->Synchronizer.rate = 0.0f;
        [m_pImpl->Synchronizer removeRenderer:m_pImpl->Renderer atTime:kCMTimeInvalid completionHandler:nil];
        [m_pImpl->Synchronizer release];
    }

    if (m_pImpl->Renderer)
    {
        [m_pImpl->Renderer flush];
        [m_pImpl->Renderer release];
    }

    if (m_pImpl->Format)
        CFRelease(m_pImpl->Format);
}

int AVFoundationDriver::GetQueuedBytes()
{
    if (!m_pImpl->Renderer || !m_pImpl->Synchronizer)
        return -1;

    const auto currentFrame = int64_t(CMTimeGetSeconds(m_pImpl->Synchronizer.currentTime) * XAUDIO_SAMPLES_HZ);

    // Fell behind the clock, skip ahead.
    if (m_pImpl->Started && m_pImpl->NextFrame < currentFrame)
        m_pImpl->NextFrame = currentFrame + LEAD_FRAMES;

    return int(std::max<int64_t>(0, m_pImpl->NextFrame - currentFrame - LEAD_FRAMES) * m_channels * sizeof(float));
}

void AVFoundationDriver::Put(const float* pSamples, uint32_t frameCount)
{
    if (!m_pImpl->Renderer || !m_pImpl->Synchronizer)
        return;

    const auto size = frameCount * m_channels * sizeof(float);
    CMBlockBufferRef block{};

    if (CMBlockBufferCreateWithMemoryBlock(kCFAllocatorDefault, nullptr, size, kCFAllocatorDefault, nullptr, 0, size, kCMBlockBufferAssureMemoryNowFlag, &block) != kCMBlockBufferNoErr)
        return;

    CMBlockBufferReplaceDataBytes(pSamples, block, 0, size);

    CMSampleBufferRef sampleBuffer{};
    const auto status = CMAudioSampleBufferCreateReadyWithPacketDescriptions(kCFAllocatorDefault, block, m_pImpl->Format,
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
