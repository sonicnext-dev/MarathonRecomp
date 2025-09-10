#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/IFlagCommunicator.h>
#include <Sonicteam/Player/IStepable.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/Player/INotification.h>
#include <Sonicteam/SoX/Engine/Task.h>
#include <Sonicteam/SoX/LinkNode.h>

namespace Sonicteam::Player::Weapon
{  
    class SonicWeapons : public IPlugIn, public IFlagCommunicator, public IStepable, public IDynamicLink, public IVariable, public INotification
    {
    public:
        MARATHON_INSERT_PADDING(0x4C);
        SoX::LinkRef<SoX::Engine::Task> m_GunDrive;
        MARATHON_INSERT_PADDING(0x24);
    };
}
