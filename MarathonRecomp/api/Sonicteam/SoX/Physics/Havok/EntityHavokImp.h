#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/Havok/WorldHavok.h>
#include <Sonicteam/SoX/Physics/Havok/EntityHavok.h>

namespace Sonicteam::SoX::Physics::Havok
{
    template<typename T>
    class EntityHavokImp :
        public T,
        public Sonicteam::SoX::Physics::Havok::EntityHavok
    {
        //padding 0x8?????
    };
}
