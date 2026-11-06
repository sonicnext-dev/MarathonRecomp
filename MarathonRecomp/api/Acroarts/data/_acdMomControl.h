#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdMomControl
{
    be<SpUint32> Flags;
    be<SpSint32> MomId;
    be<SpSint32> SwitchId;
    be<SpFloat32> StartTime;
    be<SpFloat32> EndTime;
};

MARATHON_ASSERT_OFFSETOF(_acdMomControl, Flags, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdMomControl, MomId, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdMomControl, SwitchId, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdMomControl, StartTime, 0x0C);
MARATHON_ASSERT_OFFSETOF(_acdMomControl, EndTime, 0x10);
MARATHON_ASSERT_SIZEOF(_acdMomControl, 0x14);
