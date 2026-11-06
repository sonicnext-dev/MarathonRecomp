#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>

class SpkBase
{
public:
    struct Vftable
    {
        be<uint32_t> fpDtor;
    };

    xpointer<Vftable> _vptr_SpkBase;
    be<SpUint32> _TagName;
    SpAddr _CreatePtr;
    be<SpUint16> _ObjectStatus;
};

MARATHON_ASSERT_OFFSETOF(SpkBase::Vftable, fpDtor, 0x00);
MARATHON_ASSERT_SIZEOF(SpkBase::Vftable, 0x04);

MARATHON_ASSERT_OFFSETOF(SpkBase, _vptr_SpkBase, 0x00);
MARATHON_ASSERT_OFFSETOF(SpkBase, _TagName, 0x04);
MARATHON_ASSERT_OFFSETOF(SpkBase, _CreatePtr, 0x08);
MARATHON_ASSERT_OFFSETOF(SpkBase, _ObjectStatus, 0x0C);
MARATHON_ASSERT_SIZEOF(SpkBase, 0x10);
