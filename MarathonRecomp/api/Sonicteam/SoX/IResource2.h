#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource.h>

namespace Sonicteam::SoX
{
    template <class TRes = IResource, class TMgr = IResourceMgr>
    class IResource2 : public IResource {};
}
