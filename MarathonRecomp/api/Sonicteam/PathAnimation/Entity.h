#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::PathAnimation
{
    class Controller;
    class Animation;

    class Entity : public SoX::RefCountObject
    {
    public:
        SoX::LinkNode<Controller> m_lnController;
        SoX::RefSharedPointer<Animation> m_PathAnimation;
    };

    MARATHON_ASSERT_SIZEOF(Entity, 0x18);
    MARATHON_ASSERT_OFFSETOF(Entity, m_lnController, 0x8);
    MARATHON_ASSERT_OFFSETOF(Entity, m_PathAnimation, 0x14);
}
