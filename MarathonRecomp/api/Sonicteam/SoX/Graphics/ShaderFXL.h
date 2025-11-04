#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Shader.h>

namespace Sonicteam::SoX::Graphics
{
    class ShaderFXL : public Shader
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0xC);
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0x3C);
    };

    MARATHON_ASSERT_OFFSETOF(ShaderFXL, m_pVftable, 0x70);
    MARATHON_ASSERT_SIZEOF(ShaderFXL, 0xB0);
}

