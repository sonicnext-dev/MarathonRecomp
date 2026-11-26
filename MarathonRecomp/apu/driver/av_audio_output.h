#pragma once
#include <memory>

class AVAudioOutput
{
public:
    AVAudioOutput();
    ~AVAudioOutput();

    bool IsValid() const;
    int GetQueuedBytes();
    void Put(const float* pSamples, uint32_t frameCount);

private:
    struct Impl;
    std::unique_ptr<Impl> m_pImpl;
};
