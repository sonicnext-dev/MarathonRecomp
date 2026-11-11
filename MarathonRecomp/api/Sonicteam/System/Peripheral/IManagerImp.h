#pragma once

#include <Marathon.inl>

namespace Sonicteam::System::Peripheral
{
    class IManagerImp
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDtor;
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> fpGetPeripheralCount;
            be<uint32_t> fpUpdate;
            MARATHON_INSERT_PADDING(0x04);
        };

        xpointer<Vftable> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IManagerImp, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IManagerImp, 0x04);
}
