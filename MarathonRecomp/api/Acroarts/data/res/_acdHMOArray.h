#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHMOArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdHMOArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdHMOArray, 0x04);
