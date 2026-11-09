#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Object.h>

namespace Sonicteam::SoX::Input
{
    class Listener : public Object
    {
    public:
        LinkNode<Listener> m_Siblings;
        uint8_t m_Field10;
        uint8_t m_Field11;
        MARATHON_INSERT_PADDING(0x06);
    };

    MARATHON_ASSERT_OFFSETOF(Listener, m_Siblings, 0x04);
    MARATHON_ASSERT_OFFSETOF(Listener, m_Field10, 0x10);
    MARATHON_ASSERT_OFFSETOF(Listener, m_Field11, 0x11);
    MARATHON_ASSERT_SIZEOF(Listener, 0x18);
}
