#pragma once

#include <Marathon.h>
#include <Sonicteam/Player/State/Machine2.h>
#include <boost/smart_ptr/shared_ptr.h>
#include <Sonicteam/Player/IPlugIn.h>
#include <stdx/vector.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/Player/RootFrame.h>

namespace Sonicteam::Player
{
    class Object : public Actor
    {
    public:
        stdx::string m_PlayerLua;
        stdx::string m_PlayerPackage;
        MARATHON_INSERT_PADDING(8);
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

        template <typename T>
        inline T* GetGauge();

        template <typename T = IPlugIn>
        inline T* GetPlugin(const char* pluginName);
    };
}

#include <Sonicteam/Player/Object.inl>
