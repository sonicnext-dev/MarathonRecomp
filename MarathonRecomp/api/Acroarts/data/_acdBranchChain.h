#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _acdBranchChain
{
    be<SpUint32> Flags;
    be<SpUint32> CoordType;
    be<SpUint32> Index;
};

MARATHON_ASSERT_OFFSETOF(_acdBranchChain, Flags, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdBranchChain, CoordType, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdBranchChain, Index, 0x08);
MARATHON_ASSERT_SIZEOF(_acdBranchChain, 0x0C);
