#pragma once

#include <cstdint>

#define XAUDIO_SAMPLES_HZ 48000
#define XAUDIO_NUM_CHANNELS 6
#define XAUDIO_SAMPLE_BITS 32

// Number of samples in a frame
#define XAUDIO_NUM_SAMPLES 256

class AudioDriver
{
public:
    virtual ~AudioDriver() = default;

    virtual int GetQueuedBytes() = 0;
    virtual void Put(const float* pSamples, uint32_t frameCount) = 0;

    int m_channels{};
};

void XAudioInitializeSystem();
void XAudioConfigValueChangedCallback(class IConfigDef* configDef);
uint32_t XAudioRegisterRenderDriverClient(be<uint32_t>* callback, be<uint32_t>* driver);
uint32_t XAudioUnregisterRenderDriverClient(uint32_t driver);
uint32_t XAudioSubmitRenderDriverFrame(uint32_t driver, void* samples);
