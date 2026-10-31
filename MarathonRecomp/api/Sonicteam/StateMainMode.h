#pragma once

#include <Marathon.inl>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/DocState.h>

class StateMainMode : public Sonicteam::DocState, public Sonicteam::System::Singleton<StateMainMode, 0x82D35E48, Sonicteam::System::CreateStatic<StateMainMode, 0x82161150>> {};

MARATHON_ASSERT_SIZEOF(StateMainMode, 0x04);
