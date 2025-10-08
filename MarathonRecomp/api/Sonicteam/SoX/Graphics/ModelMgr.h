#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>


namespace Sonicteam::SoX::Graphics
{
    class ModelMgr : public IResourceMgr, public System::Singleton<ModelMgr, 0x82D36F44, System::CreateStatic<ModelMgr, 0x822DE058>>
    {
    };
}
