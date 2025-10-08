#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Technique.h>
#include <Sonicteam/SoX/Graphics/SyncronizeShaderPass.h>

namespace Sonicteam::SoX::Graphics
{
    class ShaderFXL;

    class TechniqueFXL : public Technique
    {
    public:
        xpointer<ShaderFXL> m_pShader;
        be<uint32_t> m_PassKeyFXL; //0x0007FFFC (ShaderFXL::m_mPassFXL, key is match this parameter)
        xpointer<SyncronizeShaderPass> m_pvPassesFXL; // ShaderFXL::m_mPassFXL<key,SyncronizeShaderPass> (for reference)
        SoX::LinkNode<TechniqueFXL> m_lnTecnhiqueFXL;
        stdx::string m_TechniqueName;
        be<uint32_t> m_FXLFlag;
    };

    MARATHON_ASSERT_OFFSETOF(TechniqueFXL, m_pShader, 0x0C);
    MARATHON_ASSERT_OFFSETOF(TechniqueFXL, m_PassKeyFXL, 0x10);
    MARATHON_ASSERT_OFFSETOF(TechniqueFXL, m_pvPassesFXL, 0x14);
    MARATHON_ASSERT_OFFSETOF(TechniqueFXL, m_lnTecnhiqueFXL, 0x18);
    MARATHON_ASSERT_OFFSETOF(TechniqueFXL, m_TechniqueName, 0x24);
    MARATHON_ASSERT_OFFSETOF(TechniqueFXL, m_FXLFlag, 0x40);
    MARATHON_ASSERT_SIZEOF(TechniqueFXL, 0x44);
}
