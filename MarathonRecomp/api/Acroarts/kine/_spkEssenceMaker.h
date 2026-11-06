#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

struct _spkEssenceMaker
{
    xpointer<void> fEssenceSetup;
    be<SpUint32> EssenceId;
    xpointer<_spkEssenceMaker> pNext;
};

MARATHON_ASSERT_OFFSETOF(_spkEssenceMaker, fEssenceSetup, 0x00);
MARATHON_ASSERT_OFFSETOF(_spkEssenceMaker, EssenceId, 0x04);
MARATHON_ASSERT_OFFSETOF(_spkEssenceMaker, pNext, 0x08);
MARATHON_ASSERT_SIZEOF(_spkEssenceMaker, 0x0C);
