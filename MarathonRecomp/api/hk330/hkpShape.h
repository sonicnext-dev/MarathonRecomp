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
}
