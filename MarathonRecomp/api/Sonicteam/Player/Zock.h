#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IZock.h>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/MyPhantom.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/Player/RootFrame.h>
#include <Sonicteam/SoX/Physics/PhantomListener.h>

namespace Sonicteam::Player
{
    class Zock : public IZock, public IFlagCommunicator, public IStepable, public IDynamicLink, public IVariable
    {
    public:
        SoX::RefSharedPointer<SoX::Physics::World> m_spWorld;
        SoX::RefSharedPointer<SoX::Physics::World> m_spRootFrame;
        SoX::RefSharedPointer<MyPhantom> m_spPhantomA;
        MARATHON_INSERT_PADDING(0x20);
        SoX::RefSharedPointer<MyPhantom> m_spPhantomB;
        MARATHON_INSERT_PADDING(0x40);
        SoX::RefSharedPointer<SoX::Physics::PhantomListener> m_spPhantomListener;
        MARATHON_INSERT_PADDING(0x3C);
    };
}
