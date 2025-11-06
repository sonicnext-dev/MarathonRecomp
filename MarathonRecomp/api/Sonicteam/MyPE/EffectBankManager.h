#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>

namespace Sonicteam::MyPE
{
    class EffectBankLoadManager : public SoX::IResourceMgr, System::Singleton<EffectBankLoadManager, 0x82D3C53C, System::CreateStatic<EffectBankLoadManager, 0>>
    {

    };
}
