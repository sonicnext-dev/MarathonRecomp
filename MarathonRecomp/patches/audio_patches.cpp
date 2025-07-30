#include <user/config.h>
#include <kernel/function.h>
#include <os/media.h>
#include <os/version.h>
#include <patches/audio_patches.h>

int AudioPatches::m_isAttenuationSupported = -1;

bool AudioPatches::CanAttenuate()
{
#if _WIN32
    if (m_isAttenuationSupported >= 0)
        return m_isAttenuationSupported;

    auto version = os::version::GetOSVersion();

    m_isAttenuationSupported = version.Major >= 10 && version.Build >= 17763;

    return m_isAttenuationSupported;
#else
    return false;
#endif
}

// TODO (Hyper): implement music attenuation.
void AudioPatches::Update(float deltaTime) {}
