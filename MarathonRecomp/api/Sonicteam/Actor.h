#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/Game.h>

namespace Sonicteam
{
    class Actor : public SoX::Engine::Task
    {
    public:
        boost::weak_ptr<Game> m_wpGame;
        be<uint32_t> m_ActorID;
    };

    MARATHON_ASSERT_OFFSETOF(Actor, m_wpGame, 0x4C);
    MARATHON_ASSERT_OFFSETOF(Actor, m_ActorID, 0x54);
    MARATHON_ASSERT_SIZEOF(Actor, 0x58);
}
