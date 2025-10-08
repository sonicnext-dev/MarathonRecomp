#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Shader.h>
#include <api/stdx/map.h>
#include <api/stdx/string.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/BasicIScriptParamsDesc.h>
#include <Sonicteam/SoX/Graphics/SyncronizeShaderPass.h>

namespace Sonicteam::SoX::Graphics
{
    class Device;
    class TechniqueFXL; // more about it

    // Custom Name, because original was lost(optimization)
    class ShaderScriptParamDesc : BasicIScriptParamsDesc
    {
        xpointer<void> m_pShaderParams;
        xpointer<Device> m_pDevice;
    };

    class ShaderPassFXL;



    class ShaderFXL : public Shader
    {
    public:
        xpointer<void> m_pShader; //D3D Type, unknown, creation 828B98D8 (sub_829BDBC8, not in video.cpp :\ weird)
        xpointer<BasicIScriptParamsDesc> m_pShaderParamsDesc;
        stdx::map<be<uint32_t>, SyncronizeShaderPass> m_mvPassesFXL;
        stdx::string m_FXLName; // -- ? not used ?
        MARATHON_INSERT_PADDING(4);
        SoX::LinkedList<TechniqueFXL> m_llTecniques;
    };

    MARATHON_ASSERT_OFFSETOF(ShaderFXL, m_pShader, 0x70);
    MARATHON_ASSERT_OFFSETOF(ShaderFXL, m_pShaderParamsDesc, 0x74);
    MARATHON_ASSERT_OFFSETOF(ShaderFXL, m_mvPassesFXL, 0x78);
    MARATHON_ASSERT_OFFSETOF(ShaderFXL, m_FXLName, 0x84);
    MARATHON_ASSERT_OFFSETOF(ShaderFXL, m_llTecniques, 0xA4);
    MARATHON_ASSERT_SIZEOF(ShaderFXL, 0xB0);
}
