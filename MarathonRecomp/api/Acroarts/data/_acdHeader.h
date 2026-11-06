#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpChunk.h>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/data/_acdChunkArray.h>

struct _acdHeader
{
    SpChunk CnkHeader;
    be<SpUint32> Version;
    be<SpUint32> TrunkNum;
    be<SpUint32> POF0Offset;
    MARATHON_INSERT_PADDING(0x04);
    xpointer<_acdChunkArray> pCnkArray;
};

MARATHON_ASSERT_OFFSETOF(_acdHeader, CnkHeader, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdHeader, Version, 0x10);
MARATHON_ASSERT_OFFSETOF(_acdHeader, TrunkNum, 0x14);
MARATHON_ASSERT_OFFSETOF(_acdHeader, POF0Offset, 0x18);
MARATHON_ASSERT_OFFSETOF(_acdHeader, pCnkArray, 0x20);
MARATHON_ASSERT_SIZEOF(_acdHeader, 0x24);
