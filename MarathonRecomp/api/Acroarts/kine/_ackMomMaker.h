#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _ackMomMaker
{
    xpointer<void> fMomSetup;
    be<SpUint32> MomId;
    xpointer<_ackMomMaker> pNext;
};

MARATHON_ASSERT_OFFSETOF(_ackMomMaker, fMomSetup, 0x00);
MARATHON_ASSERT_OFFSETOF(_ackMomMaker, MomId, 0x04);
MARATHON_ASSERT_OFFSETOF(_ackMomMaker, pNext, 0x08);
MARATHON_ASSERT_SIZEOF(_ackMomMaker, 0x0C);
