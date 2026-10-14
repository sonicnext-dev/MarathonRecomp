#pragma once

#include <Marathon.inl>

namespace Chao::CSD
{
    template<typename TObserver, typename TObservee>
    class SubjectBase
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpGetObservee;
        };

        xpointer<Vftable> m_pVftable;
        MARATHON_INSERT_PADDING(0x0C);
    };
}
