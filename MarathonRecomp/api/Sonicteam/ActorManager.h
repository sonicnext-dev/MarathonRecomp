#pragma once

#include <Marathon.inl>
#include <Sonicteam/Actor.h>

namespace Sonicteam
{
    class ActorManager
    {
    public:
        be<uint32_t> m_ActorID[0xFFFF];
        xpointer<Actor> m_Actor[0xFFFF];
        be<uint32_t> m_LastActorID;
        be<uint32_t> m_LastActorIndex;

        Sonicteam::Actor* FindActor(uint32_t ActorID);
    };
}

#include "ActorManager.inl"
