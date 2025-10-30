#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>
#include <Sonicteam/CsdManager.h>

namespace Sonicteam
{
    class CsdResource : public SoX::IResource2<CsdResource, CsdManager>
    {
    public:
        MARATHON_INSERT_PADDING(4);
    };
}
