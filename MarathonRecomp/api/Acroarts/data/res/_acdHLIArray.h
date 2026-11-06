#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHLIArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdHLIArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdHLIArray, 0x04);
