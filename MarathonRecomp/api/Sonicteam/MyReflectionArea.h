#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/Game.h>
#include <Sonicteam/ReflectionArea.h>
#include <stdx/list.h>

namespace Sonicteam
{
    class MyReflectionArea : public ReflectionArea
    {
    public:
        boost::shared_ptr<Game> m_spGame;
        stdx::list<be<uint32_t>> m_PlayerActorIDs;
        MARATHON_INSERT_PADDING(0x0C);
    };

    MARATHON_ASSERT_OFFSETOF(MyReflectionArea, m_spGame, 0x70);
    MARATHON_ASSERT_OFFSETOF(MyReflectionArea, m_PlayerActorIDs, 0x78);
    MARATHON_ASSERT_SIZEOF(MyReflectionArea, 0x90);
}
