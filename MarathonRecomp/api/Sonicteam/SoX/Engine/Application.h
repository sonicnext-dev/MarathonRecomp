#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Object.h>
#include <Sonicteam/SoX/Engine/Doc.h>

namespace Sonicteam::SoX::Engine
{
    class Application: SoX::Object
    {
    public:
        bool IsField4;
        xpointer<SoX::Engine::Doc> m_pParent;
    };

    MARATHON_ASSERT_OFFSETOF(Application, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(Application, 0x0C);
}
