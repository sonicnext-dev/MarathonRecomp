#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdSimpleNotice
{
    be<SpFloat32> Time;
    be<SpSint32> Param0;
    be<SpSint32> Param1;
};

MARATHON_ASSERT_OFFSETOF(_acdSimpleNotice, Time, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdSimpleNotice, Param0, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdSimpleNotice, Param1, 0x08);
MARATHON_ASSERT_SIZEOF(_acdSimpleNotice, 0x0C);
