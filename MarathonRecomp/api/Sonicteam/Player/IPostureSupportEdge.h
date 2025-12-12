#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Physics/ShapeCast.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/Player/WallListener.h>
#include <Sonicteam/Player/GroundListener.h>

namespace Sonicteam::Player
{
    class IPostureSupportEdge
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0xC);
        boost::shared_ptr<WallListener> m_spWallListener;
        SoX::RefSharedPointer<SoX::Physics::ShapeCast> m_spCollisionEdge;
        xpointer<be<uint32_t>> m_pPostureRequestFlag;
        SoX::Math::Vector m_EdgeBottomPosition;             // -72.00,30.27 -0.53
        SoX::Math::Vector m_EdgeTopPosistion;               // -72.00 50.27 -0.53
        SoX::Math::Vector m_EdgeOffset;                     // Vector operations & Lua Parameters
        SoX::Math::Vector m_EdgeHeight;                     // Lua Parameter but c_edge_height(Y)
        MARATHON_INSERT_PADDING(0x10);

        void sub_82202B08(SoX::Math::Quaternion groundRotation);

        bool EdgeGrabLeftOverFunction(SoX::Math::Vector position);

    };

    MARATHON_ASSERT_SIZEOF(IPostureSupportEdge, 112);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportEdge, m_spWallListener, 0x10);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportEdge, m_spCollisionEdge, 0x18);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportEdge, m_pPostureRequestFlag, 0x1C);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportEdge, m_EdgeBottomPosition, 0x20);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportEdge, m_EdgeTopPosistion, 0x30);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportEdge, m_EdgeOffset, 0x40);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportEdge, m_EdgeHeight, 0x50);
}

#include "IPostureSupportEdge.inl"
