#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/Entity.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::SoX::Physics
{
    struct EntityRef 
    {
    public:
        xpointer<Entity> m_pEntity;
        Sonicteam::SoX::LinkNode<Entity> m_lnEntity;
    };
}
