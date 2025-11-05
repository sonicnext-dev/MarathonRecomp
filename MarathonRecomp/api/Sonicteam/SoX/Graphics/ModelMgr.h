#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam::SoX::Graphics
{
    class ModelMgr : public IResourceMgr, System::Singleton<ModelMgr, 0x82D36EF4, System::CreateStatic<ModelMgr, 0x822DE058>>
    {

    };
}
