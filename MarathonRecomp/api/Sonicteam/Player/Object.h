#pragma once

#include <Marathon.h>
#include <Sonicteam/Player/State/Machine2.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/Player/IPlugIn.h>
#include <stdx/vector.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/Player/RootFrame.h>
#include <stdx/vector.h>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/Math/Quaternion.h>

namespace Sonicteam::Player
{
    #pragma pack(push, 1)
    class __declspec(align(1)) Object : public Actor
    {
        struct ObjectPlayerUpgrade 
        {
            uint32_t global_flag;
            uint32_t equip_flag;
        };

    public:
        stdx::string m_PlayerLua;
        stdx::string m_PlayerPackage;
        be<uint32_t> m_CameramanActorID;
        xpointer<uint32_t> m_Cameraman;
        be<uint32_t> m_PlayerIndex;
        be<uint32_t> m_PlayerControllerIndex;
		SoX::Math::Quaternion m_SpawnRotation;
        SoX::Math::Vector m_SpawnPosition;
        be<uint32_t> m_SpawnRing;
        SoX::RefSharedPointer<SoX::RefCountObject> m_spSpawnSource; // REF_TYPE(RefCountObject)
        bool m_IsPlayer;
        bool m_IsPosture;
        bool m_IsAmigo;
        MARATHON_INSERT_PADDING(1);
        SoX::RefSharedPointer<RootFrame> m_spRootFrame;
		SoX::RefSharedPointer<RefCountObject> m_spPackageBinary;
        boost::shared_ptr<uint32_t> m_spPlayerModel;
        boost::shared_ptr<uint32_t> m_spPlayerPosture;
        boost::shared_ptr<State::Machine2> m_spStateMachine;
        boost::shared_ptr<uint32_t> m_spPlayerGravity;
        boost::shared_ptr<uint32_t> m_spPlayerImpulse;
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
		
        template <typename T>
        inline T* GetGauge();
		
        template <typename T = IPlugIn>
        inline T* GetPlugin(const char* pluginName);
		
		template <typename T = Sonicteam::Player::IPlugIn>
		inline T* GetPlugin(const char* v_name);
		
    };
    #pragma pack(pop) 
}

#include <Sonicteam/Player/Object.inl>
