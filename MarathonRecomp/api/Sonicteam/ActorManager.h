#pragma once

#include <Marathon.inl>
#include <Sonicteam/Actor.h>

namespace Sonicteam
{
    class ActorManager
    {
    public:
		be<uint32_t> m_ActorIDs[0xFFFF];
		xpointer<Actor> m_Actors[0xFFFF];
		be<uint32_t> m_LastActorID;
		be<uint32_t> m_LastActorIndex;
        MARATHON_INSERT_PADDING(0x04);

        template <typename T = Actor>
        T* Find(uint32_t in_actorId) const
        {
            if ((std::countl_zero(0xFFFFFFFF - in_actorId) & 0x20) != 0)
                return nullptr;

            int low{};
            int mid{};
            int high = m_LastActorIndex - 1;

            if (high > 0)
            {
                do
                {
                    mid = (high + low) >> 1;

                    if (m_ActorIDs[mid] < in_actorId)
                    {
                        low = mid + 1;
                    }
                    else
                    {
                        high = mid;
                    }
                }
                while (low < high);
            }

            if (in_actorId != m_ActorIDs[low])
                return nullptr;

            return static_cast<T*>(m_Actors[low].get());
        }
    };

    MARATHON_ASSERT_OFFSETOF(ActorManager, m_ActorIDs, 0x00);
    MARATHON_ASSERT_OFFSETOF(ActorManager, m_Actors, 0x3FFFC);
    MARATHON_ASSERT_OFFSETOF(ActorManager, m_LastActorID, 0x7FFF8);
    MARATHON_ASSERT_OFFSETOF(ActorManager, m_LastActorIndex, 0x7FFFC);
    MARATHON_ASSERT_SIZEOF(ActorManager, 0x80004);
}
