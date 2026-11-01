#pragma once

#include <Marathon.inl>
#include <Cal/CCalBase.h>

class CCalRenderer : public CCalBase
{
public:
    MARATHON_INSERT_PADDING(0x0C);
};

MARATHON_ASSERT_SIZEOF(CCalRenderer, 0x38);
