#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyCue.h>
#include <Sonicteam/MyCueAttenuate.h>
#include <Sonicteam/sonicXmaPlayer.h>

namespace
{
    class MyCueAdx : public MyCue, public MyCueAttenuate
    {
    public:
        MARATHON_INSERT_PADDING(0x2C);
        xpointer<Sonicteam::sonicXmaPlayer> m_pXmaPlayer;
        bool m_IsPaused;
        MARATHON_INSERT_PADDING(3);
    };
}
