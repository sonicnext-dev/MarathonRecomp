#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdBranchChain.h>

struct _acdBranchControl
{
    be<SpUint32> Flags;
    be<SpSint32> Id;
    be<SpFloat32> ClipRange;
    be<SpFloat32> ClipZRange;
    be<SpFloat32> StartTime;
    be<SpFloat32> EndTime;
    be<SpUint32> CoordTarget;
    be<SpUint32> CoordBranchNo;
    be<SpSint32> CoordNode;
    xpointer<char> CoordNodeName;
    be<SpUint32> CoordType;
    be<SpSint32> MessageParam0;
    be<SpSint32> MessageParam1;
    be<SpUint32> ChainNum;
    xpointer<xpointer<_acdBranchChain>> ppChain;
    be<SpSint32> SortGroup;
    be<SpSint32> LoopNum;
};

MARATHON_ASSERT_OFFSETOF(_acdBranchControl, Flags, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, Id, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, ClipRange, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, ClipZRange, 0x0C);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, StartTime, 0x10);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, EndTime, 0x14);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, CoordTarget, 0x18);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, CoordBranchNo, 0x1C);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, CoordNode, 0x20);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, CoordNodeName, 0x24);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, CoordType, 0x28);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, MessageParam0, 0x2C);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, MessageParam1, 0x30);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, ChainNum, 0x34);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, ppChain, 0x38);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, SortGroup, 0x3C);
MARATHON_ASSERT_OFFSETOF(_acdBranchControl, LoopNum, 0x40);
MARATHON_ASSERT_SIZEOF(_acdBranchControl, 0x44);
