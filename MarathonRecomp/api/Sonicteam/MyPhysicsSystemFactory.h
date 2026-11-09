#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/Havok/PhysicsSystemFactoryHavok.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam
{
    class MyPhysicsSystemFactory : public SoX::Physics::Havok::PhysicsSystemFactoryHavok, public System::Singleton<MyPhysicsSystemFactory, 0x82D3C194, System::CreateStatic<MyPhysicsSystemFactory, 0x8262A4E0>> {};

    MARATHON_ASSERT_SIZEOF(MyPhysicsSystemFactory, 0x04);
}
