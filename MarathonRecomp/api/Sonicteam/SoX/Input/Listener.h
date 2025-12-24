#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Object.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX::Input
{
    class Listener : public SoX::Object
    {
    public:
        SoX::LinkNode<Listener> m_lnListener;
        bool m_Flag1;
        bool m_Flag2;
        be<uint32_t> m_Field14;
    };

    MARATHON_ASSERT_SIZEOF(Listener, 0x18);
    MARATHON_ASSERT_OFFSETOF(Listener, m_lnListener, 0x4);
    MARATHON_ASSERT_OFFSETOF(Listener, m_Flag1, 0x10);
    MARATHON_ASSERT_OFFSETOF(Listener, m_Flag2, 0x11);
    MARATHON_ASSERT_OFFSETOF(Listener, m_Field14, 0x14);
}

