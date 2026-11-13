#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam::SoX::Audio
{
    class SoundBankMgr : public IResourceMgr, public System::Singleton<SoundBankMgr, 0x82D36894, System::CreateStatic<SoundBankMgr, 0x8216D228>> {};

    MARATHON_ASSERT_SIZEOF(SoundBankMgr, 0x10);
}
