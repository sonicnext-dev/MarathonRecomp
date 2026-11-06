#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHMDArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdHMDArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdHMDArray, 0x04);
