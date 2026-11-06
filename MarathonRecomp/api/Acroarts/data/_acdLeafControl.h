#pragma once

#include <Marathon.inl>
#include <Acroarts/data/res/_acdResource.h>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/kine/SpkTypes.h>

struct _acdLeafControl
{
    be<SpUint32> Flags;
    be<SpSint32> Id;
    be<SpUint32> CoordType;
    be<SpFloat32> LifeTimeMin;
    be<SpFloat32> LifeTimeMax;
    be<SpFloat32> SleepTime;
    be<SpFloat32> DelayTimeMax;
    be<SpFloat32> GenInterval;
    be<SpFloat32> GenLifeTime;
    be<SpFloat32> GenRate;
    be<SpFloat32> GenNumMin;
    be<SpFloat32> GenNumMax;
    be<SpFloat32> LODDistStart;
    be<SpFloat32> LODDistEnd;
    be<SpFloat32> LODRate;
    be<SpFloat32> LODNumMin;
    be<SpFloat32> LODNumMax;
    be<SpSint32> MessageParam0;
    be<SpSint32> MessageParam1;
    be<SpFloat32> ClipRange;
    be<SpFloat32> ClipZNearRange;
    be<SpFloat32> ClipZFarRange;
    be<SpkBlendType> BlendMode;
    be<SpUint32> ToLeafCoordTarget;
    be<SpUint32> ToLeafCoordType;
    be<SpUint32> ToLeafCoordNode;
    be<SpUint32> ModelAttachIndex;
    be<SpSint32> ModelType;
    be<SpFloat32> PrimX0;
    be<SpFloat32> PrimY0;
    be<SpFloat32> PrimX1;
    be<SpFloat32> PrimY1;
    be<SpSint32> ResType;
    _acdResource Resource;
    be<SpUint32> TrOpCtrlFlag;
    be<SpUint32> MdlNo;
    be<SpUint32> MotNo;
    be<SpUint32> TexNo;
};

MARATHON_ASSERT_OFFSETOF(_acdLeafControl, Flags, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, Id, 0x04);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, CoordType, 0x08);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, LifeTimeMin, 0x0C);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, LifeTimeMax, 0x10);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, SleepTime, 0x14);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, DelayTimeMax, 0x18);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, GenInterval, 0x1C);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, GenLifeTime, 0x20);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, GenRate, 0x24);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, GenNumMin, 0x28);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, GenNumMax, 0x2C);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, LODDistStart, 0x30);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, LODDistEnd, 0x34);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, LODRate, 0x38);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, LODNumMin, 0x3C);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, LODNumMax, 0x40);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, MessageParam0, 0x44);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, MessageParam1, 0x48);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, ClipRange, 0x4C);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, ClipZNearRange, 0x50);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, ClipZFarRange, 0x54);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, BlendMode, 0x58);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, ToLeafCoordTarget, 0x5C);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, ToLeafCoordType, 0x60);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, ToLeafCoordNode, 0x64);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, ModelAttachIndex, 0x68);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, ModelType, 0x6C);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, PrimX0, 0x70);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, PrimY0, 0x74);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, PrimX1, 0x78);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, PrimY1, 0x7C);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, ResType, 0x80);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, Resource, 0x84);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, TrOpCtrlFlag, 0xA4);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, MdlNo, 0xA8);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, MotNo, 0xAC);
MARATHON_ASSERT_OFFSETOF(_acdLeafControl, TexNo, 0xB0);
MARATHON_ASSERT_SIZEOF(_acdLeafControl, 0xB4);
