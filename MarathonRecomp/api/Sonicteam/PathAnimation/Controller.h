#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/LinkNode.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::PathAnimation
{
    class Entity;

    class Controller : public SoX::RefCountObject
    {
    public:
        xpointer<Entity> m_pEntity;
        SoX::LinkNode<Entity> m_lnEntity;
        SoX::Math::Vector m_Position;
        SoX::Math::Vector m_Up;
        SoX::Math::Vector m_Tangent;
    };

    MARATHON_ASSERT_SIZEOF(Controller, 0x50);
    MARATHON_ASSERT_OFFSETOF(Controller, m_pEntity, 0x8);
    MARATHON_ASSERT_OFFSETOF(Controller, m_lnEntity, 0xC);
    MARATHON_ASSERT_OFFSETOF(Controller, m_Position, 0x20);
    MARATHON_ASSERT_OFFSETOF(Controller, m_Up, 0x30);
    MARATHON_ASSERT_OFFSETOF(Controller, m_Tangent, 0x40);
}
