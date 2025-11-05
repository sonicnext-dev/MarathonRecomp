#pragma once

#include <Marathon.inl>
#include <Sonicteam/Actor.h>

namespace Sonicteam
{
    class ActorManager
    {
    public:
		be<uint32_t> m_aActorIDs[0xFFFF];
		xpointer<Actor> m_aActors[0xFFFF];
		be<uint32_t> m_LastActorID;
		be<uint32_t> m_LastActorIndex;
        MARATHON_INSERT_PADDING(4);
    };

    MARATHON_ASSERT_OFFSETOF(ActorManager, m_aActorIDs, 0x00);
    MARATHON_ASSERT_OFFSETOF(ActorManager, m_aActors, 0x3FFFC);
    MARATHON_ASSERT_OFFSETOF(ActorManager, m_LastActorID, 0x7FFF8);
    MARATHON_ASSERT_OFFSETOF(ActorManager, m_LastActorIndex, 0x7FFFC);
    MARATHON_ASSERT_SIZEOF(ActorManager, 0x80004);
}
