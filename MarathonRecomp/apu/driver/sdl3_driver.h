#pragma once

#include <apu/audio.h>
#include <SDL3/SDL_audio.h>

class SDL3AudioDriver : public AudioDriver
{
public:
    explicit SDL3AudioDriver(bool surround);
    ~SDL3AudioDriver() override;

    int GetQueuedBytes() override;
    void Put(const float* pSamples, uint32_t frameCount) override;

private:
    SDL_AudioStream* m_pAudioStream{};
};
