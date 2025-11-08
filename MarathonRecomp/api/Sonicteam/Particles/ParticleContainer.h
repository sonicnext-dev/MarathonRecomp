#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/MyPE/EffectBankLoad.h>
#include <Sonicteam/Spanverse/AckResource.h>
#include <stdx/map.h>
#include <stdx/vector.h>

namespace Sonicteam::Particles
{
    class ParticleContainerMgr;
    class ParticleManager;

    struct ParticleBank
    {
        xpointer<const char> pEffectBankFile;
        xpointer<const char> pEffectName;
        be<uint32_t> Field08;
        MARATHON_INSERT_PADDING(4);
    };

    struct ParticleBankArray
    {
        be<uint32_t> m_Length;
        ParticleBank m_First;

        ParticleBank* operator[](uint32_t index)
        {
            return (ParticleBank*)((uint8_t*)this + sizeof(ParticleBank) * index);
        }
    };

    class ParticleContainer : public SoX::IResource2<ParticleContainer, ParticleContainerMgr>
    {
    public:
        xpointer<ParticleManager> m_pParticleManager;
        be<uint16_t> m_Index; 
        stdx::string m_ContainerName;
        stdx::map<stdx::string, be<uint16_t>> m_mParticleIndices;
        xpointer<ParticleBank> m_pParticleBank; // Hint: Destructor should recast back to 0 position from + 4, then destroy pointers.
        be<uint16_t> m_Field98;
        stdx::vector<SoX::RefSharedPointer<Spanverse::AckResource>> m_vspAckResource;
        stdx::vector<SoX::RefSharedPointer<MyPE::EffectBankLoad>> m_vspEffectBankLoad;
        stdx::vector<SoX::RefSharedPointer<MyPE::TextureBankLoad>> m_vspTextureBankLoad;
    };

    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_Index, 0x68);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_ContainerName, 0x6C);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_mParticleIndices, 0x88);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_pParticleBank, 0x94);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_Field98, 0x98);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_vspAckResource, 0x9C);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_vspEffectBankLoad, 0xAC);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_vspTextureBankLoad, 0xBC);
}
