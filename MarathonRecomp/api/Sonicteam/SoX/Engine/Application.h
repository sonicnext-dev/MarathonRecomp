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
        xpointer<SoX::Engine::Doc> m_pDoc;
    };
}
