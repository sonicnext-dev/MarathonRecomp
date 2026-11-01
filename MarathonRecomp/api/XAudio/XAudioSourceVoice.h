#pragma once

#include <Marathon.inl>
#include <XAudio/XAudioVoice.h>

class XAudioSourceVoice
{
public:
    xpointer<void> m_pVftable; // 0x82043278
    be<uint32_t> m_ReferenceCount;
    xpointer<void> m_pParent;
    MARATHON_INSERT_PADDING(0x39);
    bool m_IsMuted;
    MARATHON_INSERT_PADDING(0x04);
    xpointer<XAudioVoice> m_pVoice;
    MARATHON_INSERT_PADDING(0x3C);
    be<float> m_Volume;
    uint8_t m_VolumeFlags;
};

MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_pVftable, 0x00);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_ReferenceCount, 0x04);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_pParent, 0x08);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_IsMuted, 0x45);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_pVoice, 0x4C);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_Volume, 0x8C);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_VolumeFlags, 0x90);
MARATHON_ASSERT_SIZEOF(XAudioSourceVoice, 0x94);
