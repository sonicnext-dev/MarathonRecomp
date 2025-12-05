#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player
{
    // Recreated from sub_82202B08--
    inline void IPostureSupportEdge::sub_82202B08(SoX::Math::Quaternion groundRotation)
    {
        this->m_EdgeBottomPosition = groundRotation * m_EdgeOffset;
        this->m_EdgeTopPosistion = m_EdgeBottomPosition + m_EdgeHeight;
    }

    // position 82200B94, Function was removed, i've tried to restore depending on how they usually do it
    inline bool IPostureSupportEdge::EdgeGrabLeftOverFunction(SoX::Math::Vector position)
    {
        return false;

        if (!m_spWallListener)
            return false;

        auto ledgeBottom = position + m_EdgeBottomPosition;
        auto ledgeTop = position + m_EdgeTopPosistion;

        m_spWallListener->SetPostureRequestFlag(m_pPostureRequestFlag->get());

        struct LedgeDetectionVolume
        {
            SoX::Math::Vector bottomSearch;
            SoX::Math::Vector topSearch;
        };

        guest_heap_var<LedgeDetectionVolume> ledgeVolume(ledgeBottom, ledgeTop);

        m_spCollisionEdge->Func3C(
            reinterpret_cast<SoX::Math::Vector*>(ledgeVolume.get()),
            static_cast<SoX::Physics::ShapeCastListener*>(m_spWallListener.get())
        );

        m_spWallListener->Update();

        if (m_spWallListener->GetCurrentSurfaceContacts() > 0)
        {
            return true;
        }

        return false;
    }
}
