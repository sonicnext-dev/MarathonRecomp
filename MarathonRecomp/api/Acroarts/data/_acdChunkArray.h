#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpChunk.h>
#include <Acroarts/data/SpTypes.h>

struct _acdChunkArray
{
    xpointer<SpChunk> pCnkData;
    be<SpUint32> Param;
};

MARATHON_ASSERT_OFFSETOF(_acdChunkArray, pCnkData, 0x00);
MARATHON_ASSERT_OFFSETOF(_acdChunkArray, Param, 0x04);
MARATHON_ASSERT_SIZEOF(_acdChunkArray, 0x08);
