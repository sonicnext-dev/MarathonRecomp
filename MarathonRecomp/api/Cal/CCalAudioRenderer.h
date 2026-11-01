#pragma once

#include <Marathon.inl>
#include <Cal/CCalRenderer.h>
#include <XAudio/XAudioSourceVoice.h>

class CCalAudioRenderer : public CCalRenderer
{
public:
    xpointer<XAudioSourceVoice> m_pSourceVoice;
    MARATHON_INSERT_PADDING(0x594);
};

MARATHON_ASSERT_OFFSETOF(CCalAudioRenderer, m_pSourceVoice, 0x38);
MARATHON_ASSERT_SIZEOF(CCalAudioRenderer, 0x5D0);
