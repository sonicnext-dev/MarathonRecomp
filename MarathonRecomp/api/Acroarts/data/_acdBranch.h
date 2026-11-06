#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdBranchControl.h>
#include <Acroarts/data/_acdLeaf.h>

struct _acdBranch
{
    _acdBranchControl BranchCtrl;
    be<SpUint32> LeafNum;
    xpointer<xpointer<_acdLeaf>> ppLeaf;
};

MARATHON_ASSERT_OFFSETOF(_acdBranch, BranchCtrl, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdBranch, LeafNum, 0x44);
MARATHON_ASSERT_OFFSETOF(_acdBranch, ppLeaf, 0x48);
MARATHON_ASSERT_SIZEOF(_acdBranch, 0x4C);
