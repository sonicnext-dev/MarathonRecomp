#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <Sonicteam/Mission/Core.h>
#include <Sonicteam/Prop/ActiveArea.h>
#include <Sonicteam/Prop/GenerateArea.h>
#include <Sonicteam/SoX/Audio/Cue.h>
#include <Sonicteam/SoX/Physics/World.h>
#include <Sonicteam/SoX/Scenery/Camera.h>
#include <Sonicteam/SoX/Scenery/CameraImp.h>
#include <Sonicteam/SoX/Array.h>
#include <Sonicteam/SoX/RefLinkNode.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/ActorManager.h>
#include <Sonicteam/Game.h>
#include <Sonicteam/KhronoController.h>
#include <Sonicteam/LoadingTask.h>
#include <Sonicteam/PauseAdapter.h>
#include <Sonicteam/PopupScreenTask.h>
#include <Sonicteam/TaskAdapter.h>
#include <Sonicteam/TextBook.h>
#include <stdx/string.h>
#include <stdx/vector.h>

namespace Sonicteam
{
    namespace Player
    {
        class Object;
    }

    class GameScript;
    class MovieTask;

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
            GameState_Quit,
            GameState_9
        };

        enum GameFlags : uint32_t
        {
            GameFlags_RestartArea1 = 1,
            GameFlags_RestartArea2 = 0x200,
            GameFlags_IsPaused = 0x1000,
            GameFlags_LoadArea1 = 0x40000,
            GameFlags_LoadArea2 = 0x200000
        };

        struct PlayerInfo
        {
            stdx::string StartPlayer;
            SoX::Math::Vector StartRotation;
            SoX::Math::Vector StartPosition;
            uint8_t Field40;
            stdx::string RespawnPlayer;
            SoX::Math::Vector RespawnRotation;
            SoX::Math::Vector RespawnPosition;
            SoX::Math::Vector Field80;
            SoX::Math::Vector Field90;
            SoX::Math::Vector FieldA0;
            SoX::Math::Vector FieldB0;
            uint8_t FieldC0;
            MARATHON_INSERT_PADDING(0x2C);
        };

        struct PlayerData
        {
            be<uint32_t> ActorID;
            be<uint32_t> RingCount;
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> LifeCount;
            be<uint32_t> ScoreCount;
            be<float> AliveTime;
            be<float> Time;
            be<float> SavePointTime;
            be<float> SectionTime;
            be<float> GaugeValue;
            be<uint32_t> MaturityLevel;
            be<float> MaturityValue;
            MARATHON_INSERT_PADDING(0x04);
            be<uint32_t> ExtendRingCount;
            be<uint32_t> GemIndex;
            MARATHON_INSERT_PADDING(0x10);
        };

        MARATHON_INSERT_PADDING(0x04);
        be<GameState> m_State;
        xpointer<DocMarathonState> m_pDoc;
        be<GameFlags> m_Flags;
        MARATHON_INSERT_PADDING(0x1C);
        SoX::Array<PlayerInfo, 15> m_PlayerInfo;
        SoX::Array<PlayerData, 4> m_PlayerData;
        SoX::Array<PlayerData, 4> m_PlayerDataCopy; // I have no idea.
        xpointer<TaskAdapter> m_pHeadUpDisplayAdapter;
        MARATHON_INSERT_PADDING(0x0C);
        xpointer<TaskAdapter> m_pNonStopTaskAdapter;
        MARATHON_INSERT_PADDING(0xBC);
        bool m_IsStage;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_Field1180;
        xpointer<GameScript> m_pGameScript;
        SoX::Array<be<uint32_t>, 15> m_PlayerActorIDs;
        boost::shared_ptr<ActorManager> m_spActorManager;
        xpointer<TextBook> m_pSystemTextBook;
        MARATHON_INSERT_PADDING(0x08);
        stdx::vector<stdx::vector<boost::shared_ptr<SoX::Scenery::Camera>>> m_Screens;
        MARATHON_INSERT_PADDING(0x1B4);
        xpointer<SoX::Audio::Cue> m_pBgmCue;
        MARATHON_INSERT_PADDING(0x90);
        xpointer<KhronoController> m_pKhronoController;
        MARATHON_INSERT_PADDING(0x2D8);
        xpointer<TextBook> m_pHintTextBook;
        MARATHON_INSERT_PADDING(0x04);
        xpointer<Mission::Core> m_pMissionCore;
        MARATHON_INSERT_PADDING(0x38);
        SoX::RefLinkNode<MovieTask> m_rMovieTask;
        MARATHON_INSERT_PADDING(0x200);
        SoX::RefLinkNode<PopupScreenTask> m_rPopupScreenTask;
        MARATHON_INSERT_PADDING(0x04);
        SoX::RefLinkNode<SoX::Engine::Task> m_rLoadingTask;
        SoX::RefLinkNode<void> m_rStageTitleTask;
        SoX::Array<SoX::RefSharedPointer<Prop::ActiveArea>, 4> m_ActiveAreas;
        SoX::Array<SoX::RefSharedPointer<Prop::GenerateArea>, 4> m_GenerateAreas;
        MARATHON_INSERT_PADDING(0x08);
        SoX::RefSharedPointer<SoX::Physics::World> m_spPhysicsWorld;
        xpointer<void> m_pMyCollisionFilter;
        MARATHON_INSERT_PADDING(0x0C);

        Player::Object* GetPlayer(uint32_t in_index) const
        {
            if (in_index >= 4)
                return nullptr;

            return m_spActorManager->Find<Player::Object>(m_PlayerData[in_index].ActorID);
        }

        int GetPlayerCount()
        {
            int result{};

            for (const auto& rPlayerInfo : m_PlayerInfo)
            {
                if (rPlayerInfo.RespawnPlayer.empty())
                    continue;

                result++;
            }

            return result;
        }

        int GetPlayerIndexFromActorID(uint32_t in_actorId) const
        {
            for (int i = 0; i < 4; i++)
            {
                if (m_PlayerData[i].ActorID == in_actorId)
                    return i;
            }

            return -1;
        }

        SoX::Scenery::CameraImp* GetCamera(const char* in_pName, int in_screenIndex = 0)
        {
            if (m_Screens.empty())
                return nullptr;

            for (const auto& spCamera : m_Screens[in_screenIndex])
            {
                const auto pCameraImp = reinterpret_cast<SoX::Scenery::CameraImp*>(spCamera.get());

                if (pCameraImp->m_Name == in_pName)
                    return pCameraImp;
            }

            return nullptr;
        }

        template <typename T = SoX::Audio::Cue>
        T* GetBgmCue() const
        {
            return static_cast<T*>(m_pBgmCue.get());
        }

        template <typename T = SoX::Physics::World>
        T* GetPhysicsWorld() const
        {
            return static_cast<T*>(m_spPhysicsWorld.get());
        }
    };

    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, StartPlayer, 0x00);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, StartRotation, 0x20);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, StartPosition, 0x30);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, Field40, 0x40);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, RespawnPlayer, 0x44);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, RespawnRotation, 0x60);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, RespawnPosition, 0x70);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, Field80, 0x80);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, Field90, 0x90);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, FieldA0, 0xA0);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, FieldB0, 0xB0);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerInfo, FieldC0, 0xC0);
    MARATHON_ASSERT_SIZEOF(GameImp::PlayerInfo, 0xF0);

    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, ActorID, 0x00);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, RingCount, 0x04);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, LifeCount, 0x0C);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, ScoreCount, 0x10);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, AliveTime, 0x14);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, Time, 0x18);
    MARATHON_ASSERT_OFFSETOF(GameImp::PlayerData, SavePointTime, 0x1C);
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
    MARATHON_ASSERT_OFFSETOF(GameImp, m_PlayerInfo, 0x30);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_PlayerData, 0xE40);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_PlayerDataCopy, 0xF70);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pHeadUpDisplayAdapter, 0x10A0);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pNonStopTaskAdapter, 0x10B0);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_IsStage, 0x1170);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_Field1180, 0x1180);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pGameScript, 0x1184);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_PlayerActorIDs, 0x1188);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_spActorManager, 0x11C4);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pSystemTextBook, 0x11CC);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_Screens, 0x11D8);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pBgmCue, 0x139C);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pKhronoController, 0x1430);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pHintTextBook, 0x170C);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pMissionCore, 0x1714);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_rMovieTask, 0x1750);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_rPopupScreenTask, 0x1960);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_rLoadingTask, 0x1974);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_rStageTitleTask, 0x1984);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_ActiveAreas, 0x1994);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_GenerateAreas, 0x19A4);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_spPhysicsWorld, 0x19BC);
    MARATHON_ASSERT_OFFSETOF(GameImp, m_pMyCollisionFilter, 0x19C0);
    MARATHON_ASSERT_SIZEOF(GameImp, 0x19D0);
}
