#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdBranch.h>
#include <Acroarts/data/_acdTrunkControl.h>

struct _acdTrunk
{
    _acdTrunkControl TrunkCtrl;
    be<SpUint32> BranchNum;
    xpointer<xpointer<_acdBranch>> ppBranch;
};

MARATHON_ASSERT_OFFSETOF(_acdTrunk, TrunkCtrl, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdTrunk, BranchNum, 0x24);
MARATHON_ASSERT_OFFSETOF(_acdTrunk, ppBranch, 0x28);
MARATHON_ASSERT_SIZEOF(_acdTrunk, 0x2C);
