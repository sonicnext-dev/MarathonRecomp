#pragma once

namespace Sonicteam::Player
{
    // Recreated from sub_8223B148
    inline bool PhysicsBodyList::sub_8223B148(SoX::Physics::EntityRef* entityRef)
    {
        if (entityRef->sub_82595500(0))
        {
            if (auto entity = entityRef->sub_82595020())
            {
                if (entity->m_pReceiver.get())
                    m_vlrBody.push_back(*entityRef);

            }
            return true;
        }
        return false;
    }
};
    
