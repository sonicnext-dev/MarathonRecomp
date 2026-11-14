#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResourceMgr.h>
#include <Sonicteam/System/CreateStatic.h>
#include <Sonicteam/System/Singleton.h>

namespace Sonicteam::Prop
{
    class SceneDataMgr : public SoX::IResourceMgr, public System::Singleton<SceneDataMgr, 0x8245C048, System::CreateStatic<SceneDataMgr, 0x8245C048>> {};

    MARATHON_ASSERT_SIZEOF(SceneDataMgr, 0x10);
}
