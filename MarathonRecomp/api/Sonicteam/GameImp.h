#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/SoX/Scenery/Camera.h>
#include <Sonicteam/SoX/Scenery/CameraImp.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    class GameImp : public SoX::MessageReceiver
    {
    public:
        enum GameState : uint32_t
        {
            GameState_MainMenu,
            GameState_Stage,
            GameState_Event,
            GameState_Movie,
            GameState_Result,
            GameState_Message,
            GameState_6,
            GameState_Save,
            GameState_ReturnToMainMenu
        };

        struct PlayerData
        {
            be<uint32_t> ActorID;
            be<uint32_t> RingCount;
            MARATHON_INSERT_PADDING(4);
            be<uint32_t> LifeCount;
            be<uint32_t> ScoreCount;
            be<float> AliveTime;
            be<float> Time;
            MARATHON_INSERT_PADDING(4);
            be<float> SectionSaveTime;
            be<float> GaugeValue;
            be<uint32_t> MaturityLevel;
            be<float> MaturityValue;
            MARATHON_INSERT_PADDING(4);
            be<uint32_t> ExtendRingCount;
            be<uint32_t> GemIndex;
            MARATHON_INSERT_PADDING(0x10);
        };

        MARATHON_INSERT_PADDING(4);
        be<GameState> m_GameState;
        xpointer<DocMarathonState> m_pDoc;
        be<uint32_t> m_Flags;
        MARATHON_INSERT_PADDING(0xE2C);
        PlayerData m_PlayerData[4];
        MARATHON_INSERT_PADDING(0x200);
        bool m_IsStage;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_Field1180;
        MARATHON_INSERT_PADDING(0x58);
        xpointer<stdx::vector<boost::shared_ptr<SoX::Scenery::Camera>>> m_pvspCameras;
        MARATHON_INSERT_PADDING(0x7DC);
        SoX::RefSharedPointer<SoX::Physics::World> m_spPhysicsWorld;
        xpointer<void> m_pMyCollisionFilter;

        int PlayerActorIDToIndex(int32_t actorId) const
        {
            for (int i = 0; i < 4; i++)
            {
                if (m_PlayerData[i].ActorID == actorId)
                    return i;
            }

            return -1;
        }

        SoX::Scenery::CameraImp* GetCameraImp(const char* pName) const
        {
            if (!m_pvspCameras)
                return nullptr;

            for (auto& spCamera : *m_pvspCameras)
            {
                auto pCameraImp = (SoX::Scenery::CameraImp*)spCamera.get();

                if (pCameraImp->m_Name == pName)
                    return pCameraImp;
            }

            return nullptr;
        }

        template <typename T = SoX::Physics::World>
        T* GetPhysicsWorld() const
        {
            return (T*)m_spPhysicsWorld.get();
        }
    };
}
