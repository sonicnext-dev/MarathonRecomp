#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/kine/_ackLeaf.h>

struct _ackPrimitiveControl
{
    xpointer<_ackLeaf> pLeaf;
    be<SpFloat32> Distance;
};

MARATHON_ASSERT_OFFSETOF(_ackPrimitiveControl, pLeaf, 0x00);
MARATHON_ASSERT_OFFSETOF(_ackPrimitiveControl, Distance, 0x04);
MARATHON_ASSERT_SIZEOF(_ackPrimitiveControl, 0x08);
