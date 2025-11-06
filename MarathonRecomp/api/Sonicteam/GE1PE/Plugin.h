#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>

namespace Sonicteam::GE1PE
{
    class Plugin
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            //....
        };

        xpointer<Vftable> m_pVftable;
        xpointer<SoX::Engine::Doc> m_pDoc;
    };

    // Size == 0x34 (but PE::MyPlugin no idea about this)
}
