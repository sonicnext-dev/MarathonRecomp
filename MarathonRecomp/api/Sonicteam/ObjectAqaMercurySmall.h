#pragma once

#include <Marathon.inl>
#include <Sonicteam/ObjectPsi.h>
#include <Sonicteam/PropFixture.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Scenery/Drawable.h>
#include <Sonicteam/SoX/Physics/Body.h>
#include <Sonicteam/SoX/Physics/Phantom.h>

namespace Sonicteam
{
    class ObjectAqaMercurySmallInfo;
    class MercuryDrawable;

    class ObjectAqaMercurySmall : public PropFixture
    {
    public:
        enum MercuryState
        {
            MercuryState_Default,
            MercuryState_Bounce,
            MercuryState_Follow,
            MercuryState_State3,
            MercuryState_State4
        };

        xpointer<ObjectAqaMercurySmallInfo> m_pMercurySmallInfo;
        char m_Field184;
        char m_Field185;
        char m_DoPsiCatch;
        char m_ReleasePsiCatch;
        char m_IsHasMercuryChild;
        MARATHON_INSERT_PADDING(3);
        be<uint32_t> m_pFlags18C;
        be<uint32_t> m_pFlags190;
        be<uint32_t> m_pFlags194;
        be<float> m_Frield198;    // Bounce Const
        be<float> m_BounceSpeed;  // Bounce Speed?
        be<float> m_Field1A0;
        be<float> m_Field1A4;
        be<float> m_Health;     // HP?
        be<float> m_Field1AC;
        be<uint32_t> m_Field1B0;
        be<uint32_t> m_FollowActorID; 
        be<uint32_t> m_ChildMercuryActorID;     
        be<uint32_t> m_Field1BC;
        xpointer<void> m_pField1C0;
        SoX::Math::Vector m_Vector1D0;
        SoX::Math::Vector m_Vector1E0;
        SoX::Math::Vector m_Vector1F0;
        MARATHON_INSERT_PADDING(0x10);
        SoX::Math::Vector m_Vector210;
        MARATHON_INSERT_PADDING(0x20);
        SoX::RefSharedPointer<SoX::RefCountObject> m_spField240;
        SoX::RefSharedPointer<MercuryDrawable> m_spMercuryDrawable;
        SoX::RefSharedPointer<SoX::Physics::Body> m_spPhysBody;
        SoX::RefSharedPointer<SoX::Physics::Phantom> m_spPhysPhantom;
        be<uint32_t> m_PreMercuryState;
        be<uint32_t> m_PostMercuryState;
    };

    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_pMercurySmallInfo, 0x180);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Field184, 0x184);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Field185, 0x185);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_DoPsiCatch, 0x186);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_ReleasePsiCatch, 0x187);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_IsHasMercuryChild, 0x188);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_pFlags18C, 0x18C);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_pFlags190, 0x190);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_pFlags194, 0x194);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Frield198, 0x198);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_BounceSpeed, 0x19C);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Field1A0, 0x1A0);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Field1A4, 0x1A4);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Health, 0x1A8);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Field1AC, 0x1AC);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Field1B0, 0x1B0);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_FollowActorID, 0x1B4);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_ChildMercuryActorID, 0x1B8);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Field1BC, 0x1BC);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_pField1C0, 0x1C0);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Vector1D0, 0x1D0);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Vector1E0, 0x1E0);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Vector1F0, 0x1F0);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_Vector210, 0x210);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_spField240, 0x240);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_spMercuryDrawable, 0x244);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_spPhysBody, 0x248);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_spPhysPhantom, 0x24C);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_PreMercuryState, 0x250);
    MARATHON_ASSERT_OFFSETOF(ObjectAqaMercurySmall, m_PostMercuryState, 0x254);
}
