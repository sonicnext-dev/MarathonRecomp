#pragma once

#include <Marathon.inl>
#include <Cal/CCalBase.h>
#include <Cal/CCalChannel.h>

class CCalMoviePlayer : public CCalBase
{
public:
    xpointer<CCalChannel> m_pChannel;
    MARATHON_INSERT_PADDING(0xF8);
};

MARATHON_ASSERT_OFFSETOF(CCalMoviePlayer, m_pChannel, 0x2C);
MARATHON_ASSERT_SIZEOF(CCalMoviePlayer, 0x128);
