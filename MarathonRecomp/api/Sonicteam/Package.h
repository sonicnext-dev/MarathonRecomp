#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>

namespace Sonicteam
{
    class PackageMgr;

    class Package : public SoX::IResource2<Sonicteam::Package, Sonicteam::PackageMgr> {};

    MARATHON_ASSERT_SIZEOF(Package, 0x64);
}
