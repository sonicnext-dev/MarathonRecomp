#pragma once

#include <Marathon.inl>
#include <Sonicteam/Player/IPlugIn.h>
#include <Sonicteam/Player/IVariable.h>
#include <Sonicteam/Player/IStepable.h>

namespace Sonicteam::Player
{
    class IGauge : public IPlugIn, public IVariable, public IStepable {};
}
