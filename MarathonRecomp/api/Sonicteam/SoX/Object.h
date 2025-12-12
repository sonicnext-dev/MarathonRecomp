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
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;

        const char* GetName() const
        {
            return GuestToHostFunction<const char*>(m_pVftable->fpGetName.get());
        }
    };

    MARATHON_ASSERT_OFFSETOF(Object, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(Object, 4);
}
