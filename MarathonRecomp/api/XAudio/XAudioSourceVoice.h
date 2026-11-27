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

struct XAUDIOXMAFORMAT
{
    MARATHON_INSERT_PADDING(0x08);
};

struct XAUDIOSOURCEFORMAT
{
    uint8_t SampleType;
    MARATHON_INSERT_PADDING(0x03);

    union
    {
        uint8_t NumStreams; // XMA
        uint8_t ChannelCount; // PCM
    };

    union
    {
        XAUDIOXMAFORMAT Stream[6]; // XMA
        be<uint32_t> SampleRate; // PCM
    };
};

struct XAUDIOSOURCEVOICEINIT
{
    XAUDIOSOURCEFORMAT Format;
    uint8_t Category;
    uint8_t MaxOutputVoiceCount;
    uint8_t MaxChannelMapEntryCount;
    uint8_t MaxPacketCount;
    be<float> MaxPitchShift;
    uint8_t Flags;
    xpointer<const void*> pEffectChain;
    xpointer<const void*> pVoiceOutput;
    be<uint32_t> pfnProcessCallback;
    be<uint32_t> pfnPacketCompletionCallback;
    be<uint32_t> pfnPacketLoopCallback;
    xpointer<void> pContext;
};

MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_pVftable, 0x00);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_ReferenceCount, 0x04);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_pParent, 0x08);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_IsMuted, 0x45);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_pVoice, 0x4C);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_Volume, 0x8C);
MARATHON_ASSERT_OFFSETOF(XAudioSourceVoice, m_VolumeFlags, 0x90);
MARATHON_ASSERT_SIZEOF(XAudioSourceVoice, 0x94);

MARATHON_ASSERT_SIZEOF(XAUDIOXMAFORMAT, 0x08);

MARATHON_ASSERT_OFFSETOF(XAUDIOSOURCEFORMAT, SampleType, 0x00);
MARATHON_ASSERT_OFFSETOF(XAUDIOSOURCEFORMAT, NumStreams, 0x04);
MARATHON_ASSERT_OFFSETOF(XAUDIOSOURCEFORMAT, ChannelCount, 0x04);
MARATHON_ASSERT_OFFSETOF(XAUDIOSOURCEFORMAT, Stream, 0x08);
MARATHON_ASSERT_OFFSETOF(XAUDIOSOURCEFORMAT, SampleRate, 0x08);
MARATHON_ASSERT_SIZEOF(XAUDIOSOURCEFORMAT, 0x38);

MARATHON_ASSERT_OFFSETOF(XAUDIOSOURCEVOICEINIT, Format, 0x00);
MARATHON_ASSERT_SIZEOF(XAUDIOSOURCEVOICEINIT, 0x5C);
