#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
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
        struct ObjectPlayerUpgrade
        {
            uint32_t global_flag;
            uint32_t equip_flag;
        };

    public:
        class CreationParams
        {
        public:
            xpointer<const char> m_pPlayerLua;
            MARATHON_INSERT_PADDING(0x2C);
            SoX::Math::Vector m_Position;
        };

        stdx::string m_PlayerLua;
        stdx::string m_PlayerPackage;
        be<uint32_t> m_TargetCameraActorID; 
        xpointer<MessageReceiver> m_pCameraman; // Here as MessageReceiver
        be<uint32_t> m_PlayerIndex;
        be<uint32_t> m_PlayerControllerIndex;
        SoX::Math::Quaternion m_SpawnRotation;
        SoX::Math::Vector m_SpawnPosition;
        be<uint32_t> m_SpawnRing;
        SoX::RefSharedPointer<> m_spSpawnSource; // REF_TYPE(RefCountObject)
        bool m_IsPlayer;
        bool m_IsPosture;
        bool m_IsAmigo;
        MARATHON_INSERT_PADDING(1);
        SoX::RefSharedPointer<RootFrame> m_spRootFrame;
        SoX::RefSharedPointer<> m_spPackageBinary;
        boost::shared_ptr<void> m_spPlayerModel;
        boost::shared_ptr<IPostureControl> m_spPlayerPosture;
        boost::shared_ptr<State::Machine2> m_spStateMachine;
        boost::shared_ptr<void> m_spPlayerGravity;
        boost::shared_ptr<void> m_spPlayerImpulse;
        be<uint32_t> m_SetupModuleIndexPrefix;
        be<uint32_t> m_SetupModuleIndexPostfix;
        boost::shared_ptr<IGauge> m_spGauge;
        MARATHON_INSERT_PADDING(8);
        stdx::vector<boost::shared_ptr<IPlugIn>> m_vspPlayerPlugins;
        MARATHON_INSERT_PADDING(0x58);
        be<float> m_PlayerDelta;
        MARATHON_INSERT_PADDING(0x48);
        stdx::vector<ObjectPlayerUpgrade> m_PlayerUpgrade;
        stdx::string m_PlayerName;
        MARATHON_INSERT_PADDING(0x134);

        template <typename T = IGauge>
        T* GetGauge()
        {
            return (T*)m_spGauge.get();
        }

        template <typename T = IPlugIn>
        inline T* GetPlugin(const char* pluginName)
        {
            for (auto& spPlugin : m_vspPlayerPlugins)
            {
                if (spPlugin->m_Name == pluginName)
                    return static_cast<T*>(spPlugin.get());
            }

            return nullptr;
        }
    };
}
