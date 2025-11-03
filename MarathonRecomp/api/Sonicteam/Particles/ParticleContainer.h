#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <stdx/map.h>
#include <stdx/vector.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/Spanverse/AckResource.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/MyPE/EffectBankLoad.h>


namespace Sonicteam::Particles
{
    class ParticleContainerMgr;
    class ParticleManager;

    // Figure it same as in raw bytes?
    struct ParticleInfoCounted
    {
        be<uint32_t> m_Count;
    };

    struct ParticleInfoBank
    {
        xpointer<const char> Field0; // Example : particle/pe/player_sonic
        xpointer<const char> Field4; // Example : homing03_st
        be<uint32_t> Field8;
        MARATHON_INSERT_PADDING(4);
    };

    struct ParticleInfo : ParticleInfoCounted, ParticleInfoBank
    {
        auto* operator[](uint32_t index)
        {
            return (ParticleInfoBank*)((uint64_t)this + sizeof(ParticleInfoBank) * index);
        }
    };

    class ParticleContainer:SoX::IResource2<ParticleContainer, ParticleContainerMgr>
    {
    public:
        xpointer<ParticleManager> m_pParticleManager;
        be<uint16_t> m_Index; 
        stdx::string m_ContainerName; // player_sonic   
        stdx::map<stdx::string, be<uint16_t>> m_mParticleIndices; // attack_000, accumula, ...
        xpointer<ParticleInfoBank> m_pParticleInfoBank; // Hint: Destructor should recast back to 0 position from + 4, then destroy pointers
        be<uint16_t> m_Field98;

        stdx::vector<SoX::RefSharedPointer<Spanverse::AckResource>> m_vspAcxResource;
        stdx::vector<SoX::RefSharedPointer<MyPE::EffectBankLoad>> m_vspEffectBankLoad; //for each resource yep it can repeat same pointer
        stdx::vector<SoX::RefSharedPointer<MyPE::TextureBankLoad>> m_vspTextureBankLoad; //for each resource yep it can repeat same pointer
    };

    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_Index,0x68);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_ContainerName,0x6C);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_mParticleIndices,0x88);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_pParticleInfoBank,0x94);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_Field98,0x98);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_vspAcxResource,0x9C);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_vspEffectBankLoad,0xAC);
    MARATHON_ASSERT_OFFSETOF(ParticleContainer, m_vspTextureBankLoad,0xBC);
}
