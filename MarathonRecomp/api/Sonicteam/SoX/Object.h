#pragma once

#include <Marathon.inl>

namespace Sonicteam::SoX
{
    class Object
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpGetName;
        };

        xpointer<Vftable> m_pVftable;

        const char* GetName() const
        {
            return GuestToHostFunction<const char*>(m_pVftable->fpGetName.get());
        }
    };
}
