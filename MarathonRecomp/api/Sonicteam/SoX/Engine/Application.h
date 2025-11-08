#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Object.h>
#include <Sonicteam/SoX/Engine/Doc.h>

namespace Sonicteam::SoX::Engine
{
    class Application : Object
    {
    public:
        bool IsField04;
        xpointer<SoX::Engine::Doc> m_pParent;
    };

    MARATHON_ASSERT_SIZEOF(Application, 0x0C);
}
