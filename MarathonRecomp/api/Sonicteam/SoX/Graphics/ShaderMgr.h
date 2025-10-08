#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>


namespace Sonicteam::SoX::Graphics
{
    class ShaderMgr : public IResourceMgr, public System::Singleton<ShaderMgr, 0x82D37070, System::CreateStatic<ShaderMgr, 0x82370230>>
    {
    };
}
