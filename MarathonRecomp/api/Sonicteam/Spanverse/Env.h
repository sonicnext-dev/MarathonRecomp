#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/SpkEnv.h>
#include <Sonicteam/SoX/Engine/Doc.h>
#include <Sonicteam/SoX/Graphics/Device.h>
#include <Sonicteam/SoX/Graphics/Shader.h>
#include <Sonicteam/SoX/Graphics/Technique.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Math/Matrix.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Spanverse
{
    class EnvInfo;

    class Env : SpkEnv
    {
    public:
        static constexpr uint32_t ms_Signature = 0x53505342; // SPSB
        xpointer<SoX::Engine::Doc> m_pDoc;
        xpointer<SoX::Graphics::Device> m_pDevice;
        MARATHON_INSERT_PADDING(0x4);
        SoX::RefSharedPointer<SoX::Graphics::Shader> m_pShader;
        SoX::RefSharedPointer<SoX::Graphics::Technique> m_pShaderTenchique1;
        SoX::RefSharedPointer<SoX::Graphics::Technique> m_pShaderTenchique2;
        SoX::RefSharedPointer<SoX::Graphics::Technique> m_pShaderTenchique3;
        SoX::Math::Matrix4x4 m_Transform; // usually player camera, not used i guess

        struct {
            char Flag1;
            char Flag2;
            SoX::Math::Vector Vector1;
            SoX::Math::Vector Vector2;
        } m_aField100[2];
        MARATHON_INSERT_PADDING(0x24);
        xpointer<EnvInfo> m_pEnvInfo; //0x184
    };

    MARATHON_ASSERT_OFFSETOF(Env,m_pDoc, 0xA0);
    MARATHON_ASSERT_OFFSETOF(Env,m_pDevice, 0xA4);
    MARATHON_ASSERT_OFFSETOF(Env,m_pShader, 0xAC);
    MARATHON_ASSERT_OFFSETOF(Env,m_pShaderTenchique1, 0xB0);
    MARATHON_ASSERT_OFFSETOF(Env,m_pShaderTenchique2, 0xB4);
    MARATHON_ASSERT_OFFSETOF(Env,m_pShaderTenchique3, 0xB8);
    MARATHON_ASSERT_OFFSETOF(Env,m_Transform, 0xC0);
    MARATHON_ASSERT_OFFSETOF(Env,m_aField100, 0x100);
    MARATHON_ASSERT_OFFSETOF(Env,m_pEnvInfo, 0x184);
    MARATHON_ASSERT_SIZEOF(Env, 0x190);
}
