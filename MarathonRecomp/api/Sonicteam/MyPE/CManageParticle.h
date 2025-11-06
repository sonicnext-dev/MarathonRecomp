#pragma once

#include <Marathon.inl>
#include <Sonicteam/Particles/ParticleManagerUnit.h>
#include <Sonicteam/Particles/Particle.h>
#include <Sonicteam/MyPE/MyEmitterListener.h>
#include <Sonicteam/GE1PE/Manager.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <stdx/list.h>

namespace Sonicteam::MyPE
{
    class ManageParticleTask;
    class Manager;
    class MyPlugin;

    struct CManageParticleContainer
    {
        xpointer<void> _1;
        xpointer<void> _2;
        xpointer<void> _3;
    };

    class CManageParticle : public Particles::ParticleManagerUnit
    {
    public:
        xpointer<SoX::Engine::Doc> m_pDoc;
        XRTL_CRITICAL_SECTION m_CriticalSection;
        stdx::list<CManageParticleContainer> m_lContainer;
        SoX::LinkNode<Particles::Particle> m_lnParticle;
        xpointer<ManageParticleTask> m_pManagerParticleTask;
        MARATHON_INSERT_PADDING(4);
        boost::shared_ptr<GE1PE::Manager> m_spManager;
        boost::shared_ptr<MyPlugin> m_spMyPlugin;
        MyEmitterListener m_MyEmitterListener;
        MARATHON_INSERT_PADDING(0x2C);

        static CManageParticle* GetInstance()
        {
            return  *(xpointer<CManageParticle>*)MmGetHostAddress(0x82D3C54C);
        }
    };

    MARATHON_ASSERT_OFFSETOF(CManageParticle, m_pDoc, 4);
    MARATHON_ASSERT_OFFSETOF(CManageParticle, m_CriticalSection, 8);
    MARATHON_ASSERT_OFFSETOF(CManageParticle, m_lContainer, 0x24);
    MARATHON_ASSERT_OFFSETOF(CManageParticle, m_lnParticle, 0x30);
    MARATHON_ASSERT_OFFSETOF(CManageParticle, m_pManagerParticleTask, 0x3C);
}
