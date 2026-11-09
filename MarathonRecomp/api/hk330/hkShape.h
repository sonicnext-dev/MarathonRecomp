#pragma once

#include <Marathon.inl>
#include <hk330/hkReferencedObject.h>

namespace hk330
{
    class hkShape : public hkReferencedObject
    {
    public:
        be<uint32_t> m_userData;
    };

    MARATHON_ASSERT_OFFSETOF(hkShape, m_userData, 0x08);
    MARATHON_ASSERT_SIZEOF(hkShape, 0x0C);
}
