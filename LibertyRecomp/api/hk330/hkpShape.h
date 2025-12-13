#pragma once

#include <Marathon.inl>
#include <hk330/hkReferencedObject.h>

namespace hk330
{
    class hkpShape : public hkReferencedObject
    {
    public:
        be<uint32_t> m_userData;
        be<uint32_t> m_type;
    };

    MARATHON_ASSERT_OFFSETOF(hkpShape, m_userData, 0x08);
    MARATHON_ASSERT_OFFSETOF(hkpShape, m_type, 0x0C);
}
