#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkSpangleBase.h>
#include <Sonicteam/Spanverse/TrunkList.h>

namespace Sonicteam::Particles
{
    class ParticleManager;
}

namespace Sonicteam::Spanverse
{
    class AckAdminTrunkTag;

    class AckTrunkList
    {
    public:
        xpointer<void> m_pVftable;
        xpointer<Particles::ParticleManager> m_pParticleManager;
        TrunkList m_tlTrunkTags;
    };

    MARATHON_ASSERT_SIZEOF(AckTrunkList, 0x14);
}
