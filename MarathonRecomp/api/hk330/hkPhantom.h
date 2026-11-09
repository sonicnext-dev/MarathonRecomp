#pragma once

#include <hk330/hkArray.h>
#include <hk330/hkWorldObject.h>

namespace hk330
{
    class hkPhantom : public hkWorldObject
    {
    public:
        hkArray<xpointer<void>> m_overlapListeners;
        hkArray<xpointer<void>> m_phantomListeners;
    };

    MARATHON_ASSERT_OFFSETOF(hkPhantom, m_overlapListeners, 0x8C);
    MARATHON_ASSERT_OFFSETOF(hkPhantom, m_phantomListeners, 0x98);
    MARATHON_ASSERT_SIZEOF(hkPhantom, 0xA4);
}
