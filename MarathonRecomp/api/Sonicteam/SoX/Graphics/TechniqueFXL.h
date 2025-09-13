#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Technique.h>

namespace Sonicteam::SoX::Graphics
{
    class TechniqueFXL : public Technique
    {
    public:
        xpointer<void> m_pShader;
        MARATHON_INSERT_PADDING(0x10);
        xpointer<TechniqueFXL> m_pThis;
        MARATHON_INSERT_PADDING(4);
        stdx::string m_TechniqueName;
    };
}
