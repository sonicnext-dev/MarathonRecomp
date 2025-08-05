#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>

namespace Sonicteam
{
    class CsdManager : public SoX::IResourceMgr
    {
    public:
        MARATHON_INSERT_PADDING(0x1C);

        static CsdManager* GetInstance();
    };
}

#include <Sonicteam/CsdManager.inl>
