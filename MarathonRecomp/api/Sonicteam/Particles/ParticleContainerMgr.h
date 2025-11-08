#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam::Particles
{
    class ParticleContainerMgr : public SoX::IResourceMgr, public System::Singleton<ParticleContainerMgr, 0x82D3BD1C, System::CreateStatic<ParticleContainerMgr, 0x825FC468>> {};
}
