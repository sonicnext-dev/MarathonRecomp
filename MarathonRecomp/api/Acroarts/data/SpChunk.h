#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct SpChunk
{
    be<SpUint32> tag;
    be<SpUint32> size;
    be<SpUint32> headsize;
    be<SpUint16> systemflag;
    be<SpUint16> userflag;
};

MARATHON_ASSERT_OFFSETOF(SpChunk, tag, 0x00);
MARATHON_ASSERT_OFFSETOF(SpChunk, size, 0x04);
MARATHON_ASSERT_OFFSETOF(SpChunk, headsize, 0x08);
MARATHON_ASSERT_OFFSETOF(SpChunk, systemflag, 0x0C);
MARATHON_ASSERT_OFFSETOF(SpChunk, userflag, 0x0E);
MARATHON_ASSERT_SIZEOF(SpChunk, 0x10);
