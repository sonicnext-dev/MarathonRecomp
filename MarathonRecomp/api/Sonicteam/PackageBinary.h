#pragma once

#include <Marathon.inl>
#include <Sonicteam/Package.h>

namespace Sonicteam
{
    class PackageBinary : public Package
    {
    public:
        MARATHON_INSERT_PADDING(0x18);
    };

    MARATHON_ASSERT_SIZEOF(PackageBinary, 0x7C);
}
