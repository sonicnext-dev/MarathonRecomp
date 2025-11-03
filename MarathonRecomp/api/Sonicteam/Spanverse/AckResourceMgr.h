#pragma once

#include <Marathon.inl>
#include <SoX/IResourceMgr.h>
#include <System/Singleton.h>
#include <System/CreateStatic.h>

namespace Sonicteam::Spanverse
{
    class AckResourceMgr : public SoX::IResourceMgr, public System::Singleton<AckResourceMgr, System::CreateStatic<AckResourceMgr>> {};
}
