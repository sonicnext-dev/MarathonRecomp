#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam
{
    class CsdManager : public SoX::IResourceMgr, public System::Singleton<CsdManager, 0x82D3BC58, System::CreateStatic<CsdManager, 0x825E9530>>
    {
    public:
        MARATHON_INSERT_PADDING(0x1C);
    };

    MARATHON_ASSERT_SIZEOF(CsdManager, 0x2C);
}
