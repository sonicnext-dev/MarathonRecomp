#pragma once

#include <Marathon.inl>
#include <Cal/CCalAudioRenderer.h>
#include <Cal/CCalBase.h>

class CCalChannel : public CCalBase
{
public:
    MARATHON_INSERT_PADDING(0x0C);
    xpointer<CCalAudioRenderer> m_pAudioRenderer;
    MARATHON_INSERT_PADDING(0x11C);
};

MARATHON_ASSERT_OFFSETOF(CCalChannel, m_pAudioRenderer, 0x38);
MARATHON_ASSERT_SIZEOF(CCalChannel, 0x158);
