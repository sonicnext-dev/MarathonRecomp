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

    MARATHON_ASSERT_OFFSETOF(Zock, m_spWorld, 0x30);
    MARATHON_ASSERT_OFFSETOF(Zock, m_spRootFrame, 0x34);
    MARATHON_ASSERT_OFFSETOF(Zock, m_spPhantomA, 0x38);
    MARATHON_ASSERT_OFFSETOF(Zock, m_spPhantomB, 0x5C);
    MARATHON_ASSERT_OFFSETOF(Zock, m_spPhantomListener, 0xA0);
    MARATHON_ASSERT_SIZEOF(Zock, 0xE0);
}
