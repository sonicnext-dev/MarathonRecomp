#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Task.h>

namespace Sonicteam
{
    class Actor : public SoX::Engine::Task
    {
    public:
        MARATHON_INSERT_PADDING(8); // boost::weak_ptr<Sonicteam::GameImp> GameImp;
        be<uint32_t> m_ActorID;
    };

    MARATHON_ASSERT_OFFSETOF(Actor, m_ActorID, 0x54);
    MARATHON_ASSERT_SIZEOF(Actor, 0x58);
}
