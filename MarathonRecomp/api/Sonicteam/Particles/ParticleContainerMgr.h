#pragma once

#include <Marathon.inl>
#include <SoX/IResourceMgr.h>
#include <System/Singleton.h>
#include <System/CreateStatic.h>

namespace Sonicteam::Particles
{
    class ParticleContainerMgr : public SoX::IResourceMgr, public System::Singleton<ParticleContainerMgr, 0x82D3BD1C, System::CreateStatic<ParticleContainerMgr>, 0x825FC468> {};
}
