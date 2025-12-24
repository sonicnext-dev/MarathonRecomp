#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    class IImportAnimation
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpImportAnimation;
        };

        xpointer<Vftable> m_pVftable;
    };

    MARATHON_ASSERT_OFFSETOF(IImportAnimation, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IImportAnimation, 4);
}
