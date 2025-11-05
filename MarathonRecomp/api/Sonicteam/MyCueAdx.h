#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyCue.h>
#include <Sonicteam/MyCueAttenuate.h>
#include <Sonicteam/sonicXmaPlayer.h>

class MyCueAdx : public MyCue, public MyCueAttenuate
{
public:
    MARATHON_INSERT_PADDING(0x0C);
    be<float> m_Field50;
    be<float> m_Field54;
    xpointer<Sonicteam::sonicXmaPlayer> m_pXmaPlayer;
    bool m_IsPaused;
    MARATHON_INSERT_PADDING(3);
};

MARATHON_ASSERT_OFFSETOF(MyCueAdx, m_Field50, 0x50);
MARATHON_ASSERT_OFFSETOF(MyCueAdx, m_Field54, 0x54);
MARATHON_ASSERT_OFFSETOF(MyCueAdx, m_pXmaPlayer, 0x58);
MARATHON_ASSERT_OFFSETOF(MyCueAdx, m_IsPaused, 0x5C);
MARATHON_ASSERT_SIZEOF(MyCueAdx, 0x60);
