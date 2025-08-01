#pragma once

#include <Marathon.inl>

namespace Sonicteam::Player::State
{
    class IContext : public IPlugIn, public IVariable, public IDynamicLink, public IFlagCommunicator {};
}
