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
