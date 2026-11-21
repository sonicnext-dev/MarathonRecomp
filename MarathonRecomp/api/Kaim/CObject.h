#pragma once

#include <Marathon.inl>

namespace Kaim
{
    class CObject
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDtor;
        };

        xpointer<Vftable> m_pVftable;
    };
}
