#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHSCArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdHSCArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdHSCArray, 0x04);
