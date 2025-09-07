#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/Player/IDynamicLink.h>
#include <Sonicteam/Player/IFlagCommunicator.h>

namespace Sonicteam::Player::State
{
    class IContext : public IPlugIn, public IVariable, public IDynamicLink, public IFlagCommunicator {};
}
