#pragma once

#include <Marathon.inl>
#include <Sonicteam/Particles/ParticleManagerUnit.h>
#include <Sonicteam/Spanverse/AckTrunkList.h>

namespace Sonicteam::Particles
{
    class ParticleManager;
}

namespace Sonicteam::Spanverse
{
    class AckMomServer;
    class SpkManageTask;
    class SpkRenderManageTask;


    class AckManager : public Particles::ParticleManagerUnit
    {
    public:
        xpointer<Particles::ParticleManager> m_pParticleManager;
        AckTrunkList m_tlTrunks;
        MARATHON_INSERT_PADDING(0x4);
        xpointer<AckMomServer> m_pStorage;
        xpointer<SpkManageTask> m_pSpkManageTask;
        xpointer<SpkRenderManageTask> m_pSpkRenderManageTask;
    };

    MARATHON_ASSERT_OFFSETOF(AckManager, m_pParticleManager, 0x4);
    MARATHON_ASSERT_OFFSETOF(AckManager, m_tlTrunks, 0x8);
    MARATHON_ASSERT_OFFSETOF(AckManager, m_pStorage, 0x20);
    MARATHON_ASSERT_OFFSETOF(AckManager, m_pSpkManageTask, 0x24);
    MARATHON_ASSERT_OFFSETOF(AckManager, m_pSpkRenderManageTask, 0x28);
    MARATHON_ASSERT_SIZEOF(AckManager, 0x2C);
}
