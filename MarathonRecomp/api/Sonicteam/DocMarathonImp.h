#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>

namespace Sonicteam
{
    class DocMarathonImp : public Sonicteam::SoX::Engine::Doc
    {
    public:
        MARATHON_INSERT_PADDING(0x74);
        bool m_VFrame;
    };
}
