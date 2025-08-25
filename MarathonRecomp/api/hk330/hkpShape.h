#pragma once

#include <Marathon.inl>
#include <hk330/hkReferencedObject.h>

namespace hk330
{
    class hkpShape : hkReferencedObject
    {
    public:
        be<uint32_t> m_userdata;
        be<uint32_t> m_type;
    };
}

