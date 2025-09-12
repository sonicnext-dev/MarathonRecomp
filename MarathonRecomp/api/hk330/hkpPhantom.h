#pragma once

#include <hk330/hkpWorldObject.h>
#include <hk330/hkArray.h>

namespace hk330
{
    class hkpPhantom : public hkpWorldObject
    {
    public:
        hkArray<void*> m_overlapListeners;
        hkArray<void*> m_phantomListeners;
    };
}
