#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/Class.h>
#include <Sonicteam/Prop/SceneObject.h>

namespace Sonicteam::Prop
{
    class Instance : public SceneObject
    {
    public:
        MARATHON_INSERT_PADDING(0x04);
        xpointer<Class> m_pClass;
    };

    MARATHON_ASSERT_OFFSETOF(Instance, m_pClass, 0x10);
    MARATHON_ASSERT_SIZEOF(Instance, 0x14);
}
