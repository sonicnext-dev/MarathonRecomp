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
            uint32_t GlobalFlag;
            uint32_t EquipFlag;
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
        MARATHON_INSERT_PADDING(0x134);

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
}
