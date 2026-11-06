#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHMTArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdHMTArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdHMTArray, 0x04);
