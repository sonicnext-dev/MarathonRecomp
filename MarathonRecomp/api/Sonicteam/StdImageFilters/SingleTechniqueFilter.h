#pragma once

#include <Marathon.inl>

namespace Sonicteam::StdImageFilters
{
    class SingleTechniqueFilter : public ImageFilter
    {
    public:
        MARATHON_INSERT_PADDING(0x10);
    };

    MARATHON_ASSERT_SIZEOF(SingleTechniqueFilter, 0x18);
}
