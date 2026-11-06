#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdSimpleNotice.h>

struct _acdTrunkControl
{
    be<SpUint32> Version;
    be<SpUint32> Flags;
    be<SpSint32> Id;
    be<SpFloat32> ClipRange;
    be<SpFloat32> ClipZRange;
    be<SpFloat32> StartTime;
    be<SpFloat32> EndTime;
    be<SpUint32> SimpleNoticeNum;
    xpointer<xpointer<_acdSimpleNotice>> ppSimpleNotice;
};

MARATHON_ASSERT_OFFSETOF(_acdTrunkControl, Version, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdTrunkControl, Flags, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdTrunkControl, Id, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdTrunkControl, ClipRange, 0x0C);
MARATHON_ASSERT_OFFSETOF(_acdTrunkControl, ClipZRange, 0x10);
MARATHON_ASSERT_OFFSETOF(_acdTrunkControl, StartTime, 0x14);
MARATHON_ASSERT_OFFSETOF(_acdTrunkControl, EndTime, 0x18);
MARATHON_ASSERT_OFFSETOF(_acdTrunkControl, SimpleNoticeNum, 0x1C);
MARATHON_ASSERT_OFFSETOF(_acdTrunkControl, ppSimpleNotice, 0x20);
MARATHON_ASSERT_SIZEOF(_acdTrunkControl, 0x24);
