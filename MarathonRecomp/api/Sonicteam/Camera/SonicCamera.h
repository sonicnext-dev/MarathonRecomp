#pragma once

#include <Marathon.inl>
#include <Sonicteam/Camera/CameraMode.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Physics/Entity.h>
#include <Sonicteam/SoX/Physics/ShapeCast.h>
#include <Sonicteam/SoX/Physics/ShapeCastListener.h>
#include <Sonicteam/SoX/RefLinkNode.h>

namespace Sonicteam::Camera
{
    class SonicCamera : public CameraMode
    {
    public:
        class MoveShapeCastListener : public MARATHON_VIRTUAL_BASE<SoX::Physics::ShapeCastListener, 0x82002258>
        {
        public:
            struct MoveShapeCastContext
            {
                SoX::Math::Vector HitPosition{};
                SoX::Math::Vector Field10{};
                be<float> Field20{};
                be<uint32_t> Field24{};
                SoX::RefLinkNode<SoX::Physics::Entity> Field28{};
            };

            xpointer<MoveShapeCastContext> m_pContext{};

            MoveShapeCastListener() = default;

            explicit MoveShapeCastListener(MoveShapeCastContext* in_pContext) : m_pContext(in_pContext) {}

            ~MoveShapeCastListener()
            {
                GuestToHostFunction<int>(sub_82595180, &m_pContext->Field28);
            }
        };

        SoX::Math::Quaternion m_Rotation;
        MARATHON_INSERT_PADDING(0x04);
        be<float> m_Spring;
        be<float> m_Damping;
        SoX::Math::Vector2 m_Acceleration;
        be<float> m_AzDrive;
        be<float> m_AzDamping;
        be<float> m_AltDrive;
        be<float> m_AltDamping;
        be<uint32_t> m_TargetActorID;
        xpointer<SoX::Physics::ShapeCast> m_pShapeCastA;
        xpointer<SoX::Physics::ShapeCast> m_pShapeCastB;
        MARATHON_INSERT_PADDING(0x30);
        be<float> m_FOV;
        MARATHON_INSERT_PADDING(0x0C);
        SoX::Math::Vector m_Position;
        SoX::Math::Vector m_Target;
        SoX::Math::Vector m_Velocity;
        SoX::Math::Vector m_Orbit;
    };

    MARATHON_ASSERT_OFFSETOF(SonicCamera::MoveShapeCastListener::MoveShapeCastContext, HitPosition, 0x00);
    MARATHON_ASSERT_OFFSETOF(SonicCamera::MoveShapeCastListener::MoveShapeCastContext, Field10, 0x10);
    MARATHON_ASSERT_OFFSETOF(SonicCamera::MoveShapeCastListener::MoveShapeCastContext, Field20, 0x20);
    MARATHON_ASSERT_OFFSETOF(SonicCamera::MoveShapeCastListener::MoveShapeCastContext, Field24, 0x24);
    MARATHON_ASSERT_OFFSETOF(SonicCamera::MoveShapeCastListener::MoveShapeCastContext, Field28, 0x28);
    MARATHON_ASSERT_SIZEOF(SonicCamera::MoveShapeCastListener::MoveShapeCastContext, 0x40);

    MARATHON_ASSERT_OFFSETOF(SonicCamera::MoveShapeCastListener, m_pContext, 0x08);
    MARATHON_ASSERT_SIZEOF(SonicCamera::MoveShapeCastListener, 0x0C);

    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_Rotation, 0x30);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_Spring, 0x44);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_Damping, 0x48);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_Acceleration, 0x4C);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AzDrive, 0x54);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AzDamping, 0x58);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AltDrive, 0x5C);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_AltDamping, 0x60);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_TargetActorID, 0x64);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_pShapeCastA, 0x68);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_pShapeCastB, 0x6C);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_FOV, 0xA0);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_Position, 0xB0);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_Target, 0xC0);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_Velocity, 0xD0);
    MARATHON_ASSERT_OFFSETOF(SonicCamera, m_Orbit, 0xE0);
    MARATHON_ASSERT_SIZEOF(SonicCamera, 0xF0);
}
