#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/kine/SpkSpangleBase.h>
#include <Acroarts/kine/SpkTypes.h>

struct _ackPrimitiveControl;

struct _ackBranchStatus
{
    SpMatrix BranchMatrix;
    be<SpFloat32> MyCount;
    be<SpSint32> LoopNum;
    be<SpBool> DefaultFog;
    be<SpBool> Fog;
    be<SpUint32> SortGroup;
    xpointer<SpkSpangleBase> pCoordSpangle;
    be<SpUint32> NumPrimitive;
    xpointer<_ackPrimitiveControl> pPrimitive;
};

MARATHON_ASSERT_OFFSETOF(_ackBranchStatus, BranchMatrix, 0x00);
MARATHON_ASSERT_OFFSETOF(_ackBranchStatus, MyCount, 0x40);
MARATHON_ASSERT_OFFSETOF(_ackBranchStatus, LoopNum, 0x44);
MARATHON_ASSERT_OFFSETOF(_ackBranchStatus, DefaultFog, 0x48);
MARATHON_ASSERT_OFFSETOF(_ackBranchStatus, Fog, 0x4C);
MARATHON_ASSERT_OFFSETOF(_ackBranchStatus, SortGroup, 0x50);
MARATHON_ASSERT_OFFSETOF(_ackBranchStatus, pCoordSpangle, 0x54);
MARATHON_ASSERT_OFFSETOF(_ackBranchStatus, NumPrimitive, 0x58);
MARATHON_ASSERT_OFFSETOF(_ackBranchStatus, pPrimitive, 0x5C);
MARATHON_ASSERT_SIZEOF(_ackBranchStatus, 0x60);
