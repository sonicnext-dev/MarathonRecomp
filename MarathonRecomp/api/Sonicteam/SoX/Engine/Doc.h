#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/DocMode.h>

namespace Sonicteam::SoX::Engine
{
    class Doc
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(4);
        xpointer<SoX::Engine::DocMode> m_pDocMode;
        MARATHON_INSERT_PADDING(0x50);

        template <typename T>
        inline T* GetDocMode();
    };
}

#include <Sonicteam/SoX/Engine/Doc.inl>
