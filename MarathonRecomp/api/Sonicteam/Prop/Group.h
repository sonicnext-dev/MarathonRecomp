#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/SceneObject.h>

namespace Sonicteam::Prop
{
    class Group : public SceneObject
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
    };

    MARATHON_ASSERT_SIZEOF(Group, 0x10);
}
