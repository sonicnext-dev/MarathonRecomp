#pragma once

#include <Marathon.inl>
#include <Sonicteam/Particles/ParticleRef.h>
#include <Sonicteam/Particles/ParticleManagerUnit.h>
#include <Sonicteam/Particles/ParticleContainer.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <stdx/map.h>

namespace Sonicteam::Particles
{
    class ParticleManager
    {
    public:
        xpointer<SoX::Engine::Doc> m_Doc;
        stdx::vector<boost::shared_ptr<ParticleManagerUnit>> m_vspManagerUnit; //check iterrate to be sure about size
        be<uint32_t> m_PlayCount1; // almost for any particle 
        be<uint32_t> m_PlayCount2; // always 1
        be<uint32_t> m_PlayCount3; // always 1?
        be<uint16_t> m_Field20;
        stdx::map<stdx::string, be<uint16_t>> m_mParticleContainerIndices; //make sure it really uint16_t
        stdx::map<be<uint16_t>,xpointer<ParticleContainer>> m_mParticleContainer; //here too :)
        be<float> m_ParticleSpeed;
    };
    MARATHON_ASSERT_SIZEOF(ParticleManager, 0x40);
    MARATHON_ASSERT_OFFSETOF(ParticleManager, m_Doc,0);
    MARATHON_ASSERT_OFFSETOF(ParticleManager, m_vspManagerUnit,4);
    MARATHON_ASSERT_OFFSETOF(ParticleManager, m_PlayCount1,0x14);
    MARATHON_ASSERT_OFFSETOF(ParticleManager, m_PlayCount2,0x18);
    MARATHON_ASSERT_OFFSETOF(ParticleManager, m_PlayCount3,0x1C);
    MARATHON_ASSERT_OFFSETOF(ParticleManager, m_Field20,0x20);
    MARATHON_ASSERT_OFFSETOF(ParticleManager, m_mParticleContainerIndices,0x24);
    MARATHON_ASSERT_OFFSETOF(ParticleManager, m_mParticleContainer,0x30);
}
