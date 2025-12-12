#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/Physics/ShapeCast.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player
{
    class GroundListener;
    class WallListener;

    class IPostureSupportOttoto
    {
    public:
        xpointer<void> m_pVftable;
        MARATHON_INSERT_PADDING(0xC);
        boost::shared_ptr<GroundListener> m_spGroundListener;
        boost::shared_ptr<WallListener> m_spWallListener;
        SoX::RefSharedPointer<SoX::Physics::ShapeCast> m_spCollisionOttoto;
        xpointer<be<uint32_t>> m_pPostureRequestFlag;
        SoX::Math::Vector m_GroundAngle;
        SoX::Math::Vector m_WallAngle;
        SoX::Math::Vector m_OttotoVolume;
        be<float> m_OttotoHeight;

        void sub_82201620(SoX::Math::Vector invertNormal, SoX::Math::Quaternion groundRotation);

        bool sub_82201758(SoX::Math::Vector position1, SoX::Math::Vector position2);
    };

    MARATHON_ASSERT_SIZEOF(IPostureSupportOttoto, 112);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportOttoto, m_spGroundListener, 0x10);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportOttoto, m_spWallListener, 0x18);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportOttoto, m_spCollisionOttoto, 0x20);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportOttoto, m_pPostureRequestFlag, 0x24);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportOttoto, m_GroundAngle, 0x30);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportOttoto, m_WallAngle, 0x40);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportOttoto, m_OttotoVolume, 0x50);
    MARATHON_ASSERT_OFFSETOF(IPostureSupportOttoto, m_OttotoHeight, 0x60);
}

#include "IPostureSupportOttoto.inl"
