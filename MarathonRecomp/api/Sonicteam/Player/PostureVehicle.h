#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/IPosturePlugIn.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IExportExternalFlag.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/IExportVehicleFlag.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/SoX/Physics/RayCast.h>
#include <Sonicteam/SoX/Physics/EntityRef.h>
#include <Sonicteam/Player/Input/IListener.h>
#include <Sonicteam/SoX/Graphics/Frame.h>
#include <Sonicteam/SoX/Math/Math.h>
#include <Sonicteam/SoX/Physics/EntityRef.h>
#include <Sonicteam/SoX/Engine/Task.h>
#include <stdx/deque.h>

namespace Sonicteam::Player
{
    class PostureVehicle : public IPlugIn, public IPosturePlugIn, public IDynamicLink, public IExportExternalFlag, public IStepable, public IFlagCommunicator, public IExportVehicleFlag
    {
    public:
        SoX::RefSharedPointer<SoX::Physics::World> m_spWorld;
        SoX::RefSharedPointer<SoX::Physics::RayCast> m_spRayCast;
        SoX::Physics::EntityRef m_CollideEntity;
        SoX::RefSharedPointer<SoX::Graphics::Frame> m_spAttachNode;
        boost::shared_ptr<Input::IListener> m_spVehicleListener;
        MARATHON_INSERT_PADDING(0x8);
        be<uint32_t> m_VehicleFlag1;
        be<uint32_t> m_VehicleFlag2;
        be<uint32_t> m_VehicleFlag3;
        MARATHON_INSERT_PADDING(4);
        bool m_InVehicle; // collision checker
        SoX::Math::Vector m_Rotation;
        SoX::Math::Vector m_Position;
        stdx::deque<SoX::Physics::EntityRef> m_dEntityRef;
        xpointer<SoX::Engine::Task> m_pPlayer;
        xpointer<SoX::RefCountObject> m_spComboAttackManager;
        bool m_IsBroken; // Contact Flag

        template <typename T = SoX::Physics::RayCast>
        T* GetRayCast()
        {
            return static_cast<T*>(m_spRayCast.get());
        }
        
    };

    MARATHON_ASSERT_SIZEOF(PostureVehicle, 0xC0);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_spWorld, 0x38);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_spRayCast, 0x3C);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_CollideEntity, 0x40);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_spAttachNode, 0x50);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_spVehicleListener, 0x54);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_VehicleFlag1, 0x64);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_VehicleFlag2, 0x68);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_VehicleFlag3, 0x6C);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_InVehicle, 0x74);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_Rotation, 0x80);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_Position, 0x90);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_dEntityRef, 0xA0);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_pPlayer, 0xB4);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_spComboAttackManager, 0xB8);
    MARATHON_ASSERT_OFFSETOF(PostureVehicle, m_IsBroken, 0xBC);
}
