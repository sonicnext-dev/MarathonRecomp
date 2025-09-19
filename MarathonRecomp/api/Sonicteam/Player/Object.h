#pragma once

#include <Marathon.inl>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/Player/State/Machine2.h>
#include <Sonicteam/Player/IGauge.h>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/RootFrame.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <stdx/vector.h>

namespace Sonicteam::Player
{
    class Object : public Actor
    {
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
        xpointer<Sonicteam::SoX::MessageReceiver> m_pCameraman; //here as MessageReceiver
        be<uint32_t> m_PlayerIndex;
        MARATHON_INSERT_PADDING(0x2C);
        bool m_IsPlayer;
        bool m_IsPosture;
        bool m_IsAmigo;
        MARATHON_INSERT_PADDING(1);
        SoX::RefSharedPointer<RootFrame> m_spRootFrame;
        MARATHON_INSERT_PADDING(0x14);
        boost::shared_ptr<State::Machine2> m_spStateMachine;
        MARATHON_INSERT_PADDING(0x10);
        be<uint32_t> m_SetupModuleIndexPrefix;
        be<uint32_t> m_SetupModuleIndexPostfix;
        boost::shared_ptr<IGauge> m_spGauge;
        MARATHON_INSERT_PADDING(8);
        stdx::vector<boost::shared_ptr<IPlugIn>> m_vspPlayerPlugins;
        MARATHON_INSERT_PADDING(0x1F4);

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
