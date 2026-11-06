#pragma once

#include <Marathon.inl>
#include <Acroarts/data/SpTypes.h>
#include <Acroarts/kine/SpkBase.h>
#include <Acroarts/kine/SpkTypes.h>

class SpkSpangleBase : public SpkBase
{
public:
    SpMatrix _Matrix;
    SpColorReal _MaterialColor;
    SpColorReal _Color;
    be<SpkBlendType> _BlendType;
    be<SpUint32> _SortGroup;
    SpkDrawActionType _DrawAction;
    be<SpUint32> _Flags;
};

MARATHON_ASSERT_OFFSETOF(SpkSpangleBase, _Matrix, 0x10);
MARATHON_ASSERT_OFFSETOF(SpkSpangleBase, _MaterialColor, 0x50);
MARATHON_ASSERT_OFFSETOF(SpkSpangleBase, _Color, 0x60);
MARATHON_ASSERT_OFFSETOF(SpkSpangleBase, _BlendType, 0x70);
MARATHON_ASSERT_OFFSETOF(SpkSpangleBase, _SortGroup, 0x74);
MARATHON_ASSERT_OFFSETOF(SpkSpangleBase, _DrawAction, 0x78);
MARATHON_ASSERT_OFFSETOF(SpkSpangleBase, _Flags, 0x7C);
MARATHON_ASSERT_SIZEOF(SpkSpangleBase, 0x80);
