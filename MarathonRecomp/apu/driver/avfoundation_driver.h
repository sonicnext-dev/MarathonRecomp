#pragma once

#include <apu/audio.h>
#include <memory>

class AVFoundationDriver : public AudioDriver
{
public:
    explicit AVFoundationDriver(bool surround);
    ~AVFoundationDriver() override;

    int GetQueuedBytes() override;
    void Put(const float* pSamples, uint32_t frameCount) override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
