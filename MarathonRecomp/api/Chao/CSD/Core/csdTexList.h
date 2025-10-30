#pragma once

#include <Chao/CSD/Core/csdBase.h>
#include <Chao/CSD/Core/csdRCPtr.h>

namespace Chao::CSD
{
    class CTexList : public CBase
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
        };

        xpointer<Vftable> m_pVftable;
        RCPtr<uint8_t> m_pRCData;
    };
}
