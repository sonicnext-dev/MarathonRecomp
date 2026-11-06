#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHCMArray
{
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdHCMArray, Index, 0x00);
MARATHON_ASSERT_SIZEOF(_acdHCMArray, 0x04);
