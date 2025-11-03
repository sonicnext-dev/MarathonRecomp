#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/Camera/Cameraman.h>
#include <Sonicteam/Player/State/Machine2.h>
#include <Sonicteam/Player/IGauge.h>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/RootFrame.h>
#include <Sonicteam/Player/IPostureControl.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <stdx/vector.h>

namespace Sonicteam::Player
{
    class Object : public Actor
    {
    public:
        struct CreationParams
        {
            xpointer<const char> pPlayerLua;
            MARATHON_INSERT_PADDING(0x2C);
            SoX::Math::Vector Position;
        };

        struct EquipFlags
        {
            be<uint32_t> GlobalFlag;
            be<uint32_t> EquipFlag;
        };

        stdx::string m_LuaFile;
        stdx::string m_PackageFile;
        be<uint32_t> m_TargetCameraActorID; 
        xpointer<SoX::MessageReceiver> m_pCameraman;
        be<uint32_t> m_Index;
        be<uint32_t> m_ControllerIndex;
        SoX::Math::Quaternion m_SpawnRotation;
        SoX::Math::Vector m_SpawnPosition;
        be<uint32_t> m_SpawnRingCount;
        SoX::RefSharedPointer<> m_spSpawnSource;
        bool m_IsPlayer;
        bool m_IsPosture;
        bool m_IsAmigo;
        MARATHON_INSERT_PADDING(1);
        SoX::RefSharedPointer<RootFrame> m_spRootFrame;
        SoX::RefSharedPointer<> m_spPackageBinary;
        boost::shared_ptr<void> m_spModel;
        boost::shared_ptr<IPostureControl> m_spPostureControl;
        boost::shared_ptr<State::Machine2> m_spStateMachine;
        boost::shared_ptr<void> m_spGravity;
        boost::shared_ptr<void> m_spImpulse;
        be<uint32_t> m_SetupModuleIndexPrefix;
        be<uint32_t> m_SetupModuleIndexPostfix;
        boost::shared_ptr<IGauge> m_spGauge;
        MARATHON_INSERT_PADDING(8);
        stdx::vector<boost::shared_ptr<IPlugIn>> m_vspPlugins;
        MARATHON_INSERT_PADDING(0x58);
        be<float> m_DeltaTime;
        MARATHON_INSERT_PADDING(0x48);
        stdx::vector<EquipFlags> m_vEquipFlags;
        stdx::string m_Name;
        MARATHON_INSERT_PADDING(0x110);

        template <typename T = IGauge>
        T* GetGauge()
        {
            return (T*)m_spGauge.get();
        }

        template <typename T = IPlugIn>
        inline T* GetPlugin(const char* pluginName)
        {
            for (auto& spPlugin : m_vspPlugins)
            {
                if (spPlugin->m_Name == pluginName)
                    return static_cast<T*>(spPlugin.get());
            }

            return nullptr;
        }

        SoX::Input::Manager* GetInputManager()
        {
            if (!m_IsPlayer)
                return nullptr;

            auto pDoc = GetDoc<DocMarathonState>();
            auto pGame = pDoc->GetDocMode<GameMode>()->GetGame();
            auto playerIndex = pGame->PlayerActorIDToIndex(m_ActorID);
            auto controllerID = pDoc->m_PlayerControllerID[playerIndex];

            return pDoc->m_vspInputManager[controllerID].get();
        }
    };

    MARATHON_ASSERT_OFFSETOF(Object::CreationParams, pPlayerLua, 0x00);
    MARATHON_ASSERT_OFFSETOF(Object::CreationParams, Position, 0x30);

    MARATHON_ASSERT_OFFSETOF(Object::EquipFlags, GlobalFlag, 0x00);
    MARATHON_ASSERT_OFFSETOF(Object::EquipFlags, EquipFlag, 0x04);

    MARATHON_ASSERT_OFFSETOF(Object, m_LuaFile, 0x58);
    MARATHON_ASSERT_OFFSETOF(Object, m_PackageFile, 0x74);
    MARATHON_ASSERT_OFFSETOF(Object, m_TargetCameraActorID, 0x90);
    MARATHON_ASSERT_OFFSETOF(Object, m_pCameraman, 0x94);
    MARATHON_ASSERT_OFFSETOF(Object, m_Index, 0x98);
    MARATHON_ASSERT_OFFSETOF(Object, m_ControllerIndex, 0x9C);
    MARATHON_ASSERT_OFFSETOF(Object, m_SpawnRotation, 0xA0);
    MARATHON_ASSERT_OFFSETOF(Object, m_SpawnPosition, 0xB0);
    MARATHON_ASSERT_OFFSETOF(Object, m_SpawnRingCount, 0xC0);
    MARATHON_ASSERT_OFFSETOF(Object, m_spSpawnSource, 0xC4);
    MARATHON_ASSERT_OFFSETOF(Object, m_IsPlayer, 0xC8);
    MARATHON_ASSERT_OFFSETOF(Object, m_IsPosture, 0xC9);
    MARATHON_ASSERT_OFFSETOF(Object, m_IsAmigo, 0xCA);
    MARATHON_ASSERT_OFFSETOF(Object, m_spRootFrame, 0xCC);
    MARATHON_ASSERT_OFFSETOF(Object, m_spPackageBinary, 0xD0);
    MARATHON_ASSERT_OFFSETOF(Object, m_spModel, 0xD4);
    MARATHON_ASSERT_OFFSETOF(Object, m_spPostureControl, 0xDC);
    MARATHON_ASSERT_OFFSETOF(Object, m_spStateMachine, 0xE4);
    MARATHON_ASSERT_OFFSETOF(Object, m_spGravity, 0xEC);
    MARATHON_ASSERT_OFFSETOF(Object, m_spImpulse, 0xF4);
    MARATHON_ASSERT_OFFSETOF(Object, m_SetupModuleIndexPrefix, 0xFC);
    MARATHON_ASSERT_OFFSETOF(Object, m_SetupModuleIndexPostfix, 0x100);
    MARATHON_ASSERT_OFFSETOF(Object, m_spGauge, 0x104);
    MARATHON_ASSERT_OFFSETOF(Object, m_vspPlugins, 0x114);
    MARATHON_ASSERT_OFFSETOF(Object, m_DeltaTime, 0x17C);
    MARATHON_ASSERT_OFFSETOF(Object, m_vEquipFlags, 0x1C8);
    MARATHON_ASSERT_OFFSETOF(Object, m_Name, 0x1D8);
    MARATHON_ASSERT_SIZEOF(Object, 0x310);
}
