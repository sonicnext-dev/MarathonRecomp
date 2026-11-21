#pragma once

#include <Marathon.inl>
#include <Kaim/CEntityAttribute.h>

namespace Sonicteam::KynapseSkel
{
    class CAiMeshEntityAttribute : public Kaim::CEntityAttribute
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_SIZEOF(CAiMeshEntityAttribute, 0x08);
}
