#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam
{
    class ObjInfo : public SoX::RefCountObject
    {
    public:
        MARATHON_INSERT_PADDING(0x20);
    };

    MARATHON_ASSERT_SIZEOF(ObjInfo, 0x28);
}
