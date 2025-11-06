#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/Singleton.h>
#include <Sonicteam/System/CreateStatic.h>

namespace Sonicteam::Spanverse
{
    class AckResourceMgr : public SoX::IResourceMgr, public System::Singleton<AckResourceMgr, 0x82D3C4F8, System::CreateStatic<AckResourceMgr, 0x82642600>>
    {

    };
}
