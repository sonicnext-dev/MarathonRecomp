#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/GE1PE/Plugin.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>
#include <stdx/map.h>
#include <stdx/string.h>
#include <stdx/vector.h>

namespace Sonicteam::GE1PE
{
    class EffectBank;
    class Manager;
    class Material;
    class TextureBank;

    class ParticleEngine : public System::Singleton<ParticleEngine, 0x82D3C4F4, System::CreateStatic<ParticleEngine, 0x82642090>>
    {
    public:
        xpointer<void> m_pVftable;
        boost::shared_ptr<Plugin> m_spPlugin;
        stdx::map<stdx::string, be<uint32_t>> m_mEffectIndices;
        stdx::map<stdx::string, be<uint32_t>> m_mTextureIndices;
        stdx::map<stdx::string, be<uint32_t>> m_mMaterialIndices;
        stdx::vector<xpointer<EffectBank>> m_vpEffectBank;
        stdx::vector<xpointer<TextureBank>> m_vpTextureBank;
        stdx::vector<xpointer<TextureBank>> m_vpMaterialBank;
        SoX::LinkNode<Manager> m_lnManager;
        stdx::vector<be<uint32_t>> m_vField6C;
    };

    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_spPlugin, 0x04);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_mEffectIndices, 0x0C);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_mTextureIndices, 0x18);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_mMaterialIndices, 0x24);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_vpEffectBank, 0x30);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_vpTextureBank, 0x40);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_vpMaterialBank, 0x50);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_lnManager, 0x60);
    MARATHON_ASSERT_OFFSETOF(ParticleEngine, m_vField6C, 0x6C);
}
