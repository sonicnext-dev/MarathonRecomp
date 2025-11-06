#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/Mission/Core.h>
#include <Sonicteam/SoX/Audio/Cue.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/SoX/Scenery/Camera.h>
#include <Sonicteam/SoX/Scenery/CameraImp.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/Game.h>
#include <Sonicteam/PopupScreenTask.h>
#include <Sonicteam/TextBook.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    class ActorManager;
    class GameScript;

    class GameImp : public Game
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

        enum GameFlags : uint32_t
        {
            GameFlags_RestartArea1 = 1,
            GameFlags_RestartArea2 = 0x200,
            GameFlags_IsPaused = 0x1000,
            GameFlags_LoadArea1 = 0x40000,
            GameFlags_LoadArea2 = 0x200000
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
            be<float> SectionTime;
            be<float> GaugeValue;
            be<uint32_t> MaturityLevel;
            be<float> MaturityValue;
            MARATHON_INSERT_PADDING(4);
            be<uint32_t> ExtendRingCount;
            be<uint32_t> GemIndex;
            MARATHON_INSERT_PADDING(0x10);
        };

        MARATHON_INSERT_PADDING(4);
        be<GameState> m_State;
        xpointer<DocMarathonState> m_pDoc;
        be<GameFlags> m_Flags;
        MARATHON_INSERT_PADDING(0xE2C);
        PlayerData m_PlayerData[4];
        MARATHON_INSERT_PADDING(0x200);
        bool m_IsStage;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_Field1180;
        xpointer<GameScript> m_pGameScript;
        be<uint32_t> m_aObjPlayerActorID[0x0F];
        boost::shared_ptr<ActorManager> m_spActorManager;
        xpointer<TextBook> m_pSystemTextBook;
        MARATHON_INSERT_PADDING(8);
        stdx::vector<stdx::vector<boost::shared_ptr<SoX::Scenery::Camera>>> m_vvspCameras;
        MARATHON_INSERT_PADDING(0x1B4);
        xpointer<SoX::Audio::Cue> m_pBgmCue;
        MARATHON_INSERT_PADDING(0x36C);
        xpointer<TextBook> m_pHintTextBook;
        MARATHON_INSERT_PADDING(4);
        xpointer<Mission::Core> m_pMissionCore;
        MARATHON_INSERT_PADDING(0x248);
        SoX::LinkRef<Sonicteam::PopupScreenTask> m_lrPopupScreenTask;
        MARATHON_INSERT_PADDING(0x4C);
        SoX::RefSharedPointer<SoX::Physics::World> m_spPhysicsWorld;
        xpointer<void> m_pMyCollisionFilter;
        MARATHON_INSERT_PADDING(0x0C);

        int PlayerActorIDToIndex(int32_t actorId) const
        {
            for (int i = 0; i < 4; i++)
            {
                if (m_PlayerData[i].ActorID == actorId)
                    return i;
            }

            return -1;
        }

        SoX::Scenery::CameraImp* GetCamera(const char* pName, int which = 0)
        {
            if (m_vvspCameras.empty())
                return nullptr;

            for (auto& spCamera : m_vvspCameras[which])
            {
                auto pCameraImp = (SoX::Scenery::CameraImp*)spCamera.get();

                if (pCameraImp->m_Name == pName)
                    return pCameraImp;
            }

            return nullptr;
        }

        template <typename T = SoX::Audio::Cue>
        T* GetBgmCue() const
        {
            return (T*)m_pBgmCue.get();
        }

        template <typename T = SoX::Physics::World>
        T* GetPhysicsWorld() const
        {
            return (T*)m_spPhysicsWorld.get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, ActorID, 0x00);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, RingCount, 0x04);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, LifeCount, 0x0C);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, ScoreCount, 0x10);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, AliveTime, 0x14);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, Time, 0x18);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, SectionTime, 0x20);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, GaugeValue, 0x24);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, MaturityLevel, 0x28);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, MaturityValue, 0x2C);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, ExtendRingCount, 0x34);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, GemIndex, 0x38);
    MARATHON_ASSERT_SIZEOF(GameImp::PlayerData, 0x4C);

    MARATHON_ASSERT_OFFSETOF(GameImp, m_State, 0x08);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pDoc, 0x0C);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_Flags, 0x10);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_PlayerData, 0xE40);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_IsStage, 0x1170);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_Field1180, 0x1180);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pGameScript, 0x1184);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_aObjPlayerActorID, 0x1188);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_spActorManager, 0x11C4);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pSystemTextBook, 0x11CC);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_vvspCameras, 0x11D8);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pBgmCue, 0x139C);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pHintTextBook, 0x170C);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pMissionCore, 0x1714);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_spPhysicsWorld, 0x19BC);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pMyCollisionFilter, 0x19C0);
    MARATHON_ASSERT_SIZEOF(GameImp, 0x19D0);
}
