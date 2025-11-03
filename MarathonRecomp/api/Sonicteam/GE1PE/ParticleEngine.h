#pragma once

#include <Marathon.inl>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/GE1PE/Plugin.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <stdx/map.h>
#include <stdx/string.h>
#include <stdx/vector.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::GE1PE
{
    class EffectBank;
    class TextureBank;
    class Material;
    class Manager;

    class ParticleEngine : public System::Singleton<ParticleEngine,0x82D3C4F4, System::CreateStatic<ParticleEngine,0x82642090>>
    {
    public:
        xpointer<void> m_pVftable;
        boost::shared_ptr<Plugin> m_spPlugin;
        stdx::map<stdx::string, be<uint32_t>> m_mEffectIndicies; // ev001 = index
        stdx::map<stdx::string, be<uint32_t>> m_mTextureIndicies; //vehicle_jet_grid = 0x9F;
        stdx::map<stdx::string, be<uint32_t>> m_MaterialIndicies; //addBN = 3; MeshTestShader ...
        stdx::vector<xpointer<EffectBank>> m_vpEffectBank;
        stdx::vector<xpointer<TextureBank>> m_vpTextureBank;
        stdx::vector<xpointer<TextureBank>> m_vpMaterialBank;
        SoX::LinkNode<Manager> m_lnManager;
        stdx::vector<void> m_vField6C;
    };
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_vpEffectBank, 0x30);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_vpTextureBank, 0x40);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_vpMaterialBank, 0x50);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_lnManager, 0x60);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_vField6C, 0x6C);
}
