#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/weak_ptr.hpp>
#include <Sonicteam/Prop/IActorCreator.h>
#include <Sonicteam/Game.h>
#include <Sonicteam/NamedActor.h>
#include <stdx/compare/string_compare.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam::Prop
{
    class ActorCreators
    {
    public:
        xpointer<NamedActor> m_pActor;
        boost::weak_ptr<Game> m_wpGame;
        stdx::map<stdx::string, boost::shared_ptr<IActorCreator>, stdx::string_compare> m_ActorCreators;
    };

    MARATHON_ASSERT_OFFSETOF(ActorCreators, m_pActor, 0x00);
    MARATHON_ASSERT_OFFSETOF(ActorCreators, m_wpGame, 0x04);
    MARATHON_ASSERT_OFFSETOF(ActorCreators, m_ActorCreators, 0x0C);
    MARATHON_ASSERT_SIZEOF(ActorCreators, 0x18);
}
