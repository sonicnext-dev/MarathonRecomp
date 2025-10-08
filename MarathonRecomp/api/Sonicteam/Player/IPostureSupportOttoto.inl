#pragma once

namespace Sonicteam::Player
{
    // Recreated from sub_82201620
    inline void IPostureSupportOttoto::sub_82201620(SoX::Math::Vector invertNormal, SoX::Math::Quaternion groundRotation)
    {
        m_GroundAngle = groundRotation * m_OttotoVolume;
        m_WallAngle = (invertNormal * m_OttotoHeight) + m_GroundAngle;
        if (m_spGroundListener.get())
        {
            m_spGroundListener->m_CollisionNormal = invertNormal.InvertNormal();
        }
        if (m_spWallListener.get())
        {
            m_spWallListener->m_CollisionNormal = invertNormal.InvertNormal();
        }
    }

    // Recreated from sub_82201758
    inline bool IPostureSupportOttoto::sub_82201758(SoX::Math::Vector position1, SoX::Math::Vector position2)
    {
        if ((this->m_pPostureRequestFlag.get()->get() & 0x40) != 0)
            return 0;

        auto v38 = position1 + m_WallAngle;
        auto v39 = position1 + m_GroundAngle;

        struct _points_
        {
            SoX::Math::Vector p1;
            SoX::Math::Vector p2;
        };
        guest_heap_var<_points_> _g_points_(v39, v38);
        m_spGroundListener->SetPostureRequestFlag(m_pPostureRequestFlag.get()->get());
        m_spCollisionOttoto->Func3C((SoX::Math::Vector*)_g_points_.get(), static_cast<SoX::Physics::ShapeCastListener*>(m_spGroundListener.get()));

        m_spGroundListener->Update();
        auto groundContact = m_spGroundListener->GetCurrentSurfaceContacts() > 0;

        auto v42 = position2 + m_WallAngle;
        auto v40 = position2 + m_GroundAngle;
        *_g_points_ =
        {
            v40,
            v42
        };
        m_spGroundListener->SetPostureRequestFlag(m_pPostureRequestFlag.get()->get());
        m_spCollisionOttoto->Func3C((SoX::Math::Vector*)_g_points_.get(), static_cast<SoX::Physics::ShapeCastListener*>(m_spGroundListener.get()));
        m_spGroundListener->Update();
        auto groundContact2 = m_spGroundListener->GetCurrentSurfaceContacts() > 0;

        auto v41 = position2 + m_GroundAngle;
        auto v37 = position1 + m_GroundAngle;
        *_g_points_ =
        {
            v37,
            v41
        };

        m_spWallListener->SetPostureRequestFlag(m_pPostureRequestFlag.get()->get());
        m_spCollisionOttoto->Func3C((SoX::Math::Vector*)_g_points_.get(), static_cast<SoX::Physics::ShapeCastListener*>(m_spWallListener.get()));
        m_spWallListener->Update();
        auto wallContact = m_spWallListener->GetCurrentSurfaceContacts() > 0;

        if (!groundContact || groundContact2 || !wallContact)
        {
            return false;
        }
        else
        {
            return true;
        }

    }
}
