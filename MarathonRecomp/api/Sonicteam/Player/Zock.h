#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IZock.h>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/MyPhantom.h>

namespace Sonicteam::Player
{
    class Zock : public IZock, public IFlagCommunicator, public IStepable, public IDynamicLink, public IVariable
    {
    public:
        MARATHON_INSERT_PADDING(0x8);
        xpointer<Sonicteam::MyPhantom> m_pPhantom;
        MARATHON_INSERT_PADDING(0x20);
        xpointer<Sonicteam::MyPhantom> m_pPhantomB;
        MARATHON_INSERT_PADDING(0x84);

    };
}
