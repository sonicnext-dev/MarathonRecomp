#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/Game.h>

namespace Sonicteam
{
    class KynapseUtility
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<Game> m_pGame;
        xpointer<SoX::Physics::World> m_pWorld;
    };

    MARATHON_ASSERT_OFFSETOF(KynapseUtility, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(KynapseUtility, m_pGame, 0x04);
    MARATHON_ASSERT_OFFSETOF(KynapseUtility, m_pWorld, 0x08);
    MARATHON_ASSERT_SIZEOF(KynapseUtility, 0x0C);
}
