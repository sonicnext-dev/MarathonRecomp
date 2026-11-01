#pragma once

#include <Marathon.inl>

class XAudioVoice
{
public:
    struct Vftable
    {
        MARATHON_INSERT_PADDING(0x58);
        be<uint32_t> fpSetVolume;
        MARATHON_INSERT_PADDING(0x1C);
    };

    xpointer<Vftable> m_pVftable; // 0x82044640
    MARATHON_INSERT_PADDING(0x10C);

    int SetVolume(float in_volume)
    {
        return GuestToHostFunction<int>(m_pVftable->fpSetVolume, this, in_volume);
    }
};

MARATHON_ASSERT_OFFSETOF(XAudioVoice::Vftable, fpSetVolume, 0x58);
MARATHON_ASSERT_SIZEOF(XAudioVoice::Vftable, 0x78);

MARATHON_ASSERT_OFFSETOF(XAudioVoice, m_pVftable, 0x00);
MARATHON_ASSERT_SIZEOF(XAudioVoice, 0x110);
