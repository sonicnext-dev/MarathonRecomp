#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdResource.h>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/kine/SpkSpangleBase.h>

struct _ackLeafStatus
{
    xpointer<SpkSpangleBase> pSpangle;
    be<SpSint32> ResType;
    xpointer<_acdResource> pResource;
    be<SpFloat32> LocalTimeRate;
    be<SpUint16> NumBirth;
    be<SpUint16> BirthId;
    be<SpUint16> Flags;
};

MARATHON_ASSERT_OFFSETOF(_ackLeafStatus, pSpangle, 0x00);
MARATHON_ASSERT_OFFSETOF(_ackLeafStatus, ResType, 0x04);
MARATHON_ASSERT_OFFSETOF(_ackLeafStatus, pResource, 0x08);
MARATHON_ASSERT_OFFSETOF(_ackLeafStatus, LocalTimeRate, 0x0C);
MARATHON_ASSERT_OFFSETOF(_ackLeafStatus, NumBirth, 0x10);
MARATHON_ASSERT_OFFSETOF(_ackLeafStatus, BirthId, 0x12);
MARATHON_ASSERT_OFFSETOF(_ackLeafStatus, Flags, 0x14);
MARATHON_ASSERT_SIZEOF(_ackLeafStatus, 0x18);
