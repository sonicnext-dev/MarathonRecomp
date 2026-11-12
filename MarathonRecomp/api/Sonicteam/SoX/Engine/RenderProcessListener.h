#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX::Engine
{
    class RenderProcessListener
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpFunc04;
            be<uint32_t> fpFunc08;
        };

        xpointer<Vftable> m_pVftable;
        LinkNode<RenderProcessListener> m_Node; // The end is never the end...
    };

    MARATHON_ASSERT_OFFSETOF(RenderProcessListener, m_Node, 0x04);
    MARATHON_ASSERT_SIZEOF(RenderProcessListener, 0x10);
}
