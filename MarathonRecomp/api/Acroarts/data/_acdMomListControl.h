#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdMomListControl
{
    be<SpUint32> Flags;
    be<SpSint32> Id;
    be<SpUint32> CoordTarget;
    be<SpUint32> CoordType;
    be<SpUint32> CoordNode;
};

MARATHON_ASSERT_OFFSETOF(_acdMomListControl, Flags, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdMomListControl, Id, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdMomListControl, CoordTarget, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdMomListControl, CoordType, 0x0C);
MARATHON_ASSERT_OFFSETOF(_acdMomListControl, CoordNode, 0x10);
MARATHON_ASSERT_SIZEOF(_acdMomListControl, 0x14);
