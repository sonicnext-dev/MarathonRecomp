#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdHSMArray
{
    be<SpUint32> Index;
    be<SpUint32> MotionElement;
};

MARATHON_ASSERT_OFFSETOF(_acdHSMArray, Index, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdHSMArray, MotionElement, 0x04);
MARATHON_ASSERT_SIZEOF(_acdHSMArray, 0x08);
