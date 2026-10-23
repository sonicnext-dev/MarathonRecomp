#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Manager/csdmResourceBase.h>

namespace Chao::CSD
{
    struct MotionPalette;

    class CMotionPalette : public CResourceBase<MotionPalette>, CBase
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_SIZEOF(CMotionPalette, 0x10);
}
