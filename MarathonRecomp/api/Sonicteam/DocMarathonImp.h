#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Engine/Doc.h>

namespace Sonicteam
{
    class RenderTargetContainer;

    class DocMarathonImp : public SoX::Engine::Doc
    {
    public:
        MARATHON_INSERT_PADDING(0x74);
        bool m_VFrame;
        MARATHON_INSERT_PADDING(4);
        xpointer<RenderTargetContainer> m_pRenderTargetContainer;
    };
}
