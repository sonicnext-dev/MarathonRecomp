#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Technique.h>
#include <Sonicteam/MyShader.h>

namespace Sonicteam::SoX::Graphics
{
    class TechniqueFXL : public Technique
    {
    public:
        xpointer<MyShader> m_pShader;
        MARATHON_INSERT_PADDING(0x10);
        xpointer<TechniqueFXL> m_pParent;
        MARATHON_INSERT_PADDING(4);
        stdx::string m_TechniqueName;
    };

    MARATHON_ASSERT_OFFSETOF(TechniqueFXL, m_pShader, 0x0C);
    MARATHON_ASSERT_OFFSETOF(TechniqueFXL, m_pParent, 0x20);
    MARATHON_ASSERT_OFFSETOF(TechniqueFXL, m_TechniqueName, 0x28);
    MARATHON_ASSERT_SIZEOF(TechniqueFXL, 0x44);
}
