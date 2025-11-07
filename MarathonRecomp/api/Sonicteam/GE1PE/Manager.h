#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::GE1PE
{
    class ParticleEngine;
    class Particle;
    class Emitter;
    class Effect;

    class Manager
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        xpointer<ParticleEngine> m_pParticleEngine;
        SoX::LinkNode<Manager> m_lnManager;
        SoX::LinkNode<void> m_lnField14;
        SoX::LinkNode<Particle> m_lnParticle;
        SoX::LinkNode<void> m_lnField2C;
        SoX::LinkNode<void> m_lnField38;
        SoX::LinkNode<Emitter> m_lnEmitter;
        SoX::LinkNode<Effect> m_lnEffect;
        SoX::LinkNode<Effect> m_lnField5C;
        MARATHON_INSERT_PADDING(0x24);
    };
    MARATHON_ASSERT_OFFSETOF(Manager, m_pParticleEngine, 4);
    MARATHON_ASSERT_OFFSETOF(Manager, m_lnManager, 8);
    MARATHON_ASSERT_OFFSETOF(Manager, m_lnField14, 0x14);
    MARATHON_ASSERT_OFFSETOF(Manager, m_lnParticle, 0x20);
    MARATHON_ASSERT_OFFSETOF(Manager, m_lnField2C, 0x2C);
    MARATHON_ASSERT_OFFSETOF(Manager, m_lnField38, 0x38);
    MARATHON_ASSERT_OFFSETOF(Manager, m_lnEmitter, 0x44);
    MARATHON_ASSERT_OFFSETOF(Manager, m_lnEffect, 0x50);
    MARATHON_ASSERT_OFFSETOF(Manager, m_lnField5C, 0x5C);

}
