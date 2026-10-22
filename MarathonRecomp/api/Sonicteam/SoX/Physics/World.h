#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/IntersectListener.h>
#include <Sonicteam/SoX/RefCountObject.h>

namespace Sonicteam::SoX::Physics
{
    class World : public RefCountObject
    {
    public:
        struct Vftable
        {
            MARATHON_INSERT_PADDING(0x10);
            be<uint32_t> fpTestIntersectLine;
        };

        bool TestIntersectLine(Math::Vector* in_pLineParams, IntersectListener* in_pIntersectListener)
        {
            return GuestToHostFunction<bool>(sub_825BFED8, this, in_pLineParams, in_pIntersectListener, 0);
        }
    };

    MARATHON_ASSERT_SIZEOF(World, 8);
}
